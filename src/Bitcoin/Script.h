// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../Data.h"

#include "OpCodes.h"
#include <TrustWalletCore/TWCoinType.h>

#include <string>
#include <vector>
#include <cassert>

namespace TW::Bitcoin {

class Script {
  public:
    /// Script raw bytes.
    Data bytes;

    /// Initializes an empty script.
    Script() = default;

    /// Initializes a script with a collection of raw bytes.
    template <typename It>
    Script(It begin, It end) : bytes(begin, end) {}

    /// Initializaes a script with a collection of raw bytes by moving.
    explicit Script(const Data& bytes) : bytes(bytes) {}

    /// Whether the script is empty.
    bool empty() const { return bytes.empty(); }

    /// Returns the script's script hash.
    Data hash() const;

    /// Determines whether this is a pay-to-script-hash (P2SH) script.
    bool isPayToScriptHash() const;

    /// Determines whether this is a pay-to-witness-script-hash (P2WSH) script.
    bool isPayToWitnessScriptHash() const;

    /// Determines whether this is a pay-to-witness-public-key-hash (P2WPKH) script.
    bool isPayToWitnessPublicKeyHash() const;

    /// Determines whether this is a witness programm script.
    bool isWitnessProgram() const;

    /// Matches the script to a pay-to-public-key (P2PK) script.
    bool matchPayToPublicKey(Data& publicKey) const;

    /// Matches the script to a pay-to-public-key-hash (P2PKH).
    bool matchPayToPublicKeyHash(Data& keyHash) const;

    /// Matches the script to a pay-to-script-hash (P2SH).
    bool matchPayToScriptHash(Data& scriptHash) const;

    /// Matches the script to a pay-to-witness-public-key-hash (P2WPKH).
    bool matchPayToWitnessPublicKeyHash(Data& keyHash) const;

    /// Matches the script to a pay-to-witness-script-hash (P2WSH).  Returns the script hash, a SHA256 of the witness script.
    bool matchPayToWitnessScriptHash(Data& scriptHash) const;

    /// Matches the script to a multisig script.
    bool matchMultisig(std::vector<Data>& publicKeys, int& required) const;

    /// Builds a pay-to-public-key-hash (P2PKH) script from a public key hash.
    static Script buildPayToPublicKeyHash(const Data& hash);

    /// Builds a pay-to-script-hash (P2SH) script from a script hash.
    static Script buildPayToScriptHash(const Data& scriptHash);

    /// Builds a pay-to-witness-program script, P2WSH or P2WPKH.
    static Script buildPayToWitnessProgram(const Data& program);

    /// Builds a pay-to-witness-public-key-hash (P2WPKH) script from a public
    /// key hash.
    static Script buildPayToWitnessPublicKeyHash(const Data& hash);

    /// Builds a pay-to-witness-script-hash (P2WSH) script from a script hash.
    static Script buildPayToWitnessScriptHash(const Data& scriptHash);

    /// Builds an OP_RETURN script with given data
    static Script buildOpReturnScript(const Data& data);

    /// Builds a appropriate lock script for the given
    /// address.
    static Script lockScriptForAddress(const std::string& address, enum TWCoinType coin);

    /// Encodes the script.
    void encode(Data& data) const;

    /// Encodes a small integer
    static inline uint8_t encodeNumber(int n) {
        assert(n >= 0 && n <= 16);
        if (n == 0) {
            return OP_0;
        }
        return OP_1 + uint8_t(n - 1);
    }

    /// Decodes a small integer
    static inline int decodeNumber(uint8_t opcode) {
        if (opcode == OP_0) {
            return 0;
        }
        assert(opcode >= OP_1 && opcode <= OP_16);
        return static_cast<int>(opcode) - static_cast<int>(OP_1 - 1);
    }

    /// Extracts a single opcode at the given index including its operand.
    /// Public for testability.
    ///
    /// \param index [in/out] index where the operation starts, on return the
    /// index of the next operation. \param opcode [out] the opcode. \param
    /// operand [out] the opcode's operand. \returns whether an opcode was
    /// available.
    bool getScriptOp(size_t& index, uint8_t& opcode, Data& operand) const;
};

inline bool operator==(const Script& lhs, const Script& rhs) {
    return lhs.bytes == rhs.bytes;
}
inline bool operator!=(const Script& lhs, const Script& rhs) {
    return !(lhs == rhs);
}

} // namespace TW::Bitcoin

/// Wrapper for C interface.
struct TWBitcoinScript {
    TW::Bitcoin::Script impl;
};
