// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <TrustWalletCore/TWAnyAddress.h>
#include "HexCoding.h"

#include "../interface/TWTestUtilities.h"
#include <gtest/gtest.h>

using namespace TW;

TEST(TWAvalanche, Address) {
    auto privateKey = WRAP(TWPrivateKey, TWPrivateKeyCreateWithData(DATA("0x98cb077f972feb0481f1d894f272c6a1e3c15e272a1658ff716444f465200070").get()));
    auto publicKey = WRAP(TWPublicKey, TWPrivateKeyGetPublicKeySecp256k1(privateKey.get(), false));
    auto string = "X-avax1apmh7wxg3js48fhacfv5y9md9065jxuft30vup";
    
    auto address = WRAP(TWAnyAddress, TWAnyAddressCreateWithPublicKey(publicKey.get(), TWCoinTypeAvalancheXChain));
    auto expected = WRAP(TWAnyAddress, TWAnyAddressCreateWithString(STRING(string).get(), TWCoinTypeAvalancheXChain));

    auto addressString = WRAPS(TWAnyAddressDescription(address.get()));
    auto expectedString = WRAPS(TWAnyAddressDescription(expected.get()));

    assertStringsEqual(addressString, string);
    assertStringsEqual(expectedString, string);
}
