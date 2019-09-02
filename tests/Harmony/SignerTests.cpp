// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Ethereum/RLP.h"
#include "Harmony-One/Address.h"
#include "Harmony-One/Signer.h"
#include "HexCoding.h"

#include <gtest/gtest.h>

namespace TW::Harmony {

using boost::multiprecision::uint256_t;

class SignerExposed : public Signer {
  public:
    SignerExposed(boost::multiprecision::uint256_t chainID) : Signer(chainID) {}
    using Signer::hash;
};

TEST(Signer, Hash) {
    auto address = Address("0x3535353535353535353535353535353535353535");
    auto transaction = Transaction(
        /* nonce: */ 9,
        /* gasPrice: */ 20000000000,
        /* gasLimit: */ 21000,
        /* fromShardID */ 1,
        /* toShardID */ 1,
        /* to: */ address,
        /* amount: */ 1000000000000000000,
        /* payload: */ {});
    auto signer = SignerExposed(1);
    auto hash = signer.hash(transaction);
    // ASSERT_EQ(hex(hash), "daf5a779ae972f972197303d7b574746c7ef83eadac0f2791ad23db92e4c8e53");
}

TEST(Signer, Sign) {
    auto address = Address("0x3535353535353535353535353535353535353535");
    auto transaction = Transaction(
        /* nonce: */ 9,
        /* gasPrice: */ 20000000000,
        /* gasLimit: */ 21000,
        /* fromShardID */ 1,
        /* toShardID */ 1,
        /* to: */ address,
        /* amount: */ 1000000000000000000,
        /* payload: */ {});
    auto key =
        PrivateKey(parse_hex("0x4646464646464646464646464646464646464646464646464646464646464646"));
    auto signer = SignerExposed(1);
    signer.sign(key, transaction);
    // ASSERT_EQ(transaction.v, 37);
    // ASSERT_EQ(
    //     transaction.r,
    //     uint256_t("18515461264373351373200002665853028612451056578545711640558177340181847433846"));
    ASSERT_EQ(transaction.s, uint256_t(""));
}

} // namespace TW::Harmony