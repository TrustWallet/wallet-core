// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "TxComparisonHelper.h"
#include "Bitcoin/OutPoint.h"
#include "Bitcoin/Script.h"
#include "Bitcoin/TransactionPlan.h"
#include "Bitcoin/TransactionBuilder.h"
#include "Bitcoin/FeeCalculator.h"
#include "HexCoding.h"
#include "proto/Bitcoin.pb.h"
#include <TrustWalletCore/TWCoinType.h>

#include <gtest/gtest.h>

using namespace TW;
using namespace TW::Bitcoin;


TEST(TransactionPlan, OneTypical) {
    auto utxos = buildTestUTXOs({100'000});
    auto byteFee = 1;
    auto sigingInput = buildSigningInput(50'000, byteFee, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {100'000}, 50'000, 147));

    auto& feeCalculator = getFeeCalculator(TWCoinTypeBitcoin);
    EXPECT_EQ(feeCalculator.calculate(1, 2, byteFee), 174);
}

TEST(TransactionPlan, OneInsufficient) {
    auto utxos = buildTestUTXOs({100'000});
    auto sigingInput = buildSigningInput(200'000, 1, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    // Max is returned
    EXPECT_TRUE(verifyPlan(txPlan, {100'000}, 99'887, 113));
}

TEST(TransactionPlan, OneInsufficientEqual) {
    auto utxos = buildTestUTXOs({100'000});
    auto sigingInput = buildSigningInput(100'000, 1, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    // Max is returned
    EXPECT_TRUE(verifyPlan(txPlan, {100'000}, 99'887, 113));
}

TEST(TransactionPlan, OneInsufficientLower100) {
    // requested is only slightly lower than avail, not enough for fee, cannot be satisfied
    auto utxos = buildTestUTXOs({100'000});
    auto sigingInput = buildSigningInput(100'000 - 100, 1, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {}, 0, 0, Common::Proto::Error_not_enough_utxos));
}

TEST(TransactionPlan, OneInsufficientLower170) {
    // requested is only slightly lower than avail, not enough for fee, cannot be satisfied
    auto utxos = buildTestUTXOs({100'000});
    auto sigingInput = buildSigningInput(100'000 - 170, 1, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {}, 0, 0, Common::Proto::Error_not_enough_utxos));
}

TEST(TransactionPlan, OneInsufficientLower300) {
    auto utxos = buildTestUTXOs({100'000});
    auto sigingInput = buildSigningInput(100'000 - 300, 1, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {100'000}, 100'000 - 300, 147));
}

TEST(TransactionPlan, OneMoreRequested) {
    auto utxos = buildTestUTXOs({100'000});
    auto byteFee = 1;
    auto sigingInput = buildSigningInput(150'000, byteFee, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    // Max is returned
    EXPECT_TRUE(verifyPlan(txPlan, {100'000}, 99'887, 113));
}

TEST(TransactionPlan, OneFitsExactly) {
    auto utxos = buildTestUTXOs({100'000});
    auto byteFee = 1;
    auto expectedFee = 147;
    auto sigingInput = buildSigningInput(100'000 - 174, byteFee, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {100'000}, 100'000 - 174, expectedFee));

    auto& feeCalculator = getFeeCalculator(TWCoinTypeBitcoin);
    EXPECT_EQ(feeCalculator.calculate(1, 2, byteFee), 174);
}

TEST(TransactionPlan, OneFitsExactlyHighFee) {
    auto utxos = buildTestUTXOs({100'000});
    auto byteFee = 10;
    auto expectedFee = 1470;
    auto sigingInput = buildSigningInput(100'000 - 1740, byteFee, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {100'000}, 100'000 - 1740, expectedFee));

    auto& feeCalculator = getFeeCalculator(TWCoinTypeBitcoin);
    EXPECT_EQ(feeCalculator.calculate(1, 2, byteFee), 1740);
}

TEST(TransactionPlan, TwoFirstEnough) {
    auto utxos = buildTestUTXOs({20'000, 80'000});
    auto sigingInput = buildSigningInput(15'000, 1, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {20'000}, 15'000, 147));
}

TEST(TransactionPlan, TwoSecondEnough) {
    auto utxos = buildTestUTXOs({20'000, 80'000});
    auto sigingInput = buildSigningInput(70'000, 1, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {80'000}, 70'000, 147));
}

TEST(TransactionPlan, TwoBoth) {
    auto utxos = buildTestUTXOs({20'000, 80'000});
    auto sigingInput = buildSigningInput(90'000, 1, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {20'000, 80'000}, 90'000, 215));
}

TEST(TransactionPlan, TwoFirstEnoughButSecond) {
    auto utxos = buildTestUTXOs({20'000, 22'000});
    auto sigingInput = buildSigningInput(18'000, 1, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {22'000}, 18'000, 147));
}

TEST(TransactionPlan, ThreeNoDust) {
    auto utxos = buildTestUTXOs({100'000, 70'000, 75'000});
    auto sigingInput = buildSigningInput(100'000 - 174 - 10, 1, utxos);

    // 100'000 would fit with dust; instead two UTXOs are selected not to leave dust
    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {75'000, 100'000}, 100'000 - 174 - 10, 215));

    auto& feeCalculator = getFeeCalculator(TWCoinTypeBitcoin);
    EXPECT_EQ(feeCalculator.calculate(1, 2, 1), 174);
    EXPECT_EQ(feeCalculator.calculate(2, 2, 1), 275);

    const auto dustLimit = 102;
    // Now 100'000 fits with no dust
    sigingInput = buildSigningInput(100'000 - 174 - dustLimit, 1, utxos);
    txPlan = TransactionBuilder::plan(sigingInput);
    EXPECT_TRUE(verifyPlan(txPlan, {100'000}, 100'000 - 174 - dustLimit, 147));

    // One more and we are over dust limit
    sigingInput = buildSigningInput(100'000 - 174 - dustLimit + 1, 1, utxos);
    txPlan = TransactionBuilder::plan(sigingInput);
    EXPECT_TRUE(verifyPlan(txPlan, {75'000, 100'000}, 100'000 - 174 - dustLimit + 1, 215));
}

TEST(TransactionPlan, TenThree) {
    auto utxos = buildTestUTXOs({1'000, 2'000, 100'000, 3'000, 4'000, 5,000, 125'000, 6'000, 150'000, 7'000});
    auto sigingInput = buildSigningInput(300'000, 1, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {100'000, 125'000, 150'000}, 300'000, 283));
}

TEST(TransactionPlan, NonMaxAmount) {
    auto utxos = buildTestUTXOs({4000, 2000, 6000, 1000, 50000, 120000});
    auto sigingInput = buildSigningInput(10000, 1, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {50000}, 10000, 147));
}

TEST(TransactionPlan, UnspentsInsufficient) {
    auto utxos = buildTestUTXOs({4000, 4000, 4000});
    auto sigingInput = buildSigningInput(15000, 1, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    // Max is returned
    EXPECT_TRUE(verifyPlan(txPlan, {4000, 4000, 4000}, 11751, 249));
}

TEST(TransactionPlan, SelectionSuboptimal_ExtraSmallUtxo) {
    // Solution found 4-in-2-out {500, 600, 800, 1000} avail 2900 txamount 1570 fee 702 change 628
    // Better solution: 3-in-2-out {600, 800, 1000} avail 2400 txamount 1570 fee 566 change 264
    // Previously, with with higher fee estimation used in UTXO selection, solution found was 5-in-2-out {400, 500, 600, 800, 1000} avail 3300 txamount 1570 fee 838 change 892
    auto utxos = buildTestUTXOs({400, 500, 600, 800, 1'000});
    auto byteFee = 2;
    auto sigingInput = buildSigningInput(1'570, byteFee, utxos);

    // UTXOs smaller than singleInputFee are not included
    auto txPlan = TransactionBuilder::plan(sigingInput);

    auto expectedFee = 702;
    EXPECT_TRUE(verifyPlan(txPlan, {500, 600, 800, 1'000}, 1'570, expectedFee));
    auto change = 2'900 - 1'570 - expectedFee;
    auto firstUtxo = txPlan.utxos[0].amount();
    EXPECT_TRUE(change - 204 < txPlan.utxos[0].amount());
    EXPECT_EQ(change, 628);
    EXPECT_EQ(firstUtxo, 500);
}

TEST(TransactionPlan, Selection_Satisfied5) {
    // 5-input case, with a 5-input solution.
    // Previously, with with higher fee estimation used in UTXO selection, no solution would be found.
    auto utxos = buildTestUTXOs({400, 500, 600, 800, 1'000});
    auto byteFee = 2;
    auto sigingInput = buildSigningInput(1'775, byteFee, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {400, 500, 600, 800, 1000}, 1775, 838));
}

TEST(TransactionPlan, Inputs5_33Req19NoDustFee2) {
    auto utxos = buildTestUTXOs({600, 1'200, 6'000, 8'000, 10'000});
    auto byteFee = 2;
    auto sigingInput = buildSigningInput(19'000, byteFee, utxos);

    // UTXOs smaller than singleInputFee are not included
    auto txPlan = TransactionBuilder::plan(sigingInput);

    auto expectedFee = 283*byteFee;
    EXPECT_TRUE(verifyPlan(txPlan, {6'000, 8'000, 10'000}, 19'000, expectedFee));

    auto& feeCalculator = getFeeCalculator(TWCoinTypeBitcoin);
    EXPECT_EQ(feeCalculator.calculateSingleInput(byteFee), 204);
}

TEST(TransactionPlan, Inputs5_33Req19Dust1Fee5) {
    auto utxos = buildTestUTXOs({600, 1'200, 6'000, 8'000, 10'000});
    auto byteFee = 5;
    auto sigingInput = buildSigningInput(19'000, byteFee, utxos);

    // UTXOs smaller than singleInputFee are not included
    auto txPlan = TransactionBuilder::plan(sigingInput);

    auto expectedFee = 283*byteFee;
    EXPECT_TRUE(verifyPlan(txPlan, {6'000, 8'000, 10'000}, 19'000, expectedFee));

    auto& feeCalculator = getFeeCalculator(TWCoinTypeBitcoin);
    EXPECT_EQ(feeCalculator.calculateSingleInput(byteFee), 510);
}

TEST(TransactionPlan, Inputs5_33Req19Dust1Fee9) {
    auto utxos = buildTestUTXOs({600, 1'200, 6'000, 8'000, 10'000});
    auto byteFee = 9;
    auto sigingInput = buildSigningInput(19'000, byteFee, utxos);

    // UTXOs smaller than singleInputFee are not included
    auto txPlan = TransactionBuilder::plan(sigingInput);

    auto expectedFee = 283*byteFee;
    EXPECT_TRUE(verifyPlan(txPlan, {6'000, 8'000, 10'000}, 19'000, expectedFee));

    auto& feeCalculator = getFeeCalculator(TWCoinTypeBitcoin);
    EXPECT_EQ(feeCalculator.calculateSingleInput(byteFee), 918);
}

TEST(TransactionPlan, Inputs5_33Req19Fee20) {
    auto utxos = buildTestUTXOs({600, 1'200, 6'000, 8'000, 10'000});
    auto byteFee = 20;
    auto sigingInput = buildSigningInput(19'000, byteFee, utxos);

    // UTXOs smaller than singleInputFee are not included
    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {}, 0, 0, Common::Proto::Error_not_enough_utxos));
}

TEST(TransactionPlan, Inputs5_33Req13Fee20) {
    auto utxos = buildTestUTXOs({600, 1'200, 6'000, 8'000, 10'000});
    auto byteFee = 20;
    auto sigingInput = buildSigningInput(13'000, byteFee, utxos);

    // UTXOs smaller than singleInputFee are not included
    auto txPlan = TransactionBuilder::plan(sigingInput);

    auto expectedFee = 283*byteFee;
    EXPECT_TRUE(verifyPlan(txPlan, {6'000, 8'000, 10'000}, 13'000, expectedFee));

    auto& feeCalculator = getFeeCalculator(TWCoinTypeBitcoin);
    EXPECT_EQ(feeCalculator.calculateSingleInput(byteFee), 2040);
}

TEST(TransactionPlan, NoUTXOs) {
    auto utxos = buildTestUTXOs({});
    auto sigingInput = buildSigningInput(15000, 1, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {}, 0, 0, Common::Proto::Error_missing_input_utxos));
}

TEST(TransactionPlan, CustomCase) {
    auto utxos = buildTestUTXOs({794121, 2289357});
    auto byteFee = 61;
    auto sigingInput = buildSigningInput(2287189, byteFee, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {794121, 2289357}, 2287189, 13115));

    auto& feeCalculator = getFeeCalculator(TWCoinTypeBitcoin);
    EXPECT_EQ(feeCalculator.calculate(2, 2, byteFee), 16775);
}

TEST(TransactionPlan, Target0) {
    auto utxos = buildTestUTXOs({2000, 3000});
    auto sigingInput = buildSigningInput(0, 1, utxos);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {}, 0, 0, Common::Proto::Error_zero_amount_requested));
}

TEST(TransactionPlan, MaxAmount) {
    auto utxos = buildTestUTXOs({4000, 2000, 15000, 15000, 3000, 200});
    ASSERT_EQ(sumUTXOs(utxos), 39200);
    auto byteFee = 40;
    auto sigingInput = buildSigningInput(39200, byteFee, utxos, true);

    auto& feeCalculator = getFeeCalculator(TWCoinTypeBitcoin);
    EXPECT_EQ(feeCalculator.calculateSingleInput(byteFee), 4080);

    // UTXOs smaller than singleInputFee are not included
    auto txPlan = TransactionBuilder::plan(sigingInput);

    auto expectedFee = 7240;
    EXPECT_TRUE(verifyPlan(txPlan, {15000, 15000}, 30000 - expectedFee, expectedFee));
}

TEST(TransactionPlan, MaxAmountOne) {
    auto utxos = buildTestUTXOs({10189534});
    auto sigingInput = buildSigningInput(100, 1, utxos, true);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    auto expectedFee = 113;
    EXPECT_TRUE(verifyPlan(txPlan, {10189534}, 10189534 - expectedFee, expectedFee));
}

TEST(TransactionPlan, AmountEqualsMaxButNotUseMax) {
    // amount is set to max, but UseMax is not set --> Max is returned
    auto utxos = buildTestUTXOs({10189534});
    auto sigingInput = buildSigningInput(10189534, 1, utxos, false);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {10189534}, 10189421, 113));
}

TEST(TransactionPlan, MaxAmountRequestedIsLower) {
    auto utxos = buildTestUTXOs({4000, 2000, 15000, 15000, 3000, 200});
    ASSERT_EQ(sumUTXOs(utxos), 39200);
    auto byteFee = 40;
    auto sigingInput = buildSigningInput(10, byteFee, utxos, true);

    auto& feeCalculator = getFeeCalculator(TWCoinTypeBitcoin);
    EXPECT_EQ(feeCalculator.calculateSingleInput(byteFee), 4080);

    // UTXOs smaller than singleInputFee are not included
    auto txPlan = TransactionBuilder::plan(sigingInput);

    auto expectedFee = 7240;
    EXPECT_TRUE(verifyPlan(txPlan, {15000, 15000}, 30000 - expectedFee, expectedFee));
}

TEST(TransactionPlan, MaxAmountRequestedZero) {
    auto utxos = buildTestUTXOs({4000, 2000, 15000, 15000, 3000, 200});
    ASSERT_EQ(sumUTXOs(utxos), 39200);
    auto byteFee = 40;
    auto sigingInput = buildSigningInput(0, byteFee, utxos, true);

    auto& feeCalculator = getFeeCalculator(TWCoinTypeBitcoin);
    EXPECT_EQ(feeCalculator.calculateSingleInput(byteFee), 4080);

    // UTXOs smaller than singleInputFee are not included
    auto txPlan = TransactionBuilder::plan(sigingInput);

    auto expectedFee = 7240;
    EXPECT_TRUE(verifyPlan(txPlan, {15000, 15000}, 30000 - expectedFee, expectedFee));
}

TEST(TransactionPlan, MaxAmountNoDustFee2) {
    auto utxos = buildTestUTXOs({400, 500, 600, 800, 1000});
    auto byteFee = 2;
    auto sigingInput = buildSigningInput(100, byteFee, utxos, true);

    // UTXOs smaller than singleInputFee are not included
    auto txPlan = TransactionBuilder::plan(sigingInput);

    auto expectedFee = 770;
    EXPECT_TRUE(verifyPlan(txPlan, {400, 500, 600, 800, 1000}, 3'300 - expectedFee, expectedFee));

    auto& feeCalculator = getFeeCalculator(TWCoinTypeBitcoin);
    EXPECT_EQ(feeCalculator.calculateSingleInput(byteFee), 204);
    EXPECT_EQ(feeCalculator.calculate(5, 1, byteFee), 1096);
}

TEST(TransactionPlan, MaxAmountDust1Fee4) {
    auto utxos = buildTestUTXOs({400, 500, 600, 800, 1000});
    auto byteFee = 4;
    auto sigingInput = buildSigningInput(100, byteFee, utxos, true);

    // UTXOs smaller than singleInputFee are not included
    auto txPlan = TransactionBuilder::plan(sigingInput);

    auto expectedFee = 1268;
    EXPECT_TRUE(verifyPlan(txPlan, {500, 600, 800, 1000}, 2'900 - expectedFee, expectedFee));

    auto& feeCalculator = getFeeCalculator(TWCoinTypeBitcoin);
    EXPECT_EQ(feeCalculator.calculateSingleInput(byteFee), 408);
    EXPECT_EQ(feeCalculator.calculate(4, 1, byteFee), 1784);
}

TEST(TransactionPlan, MaxAmountDust2Fee5) {
    auto utxos = buildTestUTXOs({400, 500, 600, 800, 1000});
    auto byteFee = 5;
    auto sigingInput = buildSigningInput(100, byteFee, utxos, true);

    // UTXOs smaller than singleInputFee are not included
    auto txPlan = TransactionBuilder::plan(sigingInput);

    auto expectedFee = 1245;
    EXPECT_TRUE(verifyPlan(txPlan, {600, 800, 1000}, 2'400 - expectedFee, expectedFee));

    auto& feeCalculator = getFeeCalculator(TWCoinTypeBitcoin);
    EXPECT_EQ(feeCalculator.calculateSingleInput(byteFee), 510);
    EXPECT_EQ(feeCalculator.calculate(3, 1, byteFee), 1725);
}

TEST(TransactionPlan, MaxAmountDustAllFee10) {
    auto utxos = buildTestUTXOs({400, 500, 600, 800, 1000});
    auto byteFee = 10;
    auto sigingInput = buildSigningInput(100, byteFee, utxos, true);

    // UTXOs smaller than singleInputFee are not included
    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {}, 0, 0, Common::Proto::Error_not_enough_utxos));

    auto& feeCalculator = getFeeCalculator(TWCoinTypeBitcoin);
    EXPECT_EQ(feeCalculator.calculateSingleInput(byteFee), 1020);
}

TEST(TransactionPlan, One_MaxAmount_FeeMoreThanAvailable) {
    auto utxos = buildTestUTXOs({170});
    auto byteFee = 1;
    auto expectedFee = 113;
    auto sigingInput = buildSigningInput(300, byteFee, utxos, true);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    // Fee is reduced to availableAmount
    EXPECT_TRUE(verifyPlan(txPlan, {170}, 170 - expectedFee, expectedFee));

    auto& feeCalculator = getFeeCalculator(TWCoinTypeBitcoin);
    EXPECT_EQ(feeCalculator.calculate(1, 1, byteFee), 143);
}

TEST(TransactionPlan, MaxAmountDoge) {
    auto utxos = buildTestUTXOs({Amount(100000000), Amount(2000000000), Amount(200000000)});
    ASSERT_EQ(sumUTXOs(utxos), Amount(2300000000));
    auto sigingInput = buildSigningInput(Amount(2300000000), 100, utxos, true, TWCoinTypeDogecoin);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {100000000, 2000000000, 200000000}, 2299951200, 48800));
}

TEST(TransactionPlan, AmountDecred) {
    auto utxos = buildTestUTXOs({Amount(39900000)});
    auto sigingInput = buildSigningInput(Amount(10000000), 10, utxos, false, TWCoinTypeDecred);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {39900000}, 10000000, 2540));
}

TEST(TransactionPlan, LotsofUtxosNonmax) {
    const auto n = 1000;
    const auto byteFee = 10;
    std::vector<int64_t> values;
    uint64_t valueSum = 0;
    for (int i = 0; i < n; ++i) {
        const auto val = (i + 1) * 100;
        values.push_back(val);
        valueSum += val;
    }
    const auto requestedAmount = valueSum / 2 + 123;

    auto utxos = buildTestUTXOs(values);
    auto sigingInput = buildSigningInput(requestedAmount, byteFee, utxos, false, TWCoinTypeBitcoin);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    // expected result: 298 utxos, with amounts 70300,70400,70500,...,100000. 
    std::vector<int64_t> subset;
    uint64_t subsetSum = 0;
    for (int i = n - 298; i < n; ++i) {
        const auto val = (i + 1) * 100;
        subset.push_back(val);
        subsetSum += val;
    }
    EXPECT_TRUE(verifyPlan(txPlan, subset, requestedAmount, 203'450));
}

TEST(TransactionPlan, LotsofUtxosMax) {
    const auto n = 1000;
    const auto byteFee = 10;
    std::vector<int64_t> values;
    uint64_t valueSum = 0;
    for (int i = 0; i < n; ++i) {
        const auto val = (i + 1) * 100;
        values.push_back(val);
        valueSum += val;
    }

    // Use Ravencoin, because of faster non-segwit estimation, and one original issues was with this coin.
    auto utxos = buildTestUTXOs(values);
    auto sigingInput = buildSigningInput(valueSum, byteFee, utxos, true, TWCoinTypeRavencoin);

    auto txPlan = TransactionBuilder::plan(sigingInput);

    // a few smallest UTXOs are filtered out
    const auto dustLimit = byteFee * 148;
    std::vector<int64_t> filteredValues;
    uint64_t filteredValueSum = 0;
    for (int i = 0; i < n; ++i) {
        const auto val = (i + 1) * 100;
        if (val > dustLimit) {
            filteredValues.push_back(val);
            filteredValueSum += val;
        }
    }
    EXPECT_EQ(valueSum, 50'050'000);
    EXPECT_EQ(dustLimit, 1480);
    EXPECT_EQ(filteredValues.size(), 986);
    EXPECT_EQ(filteredValueSum, 50'039'500);
    EXPECT_TRUE(verifyPlan(txPlan, filteredValues, 48'579'780, 1'459'720));
}

TEST(TransactionPlan, OpReturn) {
    auto ownAddress = "bc1q7s0a2l4aguksehx8hf93hs9yggl6njxds6m02g";
    auto toAddress = "bc1qxu5a8gtnjxw3xwdlmr2gl9d76h9fysu3zl656e";
    auto utxoAmount = 342101;
    auto toAmount = 300000;
    int byteFee = 126;
    Data memo = data("SWAP:THOR.RUNE:thor1tpercamkkxec0q0jk6ltdnlqvsw29guap8wmcl:");

    auto signingInput = Proto::SigningInput();
    signingInput.set_hash_type(TWBitcoinSigHashTypeAll);
    signingInput.set_amount(toAmount);
    signingInput.set_byte_fee(byteFee);
    signingInput.set_to_address(toAddress);
    signingInput.set_change_address(ownAddress);
    signingInput.set_output_op_return(memo.data(), memo.size());

    auto& utxo = *signingInput.add_utxo();
    auto utxoHash = parse_hex("30b82960291a39de3664ec4c844a815e3e680e29b4d3a919e450f0c119cf4e35");
    std::reverse(utxoHash.begin(), utxoHash.end());
    utxo.mutable_out_point()->set_hash(utxoHash.data(), utxoHash.size());
    utxo.mutable_out_point()->set_index(1);
    utxo.mutable_out_point()->set_sequence(UINT32_MAX);
    utxo.set_amount(utxoAmount);

    auto txPlan = TransactionBuilder::plan(signingInput);

    EXPECT_TRUE(verifyPlan(txPlan, {342101}, 300000, 205 * byteFee));
    EXPECT_EQ(txPlan.outputOpReturn.size(), 59);
    EXPECT_EQ(hex(txPlan.outputOpReturn), "535741503a54484f522e52554e453a74686f72317470657263616d6b6b7865633071306a6b366c74646e6c7176737732396775617038776d636c3a");

    auto& feeCalculator = getFeeCalculator(TWCoinTypeBitcoin);
    EXPECT_EQ(feeCalculator.calculate(1, 2, byteFee), 174 * byteFee);
    EXPECT_EQ(feeCalculator.calculate(1, 3, byteFee), 205 * byteFee);
}
