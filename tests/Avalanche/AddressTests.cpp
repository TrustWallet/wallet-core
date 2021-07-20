// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "HexCoding.h"
#include "Avalanche/Address.h"
#include "PublicKey.h"
#include "PrivateKey.h"
#include "TrustWalletCore/TWHRP.h"
#include <gtest/gtest.h>
#include <vector>

using namespace TW;
using namespace TW::Avalanche;

TEST(AvalancheAddress, Valid) {
    // three known-valid addresses from genesis
    ASSERT_TRUE(Address::isValid("X-avax1wrgqjed292adreyal364nmz4jfjq2x5qkj7ack"));
    ASSERT_TRUE(Address::isValid("X-avax1922rpmutaj68chc2k08sjq2ah7k96dvl0ukw3k"));
    ASSERT_TRUE(Address::isValid("X-avax1hqem6nf0mp3v4gz7l3wsz2en9rp9c2drk0v742"));
    ASSERT_TRUE(Address::isValid("P-avax18lasc0q97jlurnnxuyyqyz0rm6tvl0ecsvkvjj")); // can't make a P-chain address, but this address is a valid Avalanche address, so return true
}

TEST(AvalancheAddress, Invalid) {
    ASSERT_FALSE(Address::isValid("")); // Empty
    ASSERT_FALSE(Address::isValid("X-fuji10feexxuhr3z8vh3wvzg2nhwqkllwpnhs55xum5")); // Testnet not supported
    ASSERT_FALSE(Address::isValid("-fuji10feexxuhr3z8vh3wvzg2nhwqkllwpnhs55xum5")); // Must prepend with chain designator
    ASSERT_FALSE(Address::isValid("Z-fuji10feexxuhr3z8vh3wvzg2nhwqkllwpnhs55xum5")); // ...and that chain designator must be X or P
    ASSERT_FALSE(Address::isValid("0x1641303f4c3105e8ba980b271d52cafdb4e5f01e")); // C chain not supported
    ASSERT_FALSE(Address::isValid("X-custom1hqem6nf0mp3v4gz7l3wsz2en9rp9c2drk0v742")); // non-avax x-chain assets not supported
    ASSERT_FALSE(Address::isValid("X-avax1hqem6nfmp3v4gz7l3wsz2enrp9c2drk0v712")); // corrupted good value
    ASSERT_ANY_THROW(Address("X-fuji10feexxuhr3z8vh3wvzg2nhwqkllwpnhs55xum5")); // building an invalid address should throw
}

TEST(AvalancheAddress, FromPrivateKey) {
    auto privateKey = PrivateKey(parse_hex("0x98cb077f972feb0481f1d894f272c6a1e3c15e272a1658ff716444f465200070"));
    auto address = Address(privateKey.getPublicKey(TWPublicKeyTypeSECP256k1));
    ASSERT_EQ(address.string(), "X-avax1apmh7wxg3js48fhacfv5y9md9065jxuft30vup");
}

TEST(AvalancheAddress, FromPublicKey) {
    auto publicKey = PublicKey(parse_hex("0x02b33c917f2f6103448d7feb42614037d05928433cb25e78f01a825aa829bb3c27"), TWPublicKeyTypeSECP256k1);
    auto address = Address(publicKey);
    ASSERT_EQ(address.string(), "X-avax1apmh7wxg3js48fhacfv5y9md9065jxuft30vup");
}

TEST(AvalancheAddress, FromString) {
    auto address = Address("X-avax1apmh7wxg3js48fhacfv5y9md9065jxuft30vup");
    ASSERT_EQ(address.string(), "X-avax1apmh7wxg3js48fhacfv5y9md9065jxuft30vup");
}

TEST(AvalancheAddress, FromKeyHash) {
    auto address = Address("X-avax1apmh7wxg3js48fhacfv5y9md9065jxuft30vup");
    auto addressFromKeyHash = Address(address.getKeyHash());
    ASSERT_EQ(addressFromKeyHash.string(), address.string());
}

TEST(AvalancheAddress, AssignmentAndOperators) {
    auto emptyAddress = Address();
    auto baseline = Address("X-avax1apmh7wxg3js48fhacfv5y9md9065jxuft30vup");
    ASSERT_TRUE(baseline != emptyAddress);
    auto strippedString = "avax1apmh7wxg3js48fhacfv5y9md9065jxuft30vup";
    auto success = Bech32Address::decode(strippedString, emptyAddress, HRP_AVALANCHEX, true);
    ASSERT_TRUE(success);
    ASSERT_EQ("X-avax1apmh7wxg3js48fhacfv5y9md9065jxuft30vup", emptyAddress.string());
}
