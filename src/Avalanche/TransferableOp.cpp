// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "TransferableOp.h"

#include "../BinaryCoding.h"

using namespace TW::Avalanche;

bool TransferableOp::sortUTXOIDs(TransferableOp::UTXOID lhs, TransferableOp::UTXOID rhs) {
    if (lhs.first == rhs.first) {
        return lhs.second < rhs.second;
    }
    return std::lexicographical_compare(lhs.first.begin(), lhs.first.end(), rhs.first.begin(), rhs.first.end());
}

void TransferableOp::encode(Data& data) const {
    for (auto byte : AssetID) {
        data.push_back(byte);
    }
    encode32BE(static_cast<uint32_t>(UTXOIDs.size()), data);
    for (auto utxoID : UTXOIDs) {
        for (auto byte : utxoID.first) {
            data.push_back(byte);
        }
        encode32BE(utxoID.second, data);
    }
    TransferOp->encode(data);
}

bool TransferableOp::operator<(const TransferableOp& other) const {
    Data thisData;
    Data otherData;
    encode(thisData);
    other.encode(otherData);
    return std::lexicographical_compare(thisData.begin(), thisData.end(), otherData.begin(), otherData.end());
}

TransferableOp& TransferableOp::operator=(const TransferableOp &other) {
    // check for "self assignment" and do nothing in that case
    if (this == &other) {
        return *this;
    } else {
        // assign members
        UTXOIDs = other.UTXOIDs;
        std::sort(UTXOIDs.begin(), UTXOIDs.end(), sortUTXOIDs);
        AssetID = other.AssetID;
        TransferOp = other.TransferOp->duplicate();
        return *this;
    }
}

void SECP256k1MintOperation::encode(Data& data) const {
    encode32BE(typeID, data);
    encode32BE(static_cast<uint32_t>(AddressIndices.size()), data);
    for (auto index : AddressIndices) {
        encode32BE(index, data);
    }
    MintOutput.encode(data);
    TransferOutput.encode(data);
}

void NFTMintOperation::encode(Data& data) const {
    encode32BE(typeID, data);
    encode32BE(static_cast<uint32_t>(AddressIndices.size()), data);
    for (auto index : AddressIndices) {
        encode32BE(index, data);
    }
    encode32BE(GroupID, data);
    encode32BE(static_cast<uint32_t>(Payload.size()), data);
    for (auto byte : Payload) {
        data.push_back(byte);
    }
    EncodeOutputs(Outputs, data);
}


void NFTTransferOperation::encode(Data& data) const {
    encode32BE(typeID, data);
    encode32BE(static_cast<uint32_t>(AddressIndices.size()), data);
    for (auto index : AddressIndices) {
        encode32BE(index, data);
    }
    encode32BE(TransferOutput.GroupID, data);
    encode32BE(static_cast<uint32_t>(TransferOutput.Payload.size()), data);
    for (auto byte : TransferOutput.Payload) {
        data.push_back(byte);
    }
    encode64BE(TransferOutput.Locktime, data);
    encode32BE(TransferOutput.Threshold, data);
    encode32BE(static_cast<uint32_t>(TransferOutput.Addresses.size()), data);
    for (auto Address : TransferOutput.Addresses) {
        for (auto byte : Address.getKeyHash()) {
            data.push_back(byte);
        }
    }
}
