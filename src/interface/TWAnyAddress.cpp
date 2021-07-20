// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <TrustWalletCore/TWAnyAddress.h>
#include <TrustWalletCore/TWPublicKey.h>
#include <TrezorCrypto/cash_addr.h>

#include "../Bitcoin/Address.h"
#include "../Bitcoin/CashAddress.h"
#include "../Bitcoin/SegwitAddress.h"
#include "../Cosmos/Address.h"
#include "../Decred/Address.h"
#include "../Kusama/Address.h"
#include "../Polkadot/Address.h"
#include "../Zcash/TAddress.h"
#include "../Zilliqa/Address.h"
#include "../Cardano/AddressV3.h"
#include "../NEO/Address.h"
#include "../Nano/Address.h"
#include "../Elrond/Address.h"
#include "../NEAR/Address.h"
#include "../Avalanche/Address.h"

#include "../Coin.h"
#include "../HexCoding.h"

using namespace TW;

struct TWAnyAddress {
    TWString* address;
    enum TWCoinType coin;
};

bool TWAnyAddressEqual(struct TWAnyAddress* _Nonnull lhs, struct TWAnyAddress* _Nonnull rhs) {
    return TWStringEqual(lhs->address, rhs->address) && lhs->coin == rhs->coin;
}

bool TWAnyAddressIsValid(TWString* _Nonnull string, enum TWCoinType coin) {
    const auto& address = *reinterpret_cast<const std::string*>(string);
    return TW::validateAddress(coin, address);
}

struct TWAnyAddress* _Nullable TWAnyAddressCreateWithString(TWString* _Nonnull string,
                                                            enum TWCoinType coin) {
    const auto& address = *reinterpret_cast<const std::string*>(string);
    auto normalized = TW::normalizeAddress(coin, address);
    if (normalized.empty()) { return nullptr; }
    return new TWAnyAddress{TWStringCreateWithUTF8Bytes(normalized.c_str()), coin};
}

struct TWAnyAddress* _Nonnull TWAnyAddressCreateWithPublicKey(
    struct TWPublicKey* _Nonnull publicKey, enum TWCoinType coin) {
    auto address = TW::deriveAddress(coin, publicKey->impl);
    return new TWAnyAddress{TWStringCreateWithUTF8Bytes(address.c_str()), coin};
}

void TWAnyAddressDelete(struct TWAnyAddress* _Nonnull address) {
    TWStringDelete(address->address);
    delete address;
}

TWString* _Nonnull TWAnyAddressDescription(struct TWAnyAddress* _Nonnull address) {
    return TWStringCreateWithUTF8Bytes(TWStringUTF8Bytes(address->address));
}

enum TWCoinType TWAnyAddressCoin(struct TWAnyAddress* _Nonnull address) {
    return address->coin;
}

TWData* _Nonnull TWAnyAddressData(struct TWAnyAddress* _Nonnull address) {
    auto& string = *reinterpret_cast<const std::string*>(address->address);
    Data data;
    switch (address->coin) {
    case TWCoinTypeBinance:
    case TWCoinTypeCosmos:
    case TWCoinTypeKava:
    case TWCoinTypeTerra:
    case TWCoinTypeBandChain:
    case TWCoinTypeTHORChain:
    case TWCoinTypeBluzelle:
    case TWCoinTypeIoTeX: {
        Cosmos::Address addr;
        if (!Cosmos::Address::decode(string, addr)) {
            break;
        }
        data = addr.getKeyHash();
        break;
    }

    case TWCoinTypeBitcoin:
    case TWCoinTypeDigiByte:
    case TWCoinTypeGroestlcoin:
    case TWCoinTypeLitecoin:
    case TWCoinTypeViacoin: {
        auto decoded = Bitcoin::SegwitAddress::decode(string);
        if (!std::get<2>(decoded)) {
            break;
        }
        data = std::get<0>(decoded).witnessProgram;
        break;
    }

    case TWCoinTypeBitcoinCash: {
        auto addr = Bitcoin::CashAddress(string);
        data.resize(Bitcoin::Address::size);
        size_t outlen = 0;
        cash_data_to_addr(data.data(), &outlen, addr.bytes.data(), 34);
        data = Data(data.begin() + 1, data.end());
        break;
    }

    case TWCoinTypeDash:
    case TWCoinTypeDogecoin:
    case TWCoinTypeMonacoin:
    case TWCoinTypeQtum:
    case TWCoinTypeRavencoin:
    case TWCoinTypeZcoin: {
        auto addr = Bitcoin::Address(string);
        data = Data(addr.bytes.begin() + 1, addr.bytes.end());
        break;
    }

    case TWCoinTypeDecred: {
        auto addr = Decred::Address(string);
        data = Data(addr.bytes.begin() + 2, addr.bytes.end());
        break;
    }

    case TWCoinTypeZcash:
    case TWCoinTypeZelcash: {
        auto addr = Zcash::TAddress(string);
        data = Data(addr.bytes.begin() + 2, addr.bytes.end());
        break;
    }

    case TWCoinTypeCallisto:
    case TWCoinTypeEthereum:
    case TWCoinTypeEthereumClassic:
    case TWCoinTypeGoChain:
    case TWCoinTypePOANetwork:
    case TWCoinTypeThunderToken:
    case TWCoinTypeTomoChain:
    case TWCoinTypeVeChain:
    case TWCoinTypeTheta:
    case TWCoinTypeWanchain:
    case TWCoinTypeAion:
    case TWCoinTypeSmartChainLegacy:
    case TWCoinTypeSmartChain:
    case TWCoinTypePolygon:
        data = parse_hex(string);
        break;

    case TWCoinTypeNano: {
        auto addr = Nano::Address(string);
        data = Data(addr.bytes.begin(), addr.bytes.end());
        break;
    }

    case TWCoinTypeZilliqa: {
        Zilliqa::Address addr;
        if (!Zilliqa::Address::decode(string, addr)) {
            break;
        }
        // data in Zilliqa is a checksummed string without 0x
        auto str = Zilliqa::checksum(addr.getKeyHash());
        data = Data(str.begin(), str.end());
        break;
    }

    case TWCoinTypeKusama: {
        auto addr = Kusama::Address(string);
        data = Data(addr.bytes.begin() + 1, addr.bytes.end());
        break;
    }

    case TWCoinTypePolkadot: {
        auto addr = Polkadot::Address(string);
        data = Data(addr.bytes.begin() + 1, addr.bytes.end());
        break;
    }

    case TWCoinTypeCardano: {
        auto addr = Cardano::AddressV3(string);
        data = addr.data();
        break;
    }

    case TWCoinTypeNEO: {
        auto addr = NEO::Address(string);
        data = Data(addr.bytes.begin(), addr.bytes.end());
        break;
    }

    case TWCoinTypeElrond: {
        Elrond::Address addr;
        if (Elrond::Address::decode(string, addr)) {
            data = addr.getKeyHash();
        }
        
        break;
    }

    case TWCoinTypeNEAR: {
        auto addr = NEAR::Address(string);
        data = Data(addr.bytes.begin(), addr.bytes.end());
        break;
    }

    case TWCoinTypeAvalancheXChain: {
        auto addr = Avalanche::Address(string);
        data = addr.getKeyHash();
        break;
    }

    default: break;
    }
    return TWDataCreateWithBytes(data.data(), data.size());
}
