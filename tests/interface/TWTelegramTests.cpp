// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <TrustWalletCore/TWTelegramAddress.h>
#include <TrustWalletCore/TWString.h>
#include <TrustWalletCore/TWPublicKey.h>
#include <TrustWalletCore/TWHDWallet.h>

#include "TWTestUtilities.h"

#include <gtest/gtest.h>

TEST(TWTelegramAddress, CreateWithString) {
    auto addrStr = TWStringCreateWithUTF8Bytes("Ef+BVndbeTJeXWLnQtm5bDC2UVpc0vH2TF2ksZPAPwcODSkb");

    // first call isValid
    bool isValid = TWTelegramAddressIsValidString(addrStr);
    ASSERT_TRUE(isValid);

    // create address
    auto address = TWTelegramAddressCreateWithString(addrStr);
    // convert back to string
    auto str2 = TWTelegramAddressDescription(address);
    ASSERT_EQ(std::string(TWStringUTF8Bytes(addrStr)), std::string(TWStringUTF8Bytes(str2)));

    {
        // create a second one, also invoke compare
        auto address2 = TWTelegramAddressCreateWithString(TWStringCreateWithUTF8Bytes("Ef-BVndbeTJeXWLnQtm5bDC2UVpc0vH2TF2ksZPAPwcODSkb"));
        ASSERT_TRUE(TWTelegramAddressEqual(address, address2));

        TWTelegramAddressDelete(address2);
    }

    TWTelegramAddressDelete(address);
}

TEST(TWTelegramAddress, CreateWithPublicKey) {
    auto pkData = DATA("F61CF0BC8E891AD7636E0CD35229D579323AA2DA827EB85D8071407464DC2FA3");
    //auto publicKey = WRAP(TWPublicKey, TWPublicKeyCreateWithData(pkData.get(), TWPublicKeyTypeED25519));
    auto publicKey = TWPublicKeyCreateWithData(pkData.get(), TWPublicKeyTypeED25519);
    auto address = TWTelegramAddressCreateWithPublicKey(publicKey);
    auto addressStr = TWTelegramAddressDescription(address);
    ASSERT_EQ(std::string("Ef9gwEFBxqe5bWhhXnqR0mWtDzqaki6a6ckB1PqD9dPA0KTM"), TWStringUTF8Bytes(addressStr));
}

TEST(TWTelegramAddress, HDWallet) {
    auto mnemonic = "shoot island position soft burden budget tooth cruel issue economy destroy above";
    auto passphrase = "";

    auto wallet = WRAP(TWHDWallet, TWHDWalletCreateWithMnemonic(STRING(mnemonic).get(), STRING(passphrase).get()));

    auto privateKey = TWHDWalletGetKey(wallet.get(), TWCoinTypeDerivationPath(TWCoinTypeSolana));
    auto publicKey = TWPrivateKeyGetPublicKeyEd25519(privateKey);
    auto address = TWTelegramAddressCreateWithPublicKey(publicKey);
    auto addressStr = WRAPS(TWTelegramAddressDescription(address));

    assertStringsEqual(addressStr, "Ef/ILPbxb92d5s9xvpznI7YLyh7kloPJdj1dEFTmHwrX97Hf");
}
