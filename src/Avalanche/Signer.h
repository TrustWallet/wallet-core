// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../Data.h"
#include "../PrivateKey.h"
#include "../proto/Avalanche.pb.h"
#include "Transaction.h"

namespace TW::Avalanche {

/// Helper class that performs Avalanche transaction signing.
class Signer {
public:
    /// Hide default constructor
    Signer() = delete;

    /// Signs a Proto::SigningInput transaction
    static Proto::SigningOutput sign(const Proto::SigningInput& input) noexcept;
    /// Signs the given transaction.
    static Data sign(const std::vector<PrivateKey>& privateKeys, BaseTransaction& transaction) noexcept;
};

} // namespace TW::Avalanche
