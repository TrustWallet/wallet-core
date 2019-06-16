// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "TWTestUtilities.h"

#include <TrustWalletCore/TWNebulasAddress.h>
#include <TrustWalletCore/TWHash.h>
#include <TrustWalletCore/TWHDWallet.h>
#include <TrustWalletCore/TWHRP.h>
#include <TrustWalletCore/TWP2PKHPrefix.h>
#include <TrustWalletCore/TWP2SHPrefix.h>
#include <TrustWalletCore/TWPrivateKey.h>

#include "../../src/HexCoding.h"
#include <gtest/gtest.h>

TEST(Nebulas, Address) {
    auto privateKey = WRAP(TWPrivateKey, TWPrivateKeyCreateWithData(DATA("d2fd0ec9f6268fc8d1f563e3e976436936708bdf0dc60c66f35890f5967a8d2b").get()));
    auto publicKey = TWPrivateKeyGetPublicKeySecp256k1(privateKey.get(), false);
    auto address = TWNebulasAddressCreateWithPublicKey(publicKey);
    auto addressString = WRAPS(TWNebulasAddressDescription(address));
    assertStringsEqual(addressString, "n1V5bB2tbaM3FUiL4eRwpBLgEredS5C2wLY");
}

TEST(Nebulas, ExtendedKeys) {
    auto wallet = WRAP(TWHDWallet, TWHDWalletCreateWithMnemonic(
        STRING("ripple scissors kick mammal hire column oak again sun offer wealth tomorrow wagon turn fatal").get(),
        STRING("TREZOR").get()
    ));

    auto xpub = WRAPS(TWHDWalletGetExtendedPublicKey(wallet.get(), TWPurposeBIP44, TWCoinTypeNebulas, TWHDVersionXPUB));
    auto xprv = WRAPS(TWHDWalletGetExtendedPrivateKey(wallet.get(), TWPurposeBIP44, TWCoinTypeNebulas, TWHDVersionXPRV));

    // generate by https://iancoleman.io/bip39/#english
    assertStringsEqual(xpub, "xpub6BfZdeDKShWRToXqkSHsN3DN39LG59WgR4SWQoJRGLejQbfjGpsqNLgTNKkda47ykF3kpZ7ktEKPJfBR65QqBzrRfdEKPbhxQ7kxXaFWARU");
    assertStringsEqual(xprv, "xprv9xgDE8gRcKx8FKTNeQkrzuGdV7Vmfgnq3qWucQtoi17kXoLajHZapYMyX5Gg1qQ3isvekWzUJ7oTaD6U9hCYjz7mVYpRohvsgmwH8SRV7t2");
}
