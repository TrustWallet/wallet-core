// Copyright © 2014-2018 The Bitcoin Core developers
// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "CB58.h"

#include "Hash.h"

#include <algorithm>
#include <cctype>
#include <cassert>

using namespace TW;
using namespace TW::Avalanche;

// clang-format off

static const std::array<char, 58> avalancheDigits = {
    '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
    'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
    'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
};

static const std::array<signed char, 128> avalancheCharacterMap = {
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1, 0, 1, 2, 3, 4, 5, 6, 7, 8,-1,-1,-1,-1,-1,-1,
	-1, 9,10,11,12,13,14,15,16,-1,17,18,19,20,21,-1,
	22,23,24,25,26,27,28,29,30,31,32,-1,-1,-1,-1,-1,
	-1,33,34,35,36,37,38,39,40,41,42,43,-1,44,45,46,
	47,48,49,50,51,52,53,54,55,56,57,-1,-1,-1,-1,-1,
};

// clang-format on

CB58 CB58::avalanche = CB58(avalancheDigits, avalancheCharacterMap);

Data CB58::decodeCheck(const char* begin, const char* end, Hash::Hasher hasher) const {
    auto result = decode(begin, end);
    if (result.size() < 4) {
        return {};
    }

    // re-calculate the checksum, ensure it matches the included 4-byte checksum
    auto hash = hasher(result.data(), result.size() - 4);
    if (!std::equal(hash.end() - 4, hash.end(), result.end() - 4)) {
        return {};
    }

    return Data(result.begin(), result.end() - 4);
}

Data CB58::decode(const char* begin, const char* end) const {
    auto it = begin;

    // Skip leading spaces.
    it = std::find_if_not(it, end, std::isspace);

    // Skip and count leading zeros.
    std::size_t zeroes = 0;
    std::size_t length = 0;
    while (it != end && *it == digits[0]) {
        zeroes += 1;
        it += 1;
    }

    // Allocate enough space in big-endian base256 representation.
    std::size_t base258Size = (end - it) * 733 / 1000 + 1; // log(58) / log(256), rounded up.
    Data b256(base258Size);

    // Process the characters.
    while (it != end && !std::isspace(*it)) {
        if (static_cast<unsigned char>(*it) >= 128) {
            // Invalid b58 character
            return {};
        }

        // Decode base58 character
        int carry = characterMap[static_cast<unsigned char>(*it)];
        if (carry == -1) {
            // Invalid b58 character
            return {};
        }

        std::size_t i = 0;
        for (auto b256it = b256.rbegin(); (carry != 0 || i < length) && (b256it != b256.rend());
             ++b256it, ++i) {
            carry += 58 * (*b256it);
            *b256it = static_cast<uint8_t>(carry % 256);
            carry /= 256;
        }
        assert(carry == 0);
        length = i;
        it += 1;
    }

    // Skip trailing spaces.
    it = std::find_if_not(it, end, std::isspace);
    if (it != end) {
        // Extra charaters at the end
        return {};
    }

    // Skip leading zeroes in b256.
    auto b256it = b256.begin() + (base258Size - length);
    while (b256it != b256.end() && *b256it == 0) {
        b256it++;
    }

    // Copy result into output vector.
    Data result;
    result.reserve(zeroes + (b256.end() - b256it));
    result.assign(zeroes, 0x00);
    std::copy(b256it, b256.end(), std::back_inserter(result));

    return result;
}

std::string CB58::encodeCheck(const byte* begin, const byte* end, Hash::Hasher hasher) const {
    // add 4-byte hash check to the end
    Data dataWithCheck(begin, end);
    auto hash = hasher(begin, end - begin);
    dataWithCheck.insert(dataWithCheck.end(), hash.end() - 4, hash.end());
    return encode(dataWithCheck);
}

std::string CB58::encode(const byte* begin, const byte* end) const {
    // Skip & count leading zeroes.
    int zeroes = 0;
    int length = 0;
    while (begin != end && *begin == 0) {
        begin += 1;
        zeroes += 1;
    }

    // Allocate enough space in big-endian base58 representation.
    auto base58Size = (end - begin) * 138 / 100 + 1; // log(256) / log(58), rounded up.
    Data b58(base58Size);

    while (begin != end) {
        int carry = *begin;
        int i = 0;
        // Apply "b58 = b58 * 256 + ch".
        for (auto b58it = b58.rbegin(); (carry != 0 || i < length) && (b58it != b58.rend());
             b58it++, i++) {
            carry += 256 * (*b58it);
            *b58it = carry % 58;
            carry /= 58;
        }

        assert(carry == 0);
        length = i;
        begin += 1;
    }

    // Skip leading zeroes in base58 result.
    auto it = b58.begin() + (base58Size - length);
    while (it != b58.end() && *it == 0) {
        it++;
    }

    // Translate the result into a string.
    std::string str;
    str.reserve(zeroes + (b58.end() - it));
    str.assign(zeroes, digits[0]);
    while (it != b58.end()) {
        str += digits[*it];
        it += 1;
    }
    return str;
}
