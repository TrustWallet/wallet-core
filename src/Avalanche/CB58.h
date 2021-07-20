// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Data.h"
#include "Hash.h"

#include <array>
#include <string>

// The implementation of only hashing once instead of twice and using the last
// 4 bytes instead of the first four bytes as a checksum was sufficently different 
// from the Base58 class. This resulted in us not wanting to expand the Base58 
// class, instead choosing to create a new class.

namespace TW::Avalanche {

class CB58 {
  public:
    /// Base58 coder with Avalanche character map.
    static CB58 avalanche;

  public:
    /// Ordered list of valid characters.
    const std::array<char, 58> digits;

    /// Maps characters to base58 values.
    const std::array<signed char, 128> characterMap;

    /// Initializes a Base58 class with custom digit mapping.
    CB58(const std::array<char, 58>& digits, const std::array<signed char, 128>& characterMap)
        : digits(digits), characterMap(characterMap) {}

    /// Decodes a base 58 string verifying the checksum, returns empty on failure.
    Data decodeCheck(const std::string& string, Hash::Hasher hasher = Hash::sha256s) const {
        return decodeCheck(string.data(), string.data() + string.size(), hasher);
    }

    /// Decodes a base 58 string verifying the checksum, returns empty on failure.
    Data decodeCheck(const char* begin, const char* end, Hash::Hasher hasher = Hash::sha256s) const;

    /// Decodes a base 58 string into `result`, returns `false` on failure.
    Data decode(const std::string& string) const {
        return decode(string.data(), string.data() + string.size());
    }

    /// Decodes a base 58 string into `result`, returns `false` on failure.
    Data decode(const char* begin, const char* end) const;

    /// Encodes data as a base 58 string with a checksum.
    template <typename T>
    std::string encodeCheck(const T& data, Hash::Hasher hasher = Hash::sha256s) const {
        return encodeCheck(data.data(), data.data() + data.size(), hasher);
    }

    /// Encodes data as a base 58 string with a checksum.
    std::string encodeCheck(const byte* pbegin, const byte* pend, Hash::Hasher hasher = Hash::sha256s) const;

    /// Encodes data as a base 58 string.
    template <typename T>
    std::string encode(const T& data) const {
        return encode(data.data(), data.data() + data.size());
    }

    /// Encodes data as a base 58 string.
    std::string encode(const byte* pbegin, const byte* pend) const;
};

} // namespace TW
