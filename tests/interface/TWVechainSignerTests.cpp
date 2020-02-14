// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "HexCoding.h"
#include "proto/VeChain.pb.h"
#include "uint256.h"

#include <TrustWalletCore/TWAnySigner.h>
#include <gtest/gtest.h>

using namespace TW;
using namespace TW::VeChain;

TEST(TWVeChainSigner, Sign) {
    auto input = Proto::SigningInput();

    input.set_chain_tag(1);
    input.set_block_ref(1);
    input.set_expiration(1);
    input.set_gas_price_coef(0);
    input.set_gas(21000);
    input.set_nonce(1);

    auto key = parse_hex("0x4646464646464646464646464646464646464646464646464646464646464646");
    input.set_private_key(key.data(), key.size());

    auto& clause = *input.add_clauses();
    auto amount = parse_hex("31303030"); // 1000
    clause.set_to("0x3535353535353535353535353535353535353535");
    clause.set_value(amount.data(), amount.size());

    auto inputData = input.SerializeAsString();
    auto inputTWData = TWDataCreateWithBytes((const byte*)inputData.data(), inputData.size());
    auto outputTWData = TWAnySignerSign(inputTWData, TWCoinTypeVeChain);
    auto output = Proto::SigningOutput();
    output.ParseFromArray(TWDataBytes(outputTWData), TWDataSize(outputTWData));

    ASSERT_EQ(hex(output.encoded()), "f86a010101dcdb943535353535353535353535353535353535353535843130303080808252088001c0b841bf8edf9600e645b5abd677cb52f585e7f655d1361075d511b37f707a9f31da6702d28739933b264527a1d05b046f5b74044b88c30c3f5a09d616bd7a4af4901601");
}
