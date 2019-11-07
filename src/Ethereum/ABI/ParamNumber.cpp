// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "ParamNumber.h"

#include "../../Data.h"
#include "../../uint256.h"

#include <string>
#include <cassert>

using namespace TW;
using namespace TW::Ethereum::ABI;

void ParamUIntN::setVal(uint256_t val) {
    // mask it to the given bits
    _val = val & _mask;
}

bool ParamUIntN::decode(const Data& encoded, size_t& offset_inout) {
    uint256_t temp;
    auto res = decodeNumber(encoded, temp, offset_inout);
    setVal(temp);
    return res;
}

void ParamUIntN::init() {
    assert(bits >= 8 && bits <= 256 && (bits % 8) == 0);
    _mask = (uint256_t(1) << bits) - 1;
}

int256_t ParamIntN::fromUInt256(uint256_t x)
{
    static const uint256_t maxNeg = uint256_t(1) << 255;
    if (x > maxNeg) {
        return -((int256_t)~x) - 1;
    } else {
        return (int256_t)x;
    }
}

void ParamIntN::setVal(int256_t val) {
    // mask it to the given bits
    if (val < 0) {
        _val = fromUInt256(~((~((uint256_t)val)) & _mask));
    } else {
        _val = fromUInt256(((uint256_t)val) & _mask);
    }
}

bool ParamIntN::decodeNumber(const Data& encoded, int256_t& decoded, size_t& offset_inout) {
    uint256_t valU;
    auto res = TW::Ethereum::ABI::decode(encoded, valU, offset_inout);
    decoded = fromUInt256(valU);
    return res;
}

bool ParamIntN::decode(const Data& encoded, size_t& offset_inout) {
    int256_t temp;
    auto res = decodeNumber(encoded, temp, offset_inout);
    setVal(temp);
    return res;
}

void ParamIntN::init()
{
    assert(bits >= 8 && bits <= 256 && (bits % 8) == 0);
    _mask = (uint256_t(1) << bits) - 1;
}
