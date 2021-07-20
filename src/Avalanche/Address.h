// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../Bech32Address.h"
#include "../Data.h"
#include "../PublicKey.h"

#include <string>

namespace TW::Avalanche {

/// Avalanche address is a Bech32Address, with "avax" prefix and Sha2 hash; and then the chain prefixed to that.
class Address : public Bech32Address {
private:
    void extractKeyHashFromString (const std::string& string);
public:
    static const size_t hashLen;
    static const std::string hrp;

    static bool isValid(const std::string& addr);

    Address() : Bech32Address(hrp) {}

    /// Initializes an address with a key hash.
    Address(const Data& keyHash) : Bech32Address(hrp, keyHash) {}

    /// Initializes an address with a public key.
    Address(const PublicKey& publicKey) : Bech32Address(hrp, HASHER_SHA2_RIPEMD, publicKey) {}

    /// Initializes an address from a string.
    Address(const std::string& string) : Bech32Address(hrp) {
        extractKeyHashFromString(string);
    }

    std::string string() const;
};

inline bool operator==(const Address& lhs, const Address& rhs) {
    // prefer string over byte comparison because string encodes chain and net
    return lhs.string() == rhs.string();
}

inline bool operator!=(const Address& lhs, const Address& rhs) {
    return lhs.string() != rhs.string();
}

inline bool operator<(const Address& lhs, const Address& rhs) {
    // prefer lexicographical comparison of bytes for encoding
    return std::lexicographical_compare(lhs.getKeyHash().begin(), lhs.getKeyHash().end(), rhs.getKeyHash().begin(), rhs.getKeyHash().end());
}

} // namespace TW::Avalanche
