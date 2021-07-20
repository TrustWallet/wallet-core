// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.
//
// This is a GENERATED FILE, changes made here MAY BE LOST.
// Generated one-time (codegen/bin/cointests)
//

#include "../interface/TWTestUtilities.h"
#include <TrustWalletCore/TWCoinTypeConfiguration.h>
#include <gtest/gtest.h>


TEST(TWAvalancheCoinType, TWCoinType) {
    auto symbol = WRAPS(TWCoinTypeConfigurationGetSymbol(TWCoinTypeAvalancheXChain));
    auto txId = WRAPS(TWStringCreateWithUTF8Bytes("2eLNtbK8bomFKJ6qCGBTuxJvD6LAs6eAXNVK48hGS8MGxGW6B9"));
    auto txUrl = WRAPS(TWCoinTypeConfigurationGetTransactionURL(TWCoinTypeAvalancheXChain, txId.get()));
    auto accId = WRAPS(TWStringCreateWithUTF8Bytes("X-avax19cp7nwkex3kynxug5s3lskdxhdns2vk7yw4m8h"));
    auto accUrl = WRAPS(TWCoinTypeConfigurationGetAccountURL(TWCoinTypeAvalancheXChain, accId.get()));
    auto id = WRAPS(TWCoinTypeConfigurationGetID(TWCoinTypeAvalancheXChain));
    auto name = WRAPS(TWCoinTypeConfigurationGetName(TWCoinTypeAvalancheXChain));

    ASSERT_EQ(TWCoinTypeConfigurationGetDecimals(TWCoinTypeAvalancheXChain), 9);
    ASSERT_EQ(TWBlockchainAvalanche, TWCoinTypeBlockchain(TWCoinTypeAvalancheXChain));
    ASSERT_EQ(0x0, TWCoinTypeP2shPrefix(TWCoinTypeAvalancheXChain));
    ASSERT_EQ(0x0, TWCoinTypeStaticPrefix(TWCoinTypeAvalancheXChain));
    assertStringsEqual(symbol, "AVAX");
    assertStringsEqual(txUrl, "https://explorer.avax.network/tx/2eLNtbK8bomFKJ6qCGBTuxJvD6LAs6eAXNVK48hGS8MGxGW6B9");
    assertStringsEqual(accUrl, "https://explorer.avax.network/address/X-avax19cp7nwkex3kynxug5s3lskdxhdns2vk7yw4m8h");
    assertStringsEqual(id, "avalanchex");
    assertStringsEqual(name, "Avalanche X-Chain");
}
