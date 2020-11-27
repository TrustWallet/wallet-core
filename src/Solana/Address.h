// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../Base58Address.h"
#include "../PublicKey.h"

#include <vector>
#include <string>

namespace TW::Solana {

class Address : public Base58Address<32> {
  public:
    /// Determines whether a collection of bytes makes a valid Solana address.
    static bool isValid(const Data& data) { return data.size() == size; }

    /// Determines whether a string makes a valid address.
    static bool isValid(const std::string& string);

    /// Initializes an address with a string representation.
    explicit Address(const std::string& string);

    /// Initializes an address with a public key.
    explicit Address(const PublicKey& publicKey);

    /// Returns a string representation of the address.
    std::string string() const;

    /// Returns a vector of the address.
    Data vector() const;

    /// Check if given address is on ed25519 curve
    static bool isValidOnCurve(const Data& data);
};

} // namespace TW::Solana
