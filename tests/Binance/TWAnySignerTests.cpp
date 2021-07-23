// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "HexCoding.h"
#include "Binance/Address.h"
#include "proto/Binance.pb.h"
#include <TrustWalletCore/TWAnySigner.h>

#include "../interface/TWTestUtilities.h"
#include <gtest/gtest.h>
#include <thread>

using namespace TW;
using namespace TW::Binance;

Proto::SigningOutput SignTest() {
    auto input = Proto::SigningInput();
    input.set_chain_id("Binance-Chain-Nile");
    input.set_account_number(0);
    input.set_sequence(0);
    input.set_source(0);

    auto key = parse_hex("95949f757db1f57ca94a5dff23314accbe7abee89597bf6a3c7382c84d7eb832");
    input.set_private_key(key.data(), key.size());

    auto& order = *input.mutable_send_order();

    Address fromAddress;
    EXPECT_TRUE(Address::decode("bnb1grpf0955h0ykzq3ar5nmum7y6gdfl6lxfn46h2", fromAddress));
    EXPECT_EQ(hex(fromAddress.getKeyHash()), "40c2979694bbc961023d1d27be6fc4d21a9febe6");
    auto fromKeyhash = fromAddress.getKeyHash();
    Address toAddress;
    EXPECT_TRUE(Address::decode("bnb1hlly02l6ahjsgxw9wlcswnlwdhg4xhx38yxpd5", toAddress));
    EXPECT_EQ(hex(toAddress.getKeyHash()), "bffe47abfaede50419c577f1074fee6dd1535cd1");
    auto toKeyhash = toAddress.getKeyHash();

    {
        auto input = order.add_inputs();
        input->set_address(fromKeyhash.data(), fromKeyhash.size());
        auto inputCoin = input->add_coins();
        inputCoin->set_denom("BNB");
        inputCoin->set_amount(1);
    }

    {
        auto output = order.add_outputs();
        output->set_address(toKeyhash.data(), toKeyhash.size());
        auto outputCoin = output->add_coins();
        outputCoin->set_denom("BNB");
        outputCoin->set_amount(1);
    }

    Proto::SigningOutput output;
    ANY_SIGN(input, TWCoinTypeBinance);
    return output;
}

TEST(TWAnySignerBinance, Sign) {
    Proto::SigningOutput output = SignTest();
    ASSERT_EQ(hex(output.encoded()), "b801f0625dee0a462a2c87fa0a1f0a1440c2979694bbc961023d1d27be6fc4d21a9febe612070a03424e421001121f0a14bffe47abfaede50419c577f1074fee6dd1535cd112070a03424e421001126a0a26eb5ae98721026a35920088d98c3888ca68c53dfc93f4564602606cbb87f0fe5ee533db38e50212401b1181faec30b60a2ddaa2804c253cf264c69180ec31814929b5de62088c0c5a45e8a816d1208fc5366bb8b041781a6771248550d04094c3d7a504f9e8310679");
}

TEST(TWAnySignerBinance, SignJSON) {
    auto json = STRING(R"({"chainId":"Binance-Chain-Tigris","accountNumber":"13186","source":"2","memo":"Testing","sendOrder":{"inputs":[{"address":"EuZU7e+eUIuDNzaph9Bp2lqJrts=","coins":[{"denom":"BNB","amount":"1345227"}]}],"outputs":[{"address":"M7vzB7mBRvE9IGk8+UbC13pMryg=","coins":[{"denom":"BNB","amount":"1345227"}]}]}})");
    auto key = DATA("f947b3554a1c2fa70e1caa9de53fbda353348d1e856c593848f3a29737d31f11");
    auto result = WRAPS(TWAnySignerSignJSON(json.get(), key.get(), TWCoinTypeBinance));

    ASSERT_TRUE(TWAnySignerSupportsJSON(TWCoinTypeBinance));
    assertStringsEqual(result, "ca01f0625dee0a4a2a2c87fa0a210a1412e654edef9e508b833736a987d069da5a89aedb12090a03424e4210cb8d5212210a1433bbf307b98146f13d20693cf946c2d77a4caf2812090a03424e4210cb8d52126d0a26eb5ae9872102e58176f271a9796b4288908be85094a2ac978e25535fd59a37b58626e3a84d9e1240015b4beb3d6ef366a7a92fd283f66b8f0d8cdb6b152a9189146b27f84f507f047e248517cf691a36ebc2b7f3b7f64e27585ce1c40f1928d119c31af428efcf3e1882671a0754657374696e672002");
}

TEST(TWAnySignerBinance, MultithreadedSigning) {
    auto f = [](int n) {
        for (int i = 0; i < n; i++) {
            Proto::SigningOutput output = SignTest();
            ASSERT_EQ(hex(output.encoded()), "b801f0625dee0a462a2c87fa0a1f0a1440c2979694bbc961023d1d27be6fc4d21a9febe612070a03424e421001121f0a14bffe47abfaede50419c577f1074fee6dd1535cd112070a03424e421001126a0a26eb5ae98721026a35920088d98c3888ca68c53dfc93f4564602606cbb87f0fe5ee533db38e50212401b1181faec30b60a2ddaa2804c253cf264c69180ec31814929b5de62088c0c5a45e8a816d1208fc5366bb8b041781a6771248550d04094c3d7a504f9e8310679");
        }
    };

    // Ensure multiple threads cause no asserts
    std::thread th1(f, 1000);
    std::thread th2(f, 1000);
    std::thread th3(f, 1000);

    th1.join();
    th2.join();
    th3.join();
}
