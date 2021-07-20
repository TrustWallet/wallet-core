// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../CoinEntry.h"

namespace TW::Avalanche {

/// Entry point for implementation of Avalanche coin.
/// Note: do not put the implementation here (no matter how simple), to avoid having coin-specific includes in this file
class Entry: public CoinEntry {
public:
    virtual const std::vector<TWCoinType> coinTypes() const { return {TWCoinTypeAvalancheXChain}; }
    virtual bool validateAddress(TWCoinType coin, const std::string& address, TW::byte p2pkh, TW::byte p2sh, const char* hrp) const;
    virtual std::string deriveAddress(TWCoinType coin, const PublicKey& publicKey, TW::byte p2pkh, const char* hrp) const;
    virtual void sign(TWCoinType coin, const Data& dataIn, Data& dataOut) const;
    // normalizeAddress(): implement this if needed, e.g. Ethereum address is EIP55 checksummed
};

} // namespace TW::Avalanche
