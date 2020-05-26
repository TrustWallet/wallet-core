// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Bitcoin/Address.h"
#include "HexCoding.h"
#include "proto/Bitcoin.pb.h"
#include "../interface/TWTestUtilities.h"

#include <TrustWalletCore/TWBitcoinSigHashType.h>

#include <TrustWalletCore/TWAnyAddress.h>
#include <TrustWalletCore/TWAnySigner.h>
#include <TrustWalletCore/TWBitcoinAddress.h>
#include <TrustWalletCore/TWBitcoinScript.h>
#include <TrustWalletCore/TWHDWallet.h>
#include <TrustWalletCore/TWHash.h>
#include <TrustWalletCore/TWPrivateKey.h>

#include <gtest/gtest.h>

using namespace TW;
using namespace TW::Bitcoin;

TEST(BitcoinCash, Address) {
    EXPECT_TRUE(TWAnyAddressIsValid(STRING("pqx578nanz2h2estzmkr53zqdg6qt8xyqvwhn6qeyc").get(), TWCoinTypeBitcoinCash));
}

TEST(BitcoinCash, ValidAddress) {
    auto string = STRING("bitcoincash:qqa2qx0d8tegw32xk8u75ws055en4x3h2u0e6k46y4");
    auto address = WRAP(TWAnyAddress, TWAnyAddressCreateWithString(string.get(), TWCoinTypeBitcoinCash));
    ASSERT_NE(address.get(), nullptr);
    
    auto script = WRAP(TWBitcoinScript, TWBitcoinScriptBuildForAddress(string.get(), TWCoinTypeBitcoinCash));
    ASSERT_FALSE(TWBitcoinScriptSize(script.get()) == 0);
}

TEST(BitcoinCash, LegacyToCashAddr) {
    auto privateKey = WRAP(TWPrivateKey, TWPrivateKeyCreateWithData(DATA("28071bf4e2b0340db41b807ed8a5514139e5d6427ff9d58dbd22b7ed187103a4").get()));
    auto publicKey = TWPrivateKeyGetPublicKeySecp256k1(privateKey.get(), true);
    auto address = TWBitcoinAddressCreateWithPublicKey(publicKey, 0);
    auto addressString = WRAPS(TWBitcoinAddressDescription(address));
    assertStringsEqual(addressString, "1PeUvjuxyf31aJKX6kCXuaqxhmG78ZUdL1");

    auto cashAddress = WRAP(TWAnyAddress, TWAnyAddressCreateWithPublicKey(publicKey, TWCoinTypeBitcoinCash));
    auto cashAddressString = WRAPS(TWAnyAddressDescription(cashAddress.get()));
    assertStringsEqual(cashAddressString, "bitcoincash:qruxj7zq6yzpdx8dld0e9hfvt7u47zrw9gfr5hy0vh");
}

TEST(BitcoinCash, LockScript) {
    auto address = WRAP(TWAnyAddress, TWAnyAddressCreateWithString(STRING("bitcoincash:qpk05r5kcd8uuzwqunn8rlx5xvuvzjqju5rch3tc0u").get(), TWCoinTypeBitcoinCash));
    auto data = TWAnyAddressData(address.get());
    auto rawData = TWDataCreateWithSize(0);
    TWDataAppendByte(rawData, 0x00);
    TWDataAppendData(rawData, data);

    auto legacyAddress = TWBitcoinAddressCreateWithData(rawData);
    auto legacyString = WRAPS(TWBitcoinAddressDescription(legacyAddress));
    assertStringsEqual(legacyString, "1AwDXywmyhASpCCFWkqhySgZf8KiswFoGh");
    TWDataDelete(data);
    TWDataDelete(rawData);

    auto keyHash = WRAPD(TWDataCreateWithBytes(legacyAddress->impl.bytes.data() + 1, 20));
    auto script = WRAP(TWBitcoinScript, TWBitcoinScriptBuildPayToPublicKeyHash(keyHash.get()));
    auto scriptData = WRAPD(TWBitcoinScriptData(script.get()));
    assertHexEqual(scriptData, "76a9146cfa0e96c34fce09c0e4e671fcd43338c14812e588ac");

    auto script2 = WRAP(TWBitcoinScript, TWBitcoinScriptBuildForAddress(STRING("pzukqjmcyzrkh3gsqzdcy3e3d39cqxhl3g0f405k5l").get(), TWCoinTypeBitcoinCash));
    auto scriptData2 = WRAPD(TWBitcoinScriptData(script2.get()));
    assertHexEqual(scriptData2, "a914b9604b7820876bc510009b8247316c4b801aff8a87");
}

TEST(BitcoinCash, ExtendedKeys) {
    auto wallet = WRAP(TWHDWallet, TWHDWalletCreateWithMnemonic(
        STRING("ripple scissors kick mammal hire column oak again sun offer wealth tomorrow wagon turn fatal").get(),
        STRING("TREZOR").get()
    ));

    auto xprv = WRAPS(TWHDWalletGetExtendedPrivateKey(wallet.get(), TWPurposeBIP44, TWCoinTypeBitcoinCash, TWHDVersionXPRV));
    auto xpub = WRAPS(TWHDWalletGetExtendedPublicKey(wallet.get(), TWPurposeBIP44, TWCoinTypeBitcoinCash, TWHDVersionXPUB));

    assertStringsEqual(xprv, "xprv9yEvwSfPanK5gLYVnYvNyF2CEWJx1RsktQtKDeT6jnCnqASBiPCvFYHFSApXv39bZbF6hRaha1kWQBVhN1xjo7NHuhAn5uUfzy79TBuGiHh");
    assertStringsEqual(xpub, "xpub6CEHLxCHR9sNtpcxtaTPLNxvnY9SQtbcFdov22riJ7jmhxmLFvXAoLbjHSzwXwNNuxC1jUP6tsHzFV9rhW9YKELfmR9pJaKFaM8C3zMPgjw");
}

TEST(BitcoinCash, DeriveFromXPub) {
    auto xpub = STRING("xpub6CEHLxCHR9sNtpcxtaTPLNxvnY9SQtbcFdov22riJ7jmhxmLFvXAoLbjHSzwXwNNuxC1jUP6tsHzFV9rhW9YKELfmR9pJaKFaM8C3zMPgjw");
    auto pubKey2 = TWHDWalletGetPublicKeyFromExtended(xpub.get(), STRING("m/44'/145'/0'/0/2").get());
    auto pubKey9 = TWHDWalletGetPublicKeyFromExtended(xpub.get(), STRING("m/44'/145'/0'/0/9").get());

    auto address2 = WRAP(TWAnyAddress, TWAnyAddressCreateWithPublicKey(pubKey2, TWCoinTypeBitcoinCash));
    auto address2String = WRAPS(TWAnyAddressDescription(address2.get()));

    auto address9 = WRAP(TWAnyAddress, TWAnyAddressCreateWithPublicKey(pubKey9, TWCoinTypeBitcoinCash));
    auto address9String = WRAPS(TWAnyAddressDescription(address9.get()));

    assertStringsEqual(address2String, "bitcoincash:qq4cm0hcc4trsj98v425f4ackdq7h92rsy6zzstrgy");
    assertStringsEqual(address9String, "bitcoincash:qqyqupaugd7mycyr87j899u02exc6t2tcg9frrqnve");
}

TEST(BitcoinCash, SignTransaction) {
    const int64_t amount = 600;

    // Transaction on Bitcoin Cash Mainnet
    // https://blockchair.com/bitcoin-cash/transaction/96ee20002b34e468f9d3c5ee54f6a8ddaa61c118889c4f35395c2cd93ba5bbb4

    auto input = Proto::SigningInput();
    input.set_hash_type(TWBitcoinSigHashTypeFork | TWBitcoinSigHashTypeAll);
    input.set_amount(amount);
    input.set_byte_fee(1);
    input.set_to_address("1Bp9U1ogV3A14FMvKbRJms7ctyso4Z4Tcx");
    input.set_change_address("1FQc5LdgGHMHEN9nwkjmz6tWkxhPpxBvBU");

    auto hash0 = DATA("e28c2b955293159898e34c6840d99bf4d390e2ee1c6f606939f18ee1e2000d05");
    auto utxo0 = input.add_utxo();
    utxo0->mutable_out_point()->set_hash(TWDataBytes(hash0.get()), TWDataSize(hash0.get()));
    utxo0->mutable_out_point()->set_index(2);
    utxo0->mutable_out_point()->set_sequence(UINT32_MAX);
    utxo0->set_amount(5151);
    auto script0 = parse_hex("76a914aff1e0789e5fe316b729577665aa0a04d5b0f8c788ac");
    utxo0->set_script(script0.data(), script0.size());

    auto utxoKey0 = DATA("7fdafb9db5bc501f2096e7d13d331dc7a75d9594af3d251313ba8b6200f4e384");
    input.add_private_key(TWDataBytes(utxoKey0.get()), TWDataSize(utxoKey0.get()));

    // Sign
    Proto::SigningOutput output;
    ANY_SIGN(input, TWCoinTypeBitcoinCash);

    ASSERT_EQ(output.encoded().length(), 226);
    ASSERT_EQ(hex(output.encoded()),
        "01000000"
        "01"
            "e28c2b955293159898e34c6840d99bf4d390e2ee1c6f606939f18ee1e2000d05" "02000000" "6b483045022100ec09291f956567ac3fbf632710a44e4bdf5805d2167869e7d84cd95032e1af9c022019e912584e78dd62eeb5a72d0fabce18c8b4d6a475b8319ac13aaa429c0af8944121038eab72ec78e639d02758e7860cdec018b49498c307791f785aa3019622f4ea5b" "ffffffff"
        "02"
            // amount 600
            "5802000000000000" "1976a914769bdff96a02f9135a1d19b749db6a78fe07dc9088ac"
            // change 4324; available 5151; fee 227
            "e410000000000000" "1976a9149e089b6889e032d46e3b915a3392edfd616fb1c488ac"
        "00000000");
}
