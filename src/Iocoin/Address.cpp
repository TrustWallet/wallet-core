// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Address.h"

#include "../Base58.h"
#include <TrezorCrypto/ecdsa.h>

#include <cassert>

using namespace TW::Iocoin;

bool Address::isValid(const std::string& string) {
    const auto decoded = Base58::bitcoin.decodeCheck(string, Hash::sha256d);
    if (decoded.size() != Address::size) {
        return false;
    }
    return true;
}

bool Address::isValid(const std::string& string, const std::vector<byte>& validPrefixes) {
    const auto decoded = Base58::bitcoin.decodeCheck(string, Hash::sha256d);
    if (decoded.size() != Address::size) {
        return false;
    }
    if (std::find(validPrefixes.begin(), validPrefixes.end(), decoded[0]) == validPrefixes.end()) {
        return false;
    }
    return true;
}

Address::Address(const PublicKey& publicKey, uint8_t prefix) {
    if (publicKey.type() != PublicKeyType::secp256k1) {
        throw std::invalid_argument("Iocoin::Address needs a compressed SECP256k1 public key.");
    }
    bytes[0] = prefix;
    ecdsa_get_pubkeyhash(publicKey.bytes.data(), HASHER_SHA2_RIPEMD, bytes.data() + 1);
}

std::string Address::string() const {
    return Base58::bitcoin.encodeCheck(bytes, Hash::sha256d);
}
