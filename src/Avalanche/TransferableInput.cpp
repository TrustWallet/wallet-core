// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "TransferableInput.h"

#include "../BinaryCoding.h"

using namespace TW::Avalanche;

void TransferableInput::encode(Data& data) const {
    for (auto byte : TxID) {
        data.push_back(byte);
    }
    encode32BE(UTXOIndex, data);
    for (auto byte : AssetID) {
        data.push_back(byte);
    }
    Input->encode(data);
}

bool TransferableInput::operator<(const TransferableInput& other) const {
    if (TxID == other.TxID) {
        return UTXOIndex < other.UTXOIndex;
    } 
    Data thisTxIDData;
    Data otherTxIDData;
    for (auto byte : TxID) {
        thisTxIDData.push_back(byte);
    }
    for (auto byte : other.TxID) {
        otherTxIDData.push_back(byte);
    }
    return std::lexicographical_compare(thisTxIDData.begin(), thisTxIDData.end(), otherTxIDData.begin(), otherTxIDData.end());
}

TransferableInput& TransferableInput::operator=(const TransferableInput &other) {
    // check for "self assignment" and do nothing in that case
    if (this == &other) {
        return *this;
    } else {
        // assign members
        TxID = other.TxID;
        UTXOIndex = other.UTXOIndex;
        AssetID = other.AssetID;
        Input = other.Input->duplicate();
        SpendableAddresses = other.SpendableAddresses;
        return *this;
    }
}

void SECP256k1TransferInput::encode(Data& data) const {
    encode32BE(TypeID, data);
    encode64BE(Amount, data);
    encode32BE(static_cast<uint32_t>(AddressIndices.size()), data);
    for (auto index: AddressIndices) {
        encode32BE(index, data);
    }
}
