// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "HDWallet.h"

#include "Base58.h"
#include "BinaryCoding.h"
#include "Bitcoin/SegwitAddress.h"
#include "Bitcoin/CashAddress.h"
#include "Coin.h"
#include "Mnemonic.h"

#include <TrustWalletCore/TWHRP.h>
#include <TrezorCrypto/bip32.h>
#include <TrezorCrypto/bip39.h>
#include <TrezorCrypto/curves.h>

#include <array>

using namespace TW;

namespace {

uint32_t fingerprint(HDNode *node, Hash::Hasher hasher);
std::string serialize(const HDNode *node, uint32_t fingerprint, uint32_t version, bool use_public, Hash::Hasher hasher);
bool deserialize(const std::string& extended, TWCurve curve, Hash::Hasher hasher, HDNode *node);
HDNode getNode(const HDWallet& wallet, TWCurve curve, const DerivationPath& derivationPath);
HDNode getMasterNode(const HDWallet& wallet, TWCurve curve);

const char* curveName(TWCurve curve);
} // namespace

const size_t MnemonicBufLength = Mnemonic::MaxWords * 10;

HDWallet::HDWallet(int strength, const std::string& passphrase)
    : passphrase(passphrase) {
    char buf[MnemonicBufLength];
    const char* mnemonic_chars = mnemonic_generate(strength, buf, MnemonicBufLength);
    if (mnemonic_chars == nullptr) {
        throw std::invalid_argument("Invalid strength");
    }
    mnemonic = mnemonic_chars;
    assert(Mnemonic::isValid(mnemonic));
    updateSeedAndEntropy();
}

HDWallet::HDWallet(const std::string& mnemonic, const std::string& passphrase)
    : mnemonic(mnemonic), passphrase(passphrase) {
    if (!Mnemonic::isValid(mnemonic)) {
        throw std::invalid_argument("Invalid mnemonic");
    }
    assert(Mnemonic::isValid(mnemonic));
    updateSeedAndEntropy();
}

HDWallet::HDWallet(const Data& entropy, const std::string& passphrase)
    : passphrase(passphrase) {
    char buf[MnemonicBufLength];
    const char* mnemonic_chars = mnemonic_from_data(entropy.data(), static_cast<int>(entropy.size()), buf, MnemonicBufLength);
    if (mnemonic_chars == nullptr) {
        throw std::invalid_argument("Invalid mnemonic data");
    }
    mnemonic = mnemonic_chars;
    assert(Mnemonic::isValid(mnemonic));
    updateSeedAndEntropy();
}

HDWallet::~HDWallet() {
    std::fill(seed.begin(), seed.end(), 0);
    std::fill(mnemonic.begin(), mnemonic.end(), 0);
    std::fill(passphrase.begin(), passphrase.end(), 0);
}

void HDWallet::updateSeedAndEntropy() {
    // generate seed from mnemonic
    // it is assumed that mnemonic is valid, enforced before calling
    mnemonic_to_seed(mnemonic.c_str(), passphrase.c_str(), seed.data(), nullptr);

    // generate entropy from mnemonic
    Data entropyRaw((Mnemonic::MaxWords * 11) / 8);
    auto entropyBytes = mnemonic_to_bits(mnemonic.c_str(), entropyRaw.data()) / 8;
    assert(entropyBytes <= ((Mnemonic::MaxWords * 11) / 8) && entropyBytes >= ((Mnemonic::MinWords * 11) / 8));
    // copy to truncate
    entropy = data(entropyRaw.data(), entropyBytes);
    assert(entropy.size() > 10);
}

PrivateKey HDWallet::getMasterKey(TWCurve curve) const {
    auto node = getMasterNode(*this, curve);
    auto data = Data(node.private_key, node.private_key + PrivateKey::size);
    return PrivateKey(data);
}

PrivateKey HDWallet::getMasterKeyExtension(TWCurve curve) const {
    auto node = getMasterNode(*this, curve);
    auto data = Data(node.private_key_extension, node.private_key_extension + PrivateKey::size);
    return PrivateKey(data);
}

PrivateKey HDWallet::getKey(TWCoinType coin, const DerivationPath& derivationPath) const {
    const auto curve = TWCoinTypeCurve(coin);
    const auto privateKeyType = getPrivateKeyType(curve);
    auto node = getNode(*this, curve, derivationPath);
    switch (privateKeyType) {
        case PrivateKeyTypeExtended96:
            {
                auto pkData = Data(node.private_key, node.private_key + PrivateKey::size);
                auto extData = Data(node.private_key_extension, node.private_key_extension + PrivateKey::size);
                auto chainCode = Data(node.chain_code, node.chain_code + PrivateKey::size);
                return PrivateKey(pkData, extData, chainCode);
            }

        case PrivateKeyTypeDefault32:
        default:
            // default path
            auto data = Data(node.private_key, node.private_key + PrivateKey::size);
            return PrivateKey(data);
    }
}

std::string HDWallet::deriveAddress(TWCoinType coin) const {
    const auto derivationPath = TW::derivationPath(coin);
    return TW::deriveAddress(coin, getKey(coin, derivationPath));
}

std::string HDWallet::getExtendedPrivateKey(TWPurpose purpose, TWCoinType coin, TWHDVersion version) const {
    if (version == TWHDVersionNone) {
        return "";
    }
    
    const auto curve = TWCoinTypeCurve(coin);
    auto derivationPath = TW::DerivationPath({DerivationPathIndex(purpose, true), DerivationPathIndex(coin, true)});
    auto node = getNode(*this, curve, derivationPath);
    auto fingerprintValue = fingerprint(&node, publicKeyHasher(coin));
    hdnode_private_ckd(&node, 0x80000000);
    return serialize(&node, fingerprintValue, version, false, base58Hasher(coin));
}

std::string HDWallet::getExtendedPublicKey(TWPurpose purpose, TWCoinType coin, TWHDVersion version) const {
    if (version == TWHDVersionNone) {
        return "";
    }
    
    const auto curve = TWCoinTypeCurve(coin);
    auto derivationPath = TW::DerivationPath({DerivationPathIndex(purpose, true), DerivationPathIndex(coin, true)});
    auto node = getNode(*this, curve, derivationPath);
    auto fingerprintValue = fingerprint(&node, publicKeyHasher(coin));
    hdnode_private_ckd(&node, 0x80000000);
    hdnode_fill_public_key(&node);
    return serialize(&node, fingerprintValue, version, true, base58Hasher(coin));
}

std::optional<PublicKey> HDWallet::getPublicKeyFromExtended(const std::string& extended, TWCoinType coin, const DerivationPath& path) {
    const auto curve = TW::curve(coin);
    const auto hasher = TW::base58Hasher(coin);

    auto node = HDNode{};
    if (!deserialize(extended, curve, hasher, &node)) {
        return {};
    }
    if (node.curve->params == nullptr) {
        return {};
    }
    hdnode_public_ckd(&node, path.change());
    hdnode_public_ckd(&node, path.address());
    hdnode_fill_public_key(&node);

    // These public key type are not applicable.  Handled above, as node.curve->params is null
    assert(curve != TWCurveED25519 && curve != TWCurveED25519Blake2bNano && curve != TWCurveED25519Extended && curve != TWCurveCurve25519);
    TWPublicKeyType keyType = TW::publicKeyType(coin);
    if (curve == TWCurveSECP256k1) {
        assert(keyType == TWPublicKeyTypeSECP256k1);
        return PublicKey(Data(node.public_key, node.public_key + 33), TWPublicKeyTypeSECP256k1);
    }
    assert(curve == TWCurveNIST256p1);
    assert(keyType == TWPublicKeyTypeNIST256p1);
    return PublicKey(Data(node.public_key, node.public_key + 33), TWPublicKeyTypeNIST256p1);
}

std::optional<PrivateKey> HDWallet::getPrivateKeyFromExtended(const std::string& extended, TWCoinType coin, const DerivationPath& path) {
    const auto curve = TW::curve(coin);
    const auto hasher = TW::base58Hasher(coin);

    auto node = HDNode{};
    if (!deserialize(extended, curve, hasher, &node)) {
        return {};
    }
    hdnode_private_ckd(&node, path.change());
    hdnode_private_ckd(&node, path.address());

    return PrivateKey(Data(node.private_key, node.private_key + 32));
}

HDWallet::PrivateKeyType HDWallet::getPrivateKeyType(TWCurve curve) {
    switch (curve) {
    case TWCurve::TWCurveED25519Extended:
        // used by Cardano
        return PrivateKeyTypeExtended96;
    default:
        // default
        return PrivateKeyTypeDefault32;
    }
}

namespace {

uint32_t fingerprint(HDNode *node, Hash::Hasher hasher) {
    hdnode_fill_public_key(node);
    auto digest = hasher(node->public_key, 33);
    return ((uint32_t) digest[0] << 24) + (digest[1] << 16) + (digest[2] << 8) + digest[3];
}

std::string serialize(const HDNode *node, uint32_t fingerprint, uint32_t version, bool use_public, Hash::Hasher hasher) {
    Data node_data;
    node_data.reserve(78);

    encode32BE(version, node_data);
    node_data.push_back(static_cast<uint8_t>(node->depth));
    encode32BE(fingerprint, node_data);
    encode32BE(node->child_num, node_data);
    node_data.insert(node_data.end(), node->chain_code, node->chain_code + 32);
    if (use_public) {
        node_data.insert(node_data.end(), node->public_key, node->public_key + 33);
    } else {
        node_data.push_back(0);
        node_data.insert(node_data.end(), node->private_key, node->private_key + 32);
    }

    return Base58::bitcoin.encodeCheck(node_data, hasher);
}

bool deserialize(const std::string& extended, TWCurve curve, Hash::Hasher hasher, HDNode* node) {
    memset(node, 0, sizeof(HDNode));
    const char* curveNameStr = curveName(curve);
    if (curveNameStr == nullptr || ::strlen(curveNameStr) == 0) {
        return false;
    }
    node->curve = get_curve_by_name(curveNameStr);
    assert(node->curve != nullptr);

    const auto node_data = Base58::bitcoin.decodeCheck(extended, hasher);
    if (node_data.size() != 78) {
        return false;
    }

    uint32_t version = decode32BE(node_data.data());
    if (TWHDVersionIsPublic(static_cast<TWHDVersion>(version))) {
        std::copy(node_data.begin() + 45, node_data.begin() + 45 + 33, node->public_key);
    } else if (TWHDVersionIsPrivate(static_cast<TWHDVersion>(version))) {
        if (node_data[45]) { // invalid data
            return false;
        }
        std::copy(node_data.begin() + 46, node_data.begin() + 46 + 32, node->private_key);
    } else {
        return false; // invalid version
    }
    node->depth = node_data[4];
    node->child_num = decode32BE(node_data.data() + 9);
    std::copy(node_data.begin() + 13, node_data.begin() + 13 + 32, node->chain_code);
    return true;
}

HDNode getNode(const HDWallet& wallet, TWCurve curve, const DerivationPath& derivationPath) {
    const auto privateKeyType = HDWallet::getPrivateKeyType(curve);
    auto node = getMasterNode(wallet, curve);
    for (auto& index : derivationPath.indices) {
        switch (privateKeyType) {
            case HDWallet::PrivateKeyTypeExtended96:
                // special handling for extended
                hdnode_private_ckd_cardano(&node, index.derivationIndex());
                break;
            case HDWallet::PrivateKeyTypeDefault32:
            default:
                hdnode_private_ckd(&node, index.derivationIndex());
                break;
        }
    }
    return node;
}

HDNode getMasterNode(const HDWallet& wallet, TWCurve curve) {
    const auto privateKeyType = HDWallet::getPrivateKeyType(curve);
    auto node = HDNode();
    switch (privateKeyType) {
        case HDWallet::PrivateKeyTypeExtended96:
            // special handling for extended, use entropy (not seed)
            hdnode_from_entropy_cardano_icarus((const uint8_t*)"", 0, wallet.entropy.data(), (int)wallet.entropy.size(), &node);
            break;
        case HDWallet::PrivateKeyTypeDefault32:
        default:
            hdnode_from_seed(wallet.seed.data(), HDWallet::seedSize, curveName(curve), &node);
            break;
    }
    return node;
}

const char* curveName(TWCurve curve) {
    switch (curve) {
    case TWCurveSECP256k1:
        return SECP256K1_NAME;
    case TWCurveED25519:
        return ED25519_NAME;
    case TWCurveED25519Blake2bNano:
        return ED25519_BLAKE2B_NANO_NAME;
    case TWCurveED25519Extended:
        return ED25519_CARDANO_NAME;
    case TWCurveNIST256p1:
        return NIST256P1_NAME;
    case TWCurveCurve25519:
        return CURVE25519_NAME;
    case TWCurveNone:
    default:
        return "";
    }
}

} // namespace
