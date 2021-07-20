// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "../BinaryCoding.h"
#include "Output.h"

using namespace TW::Avalanche;

bool compareOutputForSort(Output lhs, Output rhs) {
    if (std::get<0>(lhs) != std::get<0>(rhs)) {
        return std::get<0>(lhs) < std::get<0>(rhs);
    }
    if (std::get<1>(lhs) != std::get<1>(rhs)) {
        return std::get<1>(lhs) < std::get<1>(rhs);
    }
    return std::lexicographical_compare(std::get<2>(lhs).begin(), std::get<2>(lhs).end(), std::get<2>(rhs).begin(), std::get<2>(rhs).end());
}

void TW::Avalanche::SortOutputs(std::vector<Output> &outputs){
    // sort the addresses in the outputs, then sort the outputs themselves
    for (auto &output : outputs) {
        std::sort(std::get<2>(output).begin(), std::get<2>(output).begin());
    }
    std::sort(outputs.begin(), outputs.end(), compareOutputForSort);
}

// must first call SortOutputs
void TW::Avalanche::EncodeOutputs(std::vector<Output> outputs, Data &data) {
    TW::encode32BE(static_cast<uint32_t>(outputs.size()), data);
    for (auto output : outputs) {
        TW::encode64BE(std::get<0>(output), data);
        TW::encode32BE(std::get<1>(output), data);
        std::vector<Address> addrs = std::get<2>(output);
        TW::encode32BE(static_cast<uint32_t>(addrs.size()), data);
        for (auto addr : addrs) {
            for (auto byte : addr.getKeyHash()) {
                data.push_back(byte);
            }
        }
    }
}
