// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Bech32Address.h"
#include "Bech32.h"
#include "Data.h"
#include <TrezorCrypto/ecdsa.h>

using namespace TW;

bool Bech32Address::isValid(const std::string& addr) {
    return isValid(addr, "");
}

bool Bech32Address::isValid(const std::string& addr, const std::string& hrp) {
    auto dec = Bech32::decode(addr);
    // check hrp prefix (if given)
    if (hrp.length() > 0 && std::get<0>(dec).compare(0, hrp.length(), hrp) != 0) {
        return false;
    }
    if (std::get<1>(dec).empty()) {
        return false;
    }

    Data conv;
    auto success = Bech32::convertBits<5, 8>(conv, std::get<1>(dec), false);
    if (!success || conv.size() < 2 || conv.size() > 40) {
        return false;
    }

    return true;
}

bool Bech32Address::decode(const std::string& addr, Bech32Address& obj_out, const std::string& hrp, bool pad) {
    auto dec = Bech32::decode(addr);
    // check hrp prefix (if given)
    if (hrp.length() > 0 && std::get<0>(dec).compare(0, hrp.length(), hrp) != 0) {
        return false;
    }
    if (std::get<1>(dec).empty()) {
        return false;
    }

    Data conv;
    bool success = success = Bech32::convertBits<5, 8>(conv, std::get<1>(dec), pad);
    if (!success || conv.size() < 2 || conv.size() > 40) {
        return false;
    }

    obj_out.setHrp(std::get<0>(dec));
    obj_out.setKey(conv);
    return true;
}

Bech32Address::Bech32Address(const std::string& hrp, HasherType hasher, const PublicKey& publicKey)
: hrp(hrp) {
    switch (hasher) {
        case HASHER_SHA2_RIPEMD:
            {
                auto key = Data(20);
                ecdsa_get_pubkeyhash(publicKey.compressed().bytes.data(), HASHER_SHA2_RIPEMD, key.data());
                setKey(key);
            }
            break;
        
        case HASHER_SHA2:
            {
                const auto hash = Hash::sha256(publicKey.bytes);
                auto key = Data(20);
                std::copy(hash.end() - 20, hash.end(), key.begin());
                setKey(key);
            }
            break;

        case HASHER_SHA3K:
            {
                const auto hash = publicKey.hash({}, static_cast<Hash::HasherSimpleType>(Hash::keccak256), true);
                auto key = Data(20);
                std::copy(hash.end() - 20, hash.end(), key.begin());
                setKey(key);
            }
            break;

        default:
            throw std::invalid_argument("Invalid HasherType in Bech32Address");
    }
}

std::string Bech32Address::string() const {
    Data enc;
    if (!Bech32::convertBits<8, 5>(enc, keyHash, true)) {
        return "";
    }
    std::string result = Bech32::encode(hrp, enc, Bech32::ChecksumVariant::Bech32);
    // check back
    Bech32Address obj;
    if (!decode(result, obj, hrp)) {
        return "";
    }
    return result;
}
