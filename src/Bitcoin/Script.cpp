// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Script.h"

#include "Address.h"
#include "CashAddress.h"
#include "SegwitAddress.h"

#include "../Base58.h"
#include "../Coin.h"

#include "../BinaryCoding.h"
#include "../Data.h"
#include "../Decred/Address.h"
#include "../Groestlcoin/Address.h"
#include "../Hash.h"
#include "../PublicKey.h"
#include "../Zcash/TAddress.h"

#include "OpCodes.h"

#include <algorithm>
#include <cassert>
#include <set>

using namespace TW;
using namespace TW::Bitcoin;

Data Script::hash() const {
    return Hash::ripemd(Hash::sha256(bytes));
}

bool Script::isPayToScriptHash() const {
    // Extra-fast test for pay-to-script-hash
    return bytes.size() == 23 && bytes[0] == OP_HASH160 && bytes[1] == 0x14 &&
           bytes[22] == OP_EQUAL;
}

bool Script::isPayToWitnessScriptHash() const {
    // Extra-fast test for pay-to-witness-script-hash
    return bytes.size() == 34 && bytes[0] == OP_0 && bytes[1] == 0x20;
}

bool Script::isPayToWitnessPublicKeyHash() const {
    // Extra-fast test for pay-to-witness-public-key-hash
    return bytes.size() == 22 && bytes[0] == OP_0 && bytes[1] == 0x14;
}

bool Script::isWitnessProgram() const {
    if (bytes.size() < 4 || bytes.size() > 42) {
        return false;
    }
    if (bytes[0] != OP_0 && (bytes[0] < OP_1 || bytes[0] > OP_16)) {
        return false;
    }
    return bytes[1] + 2 == bytes.size();
}

bool Script::matchPayToPublicKey(Data& result) const {
    if (bytes.size() == PublicKey::secp256k1ExtendedSize + 2 &&
        bytes[0] == PublicKey::secp256k1ExtendedSize && bytes.back() == OP_CHECKSIG) {
        result.clear();
        std::copy(std::begin(bytes) + 1, std::begin(bytes) + 1 + PublicKey::secp256k1Size,
                  std::back_inserter(result));
        return true;
    }
    if (bytes.size() == PublicKey::secp256k1Size + 2 && bytes[0] == PublicKey::secp256k1Size &&
        bytes.back() == OP_CHECKSIG) {
        result.clear();
        std::copy(std::begin(bytes) + 1, std::begin(bytes) + 1 + PublicKey::secp256k1Size,
                  std::back_inserter(result));
        return true;
    }
    return false;
}

bool Script::matchPayToPublicKeyHash(Data& result) const {
    if (bytes.size() == 25 && bytes[0] == OP_DUP && bytes[1] == OP_HASH160 && bytes[2] == 20 &&
        bytes[23] == OP_EQUALVERIFY && bytes[24] == OP_CHECKSIG) {
        result.clear();
        std::copy(std::begin(bytes) + 3, std::begin(bytes) + 3 + 20, std::back_inserter(result));
        return true;
    }
    return false;
}

bool Script::matchPayToScriptHash(Data& result) const {
    if (!isPayToScriptHash()) {
        return false;
    }
    result.clear();
    std::copy(std::begin(bytes) + 2, std::begin(bytes) + 22, std::back_inserter(result));
    return true;
}

bool Script::matchPayToWitnessPublicKeyHash(Data& result) const {
    if (!isPayToWitnessPublicKeyHash()) {
        return false;
    }
    result.clear();
    std::copy(std::begin(bytes) + 2, std::end(bytes), std::back_inserter(result));
    return true;
}

bool Script::matchPayToWitnessScriptHash(Data& result) const {
    if (!isPayToWitnessScriptHash()) {
        return false;
    }
    result.clear();
    std::copy(std::begin(bytes) + 2, std::end(bytes), std::back_inserter(result));
    return true;
}

bool Script::matchMultisig(std::vector<Data>& keys, int& required) const {
    keys.clear();
    required = 0;

    if (bytes.size() < 1 || bytes.back() != OP_CHECKMULTISIG) {
        return false;
    }

    size_t it = 0;
    uint8_t opcode;
    Data operand;

    auto op = getScriptOp(it, opcode, operand);
    if (!op || !TWOpCodeIsSmallInteger(opcode)) {
        return false;
    }
    required = decodeNumber(opcode);
    while (true) {
        auto res = getScriptOp(it, opcode, operand);
        if (!res) {
            break;
        }
        if (!PublicKey::isValid(operand, TWPublicKeyTypeSECP256k1)) {
            break;
        }
        keys.push_back(operand);
    }

    if (!TWOpCodeIsSmallInteger(opcode)) {
        return false;
    }

    auto expectedCount = decodeNumber(opcode);
    if (keys.size() != expectedCount || expectedCount < required) {
        return false;
    }
    if (it + 1 != bytes.size()) {
        return false;
    }

    return true;
}

bool Script::getScriptOp(size_t& index, uint8_t& opcode, Data& operand) const {
    operand.clear();

    // Read instruction
    if (index >= bytes.size()) {
        return false;
    }

    opcode = bytes[index];
    index += 1;

    if (opcode > OP_PUSHDATA4) {
        return true;
    }

    // Immediate operand
    size_t size = 0;
    if (opcode < OP_PUSHDATA1) {
        size = static_cast<size_t>(opcode);
    } else if (opcode == OP_PUSHDATA1) {
        if (bytes.size() - index < 1) {
            return false;
        }
        size = bytes[index];
        index += 1;
    } else if (opcode == OP_PUSHDATA2) {
        if (bytes.size() - index < 2) {
            return false;
        }
        size = static_cast<size_t>(decode16LE(bytes.data() + index));
        index += 2;
    } else if (opcode == OP_PUSHDATA4) {
        if (bytes.size() - index < 4) {
            return false;
        }
        size = static_cast<size_t>(decode32LE(bytes.data() + index));
        index += 4;
    }
    if (bytes.size() - index < size) {
        return false;
    }
    operand = Data(bytes.begin() + index, bytes.begin() + index + size);
    index += size;

    return true;
}

Script Script::buildPayToPublicKeyHash(const Data& hash) {
    assert(hash.size() == 20);
    Script script;
    script.bytes.push_back(OP_DUP);
    script.bytes.push_back(OP_HASH160);
    script.bytes.push_back(20);
    script.bytes.insert(script.bytes.end(), hash.begin(), hash.end());
    script.bytes.push_back(OP_EQUALVERIFY);
    script.bytes.push_back(OP_CHECKSIG);
    return script;
}

Script Script::buildPayToScriptHash(const Data& scriptHash) {
    assert(scriptHash.size() == 20);
    Script script;
    script.bytes.push_back(OP_HASH160);
    script.bytes.push_back(20);
    script.bytes.insert(script.bytes.end(), scriptHash.begin(), scriptHash.end());
    script.bytes.push_back(OP_EQUAL);
    return script;
}

Script Script::buildPayToWitnessProgram(const Data& program) {
    assert(program.size() == 20 || program.size() == 32);
    Script script;
    script.bytes.push_back(OP_0);
    script.bytes.push_back(static_cast<byte>(program.size()));
    script.bytes.insert(script.bytes.end(), program.begin(), program.end());
    assert(script.bytes.size() == 22 || script.bytes.size() == 34);
    return script;
}

Script Script::buildPayToWitnessPublicKeyHash(const Data& hash) {
    assert(hash.size() == 20);
    return Script::buildPayToWitnessProgram(hash);
}

Script Script::buildPayToWitnessScriptHash(const Data& scriptHash) {
    assert(scriptHash.size() == 32);
    return Script::buildPayToWitnessProgram(scriptHash);
}

Script Script::buildOpReturnScript(const Data& data) {
    const int MaxOpReturnLength = 64;
    Script script;
    script.bytes.push_back(OP_RETURN);
    size_t size = data.size();
    if (size > MaxOpReturnLength) {
        size = MaxOpReturnLength;
    }
    script.bytes.push_back(static_cast<byte>(size));
    script.bytes.insert(script.bytes.end(), data.begin(), data.begin() + size);
    return script;
}

void Script::encode(Data& data) const {
    encodeVarInt(bytes.size(), data);
    std::copy(std::begin(bytes), std::end(bytes), std::back_inserter(data));
}

Script Script::lockScriptForAddress(const std::string& string, enum TWCoinType coin) {
    if (Address::isValid(string)) {
        auto address = Address(string);
        auto p2pkh = TW::p2pkhPrefix(coin);
        auto p2sh = TW::p2shPrefix(coin);
        if (p2pkh == address.bytes[0]) {
            // address starts with 1/L
            auto data = Data();
            data.reserve(Address::size - 1);
            std::copy(address.bytes.begin() + 1, address.bytes.end(), std::back_inserter(data));
            return buildPayToPublicKeyHash(data);
        } else if (p2sh == address.bytes[0]) {
            // address starts with 3/M
            auto data = Data();
            data.reserve(Address::size - 1);
            std::copy(address.bytes.begin() + 1, address.bytes.end(), std::back_inserter(data));
            return buildPayToScriptHash(data);
        }
    } else if (SegwitAddress::isValid(string)) {
        auto result = SegwitAddress::decode(string);
        // address starts with bc/ltc
        auto program = std::get<0>(result).witnessProgram;
        return buildPayToWitnessProgram(program);
    } else if (CashAddress::isValid(string)) {
        auto address = CashAddress(string);
        auto bitcoinAddress = address.legacyAddress();
        return lockScriptForAddress(bitcoinAddress.string(), TWCoinTypeBitcoinCash);
    } else if (Decred::Address::isValid(string)) {
        auto bytes = Base58::bitcoin.decodeCheck(string, Hash::blake256d);
        if (bytes[1] == TW::p2pkhPrefix(TWCoinTypeDecred)) {
            return buildPayToPublicKeyHash(Data(bytes.begin() + 2, bytes.end()));
        }
        if (bytes[1] == TW::p2shPrefix(TWCoinTypeDecred)) {
            return buildPayToScriptHash(Data(bytes.begin() + 2, bytes.end()));
        }
    } else if (Groestlcoin::Address::isValid(string)) {
        auto address = Groestlcoin::Address(string);
        auto data = Data();
        data.reserve(Address::size - 1);
        std::copy(address.bytes.begin() + 1, address.bytes.end(), std::back_inserter(data));
        if (address.bytes[0] == TW::p2pkhPrefix(TWCoinTypeGroestlcoin)) {
            return buildPayToPublicKeyHash(data);
        }
        if (address.bytes[0] == TW::p2shPrefix(TWCoinTypeGroestlcoin)) {
            return buildPayToScriptHash(data);
        }
    } else if (Zcash::TAddress::isValid(string)) {
        auto address = Zcash::TAddress(string);
        auto data = Data();
        data.reserve(Address::size - 2);
        std::copy(address.bytes.begin() + 2, address.bytes.end(), std::back_inserter(data));
        if (address.bytes[1] == TW::p2pkhPrefix(TWCoinTypeZcash)) {
            return buildPayToPublicKeyHash(data);
        } else if (address.bytes[1] == TW::p2shPrefix(TWCoinTypeZcash)) {
            return buildPayToScriptHash(data);
        }
    }
    return {};
}
