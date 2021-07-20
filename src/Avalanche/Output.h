// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Address.h"

namespace TW::Avalanche {    
    using Locktime = uint64_t;
    using Threshold = uint32_t;
    using Output = std::tuple<Locktime, Threshold, std::vector<Address>>;

    void SortOutputs(std::vector<Output> &outputs);
    void EncodeOutputs(std::vector<Output> outputs, Data &data);
}

