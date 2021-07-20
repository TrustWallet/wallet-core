// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "AddressV3.h"
#include "AddressV2.h"
#include <TrustWalletCore/TWCoinType.h>
#include "../Data.h"
#include "../Bech32.h"
#include "../Base32.h"
#include "../Crc.h"
#include "../HexCoding.h"
#include "../Hash.h"

#include <array>

using namespace TW;
using namespace TW::Cardano;
using namespace std;

bool AddressV3::parseAndCheckV3(const std::string& addr, Discrimination& discrimination, Kind& kind, Data& key1, Data& key2) {
    try {
        auto bech = Bech32::decode(addr);
        if (std::get<1>(bech).size() == 0) {
            // empty Bech data
            return false;
        }
        // Bech bits conversion
        Data conv;
        auto success = Bech32::convertBits<5, 8>(conv, std::get<1>(bech), false);
        if (!success) {
            return false;
        }
        if (conv.size() != 33 && conv.size() != 65) {
            return false;
        }
        discrimination = (Discrimination)((conv[0] & 0b10000000) >> 7);
        kind = (Kind)(conv[0] & 0b01111111);
        if (kind <= Kind_Sentinel_Low || kind >= Kind_Sentinel_High) {
            return false;
        }
        if ((kind == Kind_Group && conv.size() != 65) ||
            (kind != Kind_Group && conv.size() != 33)) {
            return false;
        }

        switch (kind) {
            case Kind_Single:
            case Kind_Account:
            case Kind_Multisig:
                assert(conv.size() == 33);
                key1 = Data(32);
                std::copy(conv.begin() + 1, conv.begin() + 33, key1.begin());
                return true;

            case Kind_Group:
                assert(conv.size() == 65);
                key1 = Data(32);
                key2 = Data(32);
                std::copy(conv.begin() + 1, conv.begin() + 33, key1.begin());
                std::copy(conv.begin() + 33, conv.begin() + 65, key2.begin());
                return true;

            default:
                return false;
        }
    } catch (...) {
        return false;
    }
}

bool AddressV3::isValid(const std::string& addr) {
    Discrimination discrimination;
    Kind kind;
    Data key1;
    Data key2;
    if (parseAndCheckV3(addr, discrimination, kind, key1, key2)) {
        return true;
    }
    // not V3, try older
    return AddressV2::isValid(addr);
}

AddressV3 AddressV3::createSingle(Discrimination discrimination_in, const Data& spendingKey) {
    if (spendingKey.size() != 32) {
        throw std::invalid_argument("Wrong spending key size");
    }
    auto addr = AddressV3();
    addr.discrimination = discrimination_in;
    addr.kind = Kind_Single;
    addr.key1 = spendingKey;
    return addr;
}

AddressV3 AddressV3::createGroup(Discrimination discrimination_in, const Data& spendingKey, const Data& groupKey) {
    if (spendingKey.size() != 32) {
        throw std::invalid_argument("Wrong spending key size");
    }
    if (groupKey.size() != 32) {
        throw std::invalid_argument("Wrong group key size");
    }
    auto addr = AddressV3();
    addr.discrimination = discrimination_in;
    addr.kind = Kind_Group;
    addr.key1 = spendingKey;
    addr.groupKey = groupKey;
    return addr;
}

AddressV3 AddressV3::createAccount(Discrimination discrimination_in, const Data& accountKey) {
    if (accountKey.size() != 32) {
        throw std::invalid_argument("Wrong spending key size");
    }
    auto addr = AddressV3();
    addr.discrimination = discrimination_in;
    addr.kind = Kind_Account;
    addr.key1 = accountKey;
    return addr;
}

AddressV3::AddressV3(const std::string& addr) {
    if (parseAndCheckV3(addr, discrimination, kind, key1, groupKey)) {
        // values stored
        return;
    }
    // try legacy
    // throw on error
    legacyAddressV2 = AddressV2(addr);
}

AddressV3::AddressV3(const PublicKey& publicKey) {
    // input is extended pubkey, 64-byte
    if (publicKey.type != TWPublicKeyTypeED25519Extended) {
        throw std::invalid_argument("Invalid public key type");
    }
    discrimination = Discrim_Test;
    kind = Kind_Group;
    key1 = Data(32);
    groupKey = Data(32);
    std::copy(publicKey.bytes.begin(), publicKey.bytes.begin() + 32, key1.begin());
    std::copy(publicKey.bytes.begin() + 32, publicKey.bytes.begin() + 64, groupKey.begin());
}

AddressV3::AddressV3(const Data& data) {
    // min 4 bytes, 2 prefix + 2 len
    if (data.size() < 4) { throw std::invalid_argument("Address data too short"); }
    assert(data.size() >= 4);
    int index = 0;
    discrimination = (Discrimination)data[index++];
    kind = (Kind)data[index++];
    // key1:
    byte len1 = data[index++];
    if (data.size() < 4 + len1) { throw std::invalid_argument("Address data too short"); }
    assert(data.size() >= 4 + len1);
    key1 = Data(len1);
    std::copy(data.begin() + index, data.begin() + index + len1, key1.begin());
    index += len1;
    // groupKey:
    byte len2 = data[index++];
    if (data.size() < 4 + len1 + len2) { throw std::invalid_argument("Address data too short"); }
    assert(data.size() >= 4 + len1 + len2);
    groupKey = Data(len2);
    std::copy(data.begin() + index, data.begin() + index + len2, groupKey.begin());
}

AddressV3::AddressV3(const AddressV3& other) :
    discrimination(other.discrimination),
    kind(other.kind),
    key1(other.key1),
    groupKey(other.groupKey),
    legacyAddressV2(other.legacyAddressV2)
{}

void AddressV3::operator=(const AddressV3& other)
{
    discrimination = other.discrimination;
    kind = other.kind;
    key1 = other.key1;
    groupKey = other.groupKey;
    legacyAddressV2 = other.legacyAddressV2;
}

string AddressV3::string() const {
    std::string hrp;
    switch (kind) {
        case Kind_Single:
        case Kind_Group:
        case Kind_Account:
            hrp = stringForHRP(TWHRPCardano); break;
        default:
            hrp = ""; break;
    }
    return string(hrp);
}

string AddressV3::string(const std::string& hrp) const {
    if (legacyAddressV2.has_value()) {
        return legacyAddressV2->string();
    }

    byte first = (byte)kind;
    if (discrimination == Discrim_Test) first = first | 0b10000000;
    Data keys;
    TW::append(keys, first);
    TW::append(keys, key1);
    if (groupKey.size() > 0) {
        TW::append(keys, groupKey);
    }
    // bech
    Data bech;
    if (!Bech32::convertBits<8, 5>(bech, keys, true)) {
        return "";
    }
    return Bech32::encode(hrp, bech, Bech32::ChecksumVariant::Bech32);
}

string AddressV3::stringBase32() const {
    if (legacyAddressV2.has_value()) {
        return legacyAddressV2->string();
    }

    byte first = (byte)kind;
    if (discrimination == Discrim_Test) first = first | 0b10000000;
    Data keys;
    TW::append(keys, first);
    TW::append(keys, key1);
    if (groupKey.size() > 0) {
        TW::append(keys, groupKey);
    }
    std::string base32 = Base32::encode(keys, "abcdefghijklmnopqrstuvwxyz23456789");
    return base32;
}

Data AddressV3::data() const {
    Data data;
    TW::append(data, (uint8_t)discrimination);
    TW::append(data, (uint8_t)kind);
    TW::append(data, (uint8_t)key1.size());
    TW::append(data, key1);
    TW::append(data, (uint8_t)groupKey.size());
    TW::append(data, groupKey);
    return data;
}
