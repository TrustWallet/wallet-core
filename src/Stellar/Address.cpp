// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Address.h"
#include "../Base32.h"
#include "../HexCoding.h"
#include "Crc.h"

#include <TrezorCrypto/memzero.h>
#include <TrustWalletCore/TWStellarVersionByte.h>

#include <array>
#include <cassert>

using namespace TW::Stellar;

bool Address::isValid(const std::string& string) {
    bool valid = false;

    if (string.length() != size) {
        return false;
    }

    // Check that it decodes correctly
    Data decoded;
    valid = TW::Base32::decode(string, decoded);

    // ... and that version byte is 0x30
    if (valid && TWStellarVersionByte(decoded[0]) != TWStellarVersionByte::TWStellarVersionByteAccountID) {
        valid = false;
    }

    // ... and that checksums match
    uint16_t checksum_expected = Crc::crc16(decoded.data(), 33);
    uint16_t checksum_actual = static_cast<uint16_t>((decoded[34] << 8) | decoded[33]); // unsigned short (little endian)
    if (valid && checksum_expected != checksum_actual) {
        valid = false;
    }

    memzero(decoded.data(), decoded.size());
    return valid;
}

Address::Address(const std::string& string) {
    // Ensure address is valid
    if (!isValid(string)) {
        throw std::invalid_argument("Invalid address data");
    }

    Data decoded;
    TW::Base32::decode(string, decoded);
    std::copy(decoded.begin() + 1, decoded.begin() + 1 + bytes.size(), bytes.begin());
    memzero(decoded.data(), decoded.size());
}

Address::Address(const PublicKey& publicKey) {
    if (publicKey.type != TWPublicKeyTypeED25519) {
        throw std::invalid_argument("Invalid public key type");
    }
    static_assert(PublicKey::ed25519Size == keySize);
    std::copy(publicKey.bytes.begin(), publicKey.bytes.end(), bytes.data());
}

std::string Address::string() const {
    // version + key bytes + checksum
    constexpr uint8_t keylen = 1 + 32 + 2;
    std::array<uint8_t, keylen> bytes_full;
    bytes_full[0] = 6 << 3; // 'G'

    std::copy(bytes.begin(), bytes.end(), bytes_full.begin() + 1);

    // Last two bytes are the checksum
    uint16_t checksum = Crc::crc16(bytes_full.data(), 33);
    bytes_full[keylen - 2] = checksum & 0x00ff;
    bytes_full[keylen - 1] = (checksum >> 8) & 0x00ff;

    Data bytesAsData;
    bytesAsData.assign(bytes_full.begin(), bytes_full.end());
    auto out = TW::Base32::encode(bytesAsData);
    return out;
}
