// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "InitialState.h"

#include "../BinaryCoding.h"

using namespace TW::Avalanche;

void InitialState::encode(Data& data) const {
    encode32BE(FxID, data);
    encode32BE(static_cast<uint32_t>(Outputs.size()), data);
    for (auto &output : Outputs) {
        output->encode(data);
    }
}

bool InitialState::operator<(const InitialState& other) const {
    Data thisData;
    Data otherData;
    encode(thisData);
    other.encode(otherData);
    return std::lexicographical_compare(thisData.begin(), thisData.end(), otherData.begin(), otherData.end());
}

InitialState& InitialState::operator=(const InitialState &other) {
    // check for "self assignment" and do nothing in that case
    if (this == &other) {
        return *this;
    }
    // assign members
    FxID = other.FxID;
    Outputs.clear();
    for (auto &output : other.Outputs) {
        Outputs.push_back(output->duplicate());
    }
    std::sort(Outputs.begin(), Outputs.end(), [](const auto& lhs, const auto& rhs)
    {
        return *lhs < *rhs;
    });
    return *this;
}
