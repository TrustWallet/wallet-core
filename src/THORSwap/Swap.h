// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Data.h"

#include <string>
#include <utility>

namespace TW::THORSwap {

enum Chain {
    THOR = 0,
    BTC = 1,
    ETH = 2,
    BNB = 3,
};

/// Building THORChain cross-chain transactions
class Swap {
public:
    /// Logic to build a native transaction on the source chain for a swap
    /// Returns serialized SigningInput proto message, on the source chain, and an optional error message
    static std::pair<Data, std::string> build(
        Chain fromChain,
        Chain toChain,
        const std::string& fromAddress,
        const std::string& toSymbol,
        const std::string& toTokenId,
        const std::string& toAddress,
        const std::string& vaultAddress,
        const std::string& routerAddress, // for Ethereum, can be empty otherwise
        const std::string& fromAmount,
        const std::string& toAmountLimit
    );

protected:
    static std::string buildBitcoin(Chain toChain, const std::string& toSymbol, const std::string& toTokenId, const std::string& fromAddress, const std::string& toAddress, const std::string& vaultAddress, uint64_t amount, const std::string& memo, Data& out);
    static std::string buildEthereum(Chain toChain, const std::string& toSymbol, const std::string& toTokenId, const std::string& fromAddress, const std::string& toAddress, const std::string& vaultAddress, const std::string& routerAddress, uint64_t amount, const std::string& memo, Data& out);
    static std::string buildBinance(Chain toChain, const std::string& toSymbol, const std::string& toTokenId, const std::string& fromAddress, const std::string& toAddress, const std::string& vaultAddress, uint64_t amount, const std::string& memo, Data& out);

public:
    static std::string buildMemo(Chain toChain, const std::string& toSymbol, const std::string& toAddress, uint64_t limit);
};

} // namespace TW
