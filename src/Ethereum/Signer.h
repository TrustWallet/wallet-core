// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "RLP.h"
#include "Transaction.h"
#include "../Data.h"
#include "../Hash.h"
#include "../PrivateKey.h"
#include "../proto/Ethereum.pb.h"
#include "../uint256.h"

#include <boost/multiprecision/cpp_int.hpp>
#include <cstdint>
#include <vector>
#include <memory>

namespace TW::Ethereum {

/// Helper class that performs Ethereum transaction signing.
class Signer {
  public:
    /// Signs a Proto::SigningInput transaction
    static Proto::SigningOutput sign(const Proto::SigningInput& input) noexcept;
    /// Signs a json Proto::SigningInput with private key
    static std::string signJSON(const std::string& json, const Data& key);

  public:
    Signer() = delete;

    /// Signs the given transaction.
    static Signature sign(const PrivateKey& privateKey, const uint256_t& chainID, std::shared_ptr<TransactionBase> transaction) noexcept;

  public:
    /// build Transaction from signing input
    static std::shared_ptr<TransactionBase> build(const Proto::SigningInput& input);

    /// Signs a hash with the given private key for the given chain identifier.
    ///
    /// @returns the r, s, and v values of the transaction signature
    static Signature sign(const PrivateKey& privateKey, const Data& hash, bool includeEip155, const uint256_t& chainID) noexcept;

    /// Break up the signature into the R, S, and V values.
    /// @returns the r, s, and v values of the transaction signature
    static Signature signatureDataToStruct(const Data& signature) noexcept;

    /// Break up the signature into the R, S, and V values, and include chainID in V for replay protection (Eip155)
    /// @returns the r, s, and v values of the transaction signature
    static Signature signatureDataToStructWithEip155(const uint256_t& chainID, const Data& signature) noexcept;
};

} // namespace TW::Ethereum

/// Wrapper for C interface.
struct TWEthereumSigner {
    TW::Ethereum::Signer impl;
};
