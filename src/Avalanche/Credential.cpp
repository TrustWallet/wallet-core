// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Credential.h"

#include "../BinaryCoding.h"

using namespace TW::Avalanche;

void Credential::encode(Data& data) const {
    encode32BE(TypeID, data);
    // do not sort signatures, they must match input signature indices
    encode32BE(static_cast<uint32_t>(Signatures.size()), data);
    for (auto sig : Signatures) {
        for (auto byte : sig) {
            data.push_back(byte);
        }
    }
}

bool Credential::operator<(const Credential& other) const {
    Data thisData;
    Data otherData;
    encode(thisData);
    other.encode(otherData);
    return std::lexicographical_compare(thisData.begin(), thisData.end(), otherData.begin(), otherData.end());
}
