// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "TransferableOutput.h"

#include "../BinaryCoding.h"

using namespace TW::Avalanche;

bool TransactionOutput::operator<(const TransactionOutput& other) const {
    Data thisData;
    Data otherData;
    encode(thisData);
    other.encode(otherData);
    return std::lexicographical_compare(thisData.begin(), thisData.end(), otherData.begin(), otherData.end());
}

void TransferableOutput::encode(Data& data) const {
    for (auto byte : AssetID) {
        data.push_back(byte);
    }
    Output->encode(data);
}

bool TransferableOutput::operator<(const TransferableOutput& other) const {
    Data thisData;
    Data otherData;
    encode(thisData);
    other.encode(otherData);
    return std::lexicographical_compare(thisData.begin(), thisData.end(), otherData.begin(), otherData.end());
}


TransferableOutput& TransferableOutput::operator=(const TransferableOutput &other) {
    // check for "self assignment" and do nothing in that case
    if (this == &other) {
        return *this;
    } else {
        // assign members
        AssetID = other.AssetID;
        Output = other.Output->duplicate();
        return *this;
    }
}
      

void SECP256k1TransferOutput::encode(Data& data) const {
    encode32BE(typeID, data);
    encode64BE(Amount, data);
    encode64BE(Locktime, data);
    encode32BE(Threshold, data);
    encode32BE(static_cast<uint32_t>(Addresses.size()), data);
    for (auto Address : Addresses) {
        for (auto byte : Address.getKeyHash()) {
            data.push_back(byte);
        }
    }
}

void SECP256k1MintOutput::encode(Data& data) const {
    encode32BE(typeID, data);
    encode64BE(Locktime, data);
    encode32BE(Threshold, data);
    encode32BE(static_cast<uint32_t>(Addresses.size()), data);
    for (auto Address : Addresses) {
        for (auto byte : Address.getKeyHash()) {
            data.push_back(byte);
        }
    }
}

void NFTTransferOutput::encode(Data& data) const {
    encode32BE(typeID, data);
    encode32BE(GroupID, data);
    encode32BE(static_cast<uint32_t>(Payload.size()), data);
    for (auto byte : Payload) {
        data.push_back(byte);
    }
    encode64BE(Locktime, data);
    encode32BE(Threshold, data);
    encode32BE(static_cast<uint32_t>(Addresses.size()), data);
    for (auto Address : Addresses) {
        for (auto byte : Address.getKeyHash()) {
            data.push_back(byte);
        }
    }
}

void NFTMintOutput::encode(Data& data) const {
    encode32BE(typeID, data);
    encode32BE(GroupID, data);
    encode64BE(Locktime, data);
    encode32BE(Threshold, data);
    encode32BE(static_cast<uint32_t>(Addresses.size()), data);
    for (auto Address : Addresses) {
        for (auto byte : Address.getKeyHash()) {
            data.push_back(byte);
        }
    }
}
