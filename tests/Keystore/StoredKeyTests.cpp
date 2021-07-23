// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Keystore/StoredKey.h"

#include "Coin.h"
#include "HexCoding.h"
#include "Data.h"
#include "PrivateKey.h"
#include "Mnemonic.h"

#include <stdexcept>
#include <gtest/gtest.h>

extern std::string TESTS_ROOT;

namespace TW::Keystore {

using namespace std;

const auto passwordString = "password";
const auto password = TW::data(string(passwordString));
const auto mnemonic = "team engine square letter hero song dizzy scrub tornado fabric divert saddle";
const TWCoinType coinTypeBc = TWCoinTypeBitcoin;
const TWCoinType coinTypeBnb = TWCoinTypeBinance;
const TWCoinType coinTypeBsc = TWCoinTypeSmartChain;
const TWCoinType coinTypeEth = TWCoinTypeEthereum;
const TWCoinType coinTypeBscLegacy = TWCoinTypeSmartChainLegacy;

TEST(StoredKey, CreateWithMnemonic) {
    auto key = StoredKey::createWithMnemonic("name", password, mnemonic);
    EXPECT_EQ(key.type, StoredKeyType::mnemonicPhrase);
    const Data& mnemo2Data = key.payload.decrypt(password);
    EXPECT_EQ(string(mnemo2Data.begin(), mnemo2Data.end()), string(mnemonic));
    EXPECT_EQ(key.accounts.size(), 0);
    EXPECT_EQ(key.wallet(password).mnemonic, string(mnemonic));

    const auto json = key.json();
    EXPECT_EQ(json["name"], "name");
    EXPECT_EQ(json["type"], "mnemonic");
    EXPECT_EQ(json["version"], 3);
}

TEST(StoredKey, CreateWithMnemonicInvalid) {
    try {
        auto key = StoredKey::createWithMnemonic("name", password, "_THIS_IS_NOT_A_VALID_MNEMONIC_");
    } catch (std::invalid_argument&) {
        // expedcted exception OK
        return;
    }
    FAIL() << "Missing excpected excpetion";
}

TEST(StoredKey, CreateWithMnemonicRandom) {
    const auto key = StoredKey::createWithMnemonicRandom("name", password);
    EXPECT_EQ(key.type, StoredKeyType::mnemonicPhrase);
    // random mnemonic: check only length and validity
    const Data& mnemo2Data = key.payload.decrypt(password);
    EXPECT_TRUE(mnemo2Data.size() >= 36);
    EXPECT_TRUE(Mnemonic::isValid(string(mnemo2Data.begin(), mnemo2Data.end())));
    EXPECT_EQ(key.accounts.size(), 0);
}

TEST(StoredKey, CreateWithMnemonicAddDefaultAddress) {
    auto key = StoredKey::createWithMnemonicAddDefaultAddress("name", password, mnemonic, coinTypeBc);
    EXPECT_EQ(key.type, StoredKeyType::mnemonicPhrase);
    const Data& mnemo2Data = key.payload.decrypt(password);
    EXPECT_EQ(string(mnemo2Data.begin(), mnemo2Data.end()), string(mnemonic));
    EXPECT_EQ(key.accounts.size(), 1);
    EXPECT_EQ(key.accounts[0].coin, coinTypeBc);
    EXPECT_EQ(key.accounts[0].address, "bc1qturc268v0f2srjh4r2zu4t6zk4gdutqd5a6zny");
    EXPECT_EQ(hex(key.privateKey(coinTypeBc, password).bytes), "d2568511baea8dc347f14c4e0479eb8ebe29eb5f664ed796e755896250ffd11f");
}

TEST(StoredKey, CreateWithPrivateKeyAddDefaultAddress) {
    const auto privateKey = parse_hex("3a1076bf45ab87712ad64ccb3b10217737f7faacbf2872e88fdd9a537d8fe266");
    auto key = StoredKey::createWithPrivateKeyAddDefaultAddress("name", password, coinTypeBc, privateKey);
    EXPECT_EQ(key.type, StoredKeyType::privateKey);
    EXPECT_EQ(key.accounts.size(), 1);
    EXPECT_EQ(key.accounts[0].coin, coinTypeBc);
    EXPECT_EQ(key.accounts[0].address, "bc1q375sq4kl2nv0mlmup3vm8znn4eqwu7mt6hkwhr");
    EXPECT_EQ(hex(key.privateKey(coinTypeBc, password).bytes), hex(privateKey));

    const auto json = key.json();
    EXPECT_EQ(json["name"], "name");
    EXPECT_EQ(json["type"], "private-key");
    EXPECT_EQ(json["version"], 3);
}

TEST(StoredKey, CreateWithPrivateKeyAddDefaultAddressInvalid) {
    try {
        const auto privateKeyInvalid = parse_hex("0001020304");
        auto key = StoredKey::createWithPrivateKeyAddDefaultAddress("name", password, coinTypeBc, privateKeyInvalid);
    } catch (std::invalid_argument&) {
        // expected exception ok
        return;
    }
    FAIL() << "Missing expected exception";
}

TEST(StoredKey, AccountGetCreate) {
    auto key = StoredKey::createWithMnemonic("name", password, mnemonic);
    EXPECT_EQ(key.accounts.size(), 0);

    // not exists
    EXPECT_FALSE(key.account(coinTypeBc).has_value());
    EXPECT_EQ(key.accounts.size(), 0);

    auto wallet = key.wallet(password);
    // not exists, wallet null, not create
    EXPECT_FALSE(key.account(coinTypeBc, nullptr).has_value());
    EXPECT_EQ(key.accounts.size(), 0);

    // not exists, wallet nonnull, create
    std::optional<Account> acc3 = key.account(coinTypeBc, &wallet);
    EXPECT_TRUE(acc3.has_value());
    EXPECT_EQ(acc3->coin, coinTypeBc); 
    EXPECT_EQ(key.accounts.size(), 1);

    // exists
    std::optional<Account> acc4 = key.account(coinTypeBc);
    EXPECT_TRUE(acc4.has_value());
    EXPECT_EQ(acc4->coin, coinTypeBc); 
    EXPECT_EQ(key.accounts.size(), 1);

    // exists, wallet nonnull, not create
    std::optional<Account> acc5 = key.account(coinTypeBc, &wallet);
    EXPECT_TRUE(acc5.has_value());
    EXPECT_EQ(acc5->coin, coinTypeBc); 
    EXPECT_EQ(key.accounts.size(), 1);

    // exists, wallet null, not create
    std::optional<Account> acc6 = key.account(coinTypeBc, nullptr);
    EXPECT_TRUE(acc6.has_value());
    EXPECT_EQ(acc6->coin, coinTypeBc); 
    EXPECT_EQ(key.accounts.size(), 1);
}

TEST(StoredKey, AccountGetDoesntChange) {
    auto key = StoredKey::createWithMnemonic("name", password, mnemonic);
    auto wallet = key.wallet(password);
    EXPECT_EQ(key.accounts.size(), 0);

    vector<TWCoinType> coins = {coinTypeBc, coinTypeEth, coinTypeBnb};
    // retrieve multiple accounts, which will be created
    vector<Account> accounts;
    for (auto coin: coins) {
        std::optional<Account> account = key.account(coin, &wallet);
        accounts.push_back(*account);

        // check
        ASSERT_TRUE(account.has_value());
        EXPECT_EQ(account->coin, coin);
    }

    // Check again; make sure returned references don't change
    for (auto i = 0; i < accounts.size(); ++i) {
        // check
        EXPECT_EQ(accounts[i].coin, coins[i]);
    }
}

TEST(StoredKey, AddRemoveAccount) {
    auto key = StoredKey::createWithMnemonic("name", password, mnemonic);
    EXPECT_EQ(key.accounts.size(), 0);

    {
        const auto derivationPath = DerivationPath("m/84'/0'/0'/0/0");
        key.addAccount("bc1qaucw06s3agez8tyyk4zj9kt0q2934e3mcewdpf", coinTypeBc, derivationPath, "zpub6rxtad3SPT1C5GUDjPiKQ5oJN5DBeMbdUR7LrdYt12VbU7TBSpGUkdLvfVYGuj1N5edkDoZ3bu1fdN1HprQYfCBdsSH5CaAAygHGsanwtTe");
        EXPECT_EQ(key.accounts.size(), 1);
    }
    {
        const auto derivationPath = DerivationPath("m/714'/0'/0'/0/0");
        key.addAccount("bnb1utrnnjym7ustgw7pgyvtmnxay4qmt3ahh276nu", coinTypeBnb, derivationPath, "");
        key.addAccount("0x23b02dC8f67eD6cF8DCa47935791954286ffe7c9", coinTypeBsc, derivationPath, "");
        EXPECT_EQ(key.accounts.size(), 3);
    }
    {
        const auto derivationPath = DerivationPath("m/60'/0'/0'/0/0");
        key.addAccount("0xC0d97f61A84A0708225F15d54978D628Fe2C5E62", coinTypeEth, derivationPath, "");
        key.addAccount("0xC0d97f61A84A0708225F15d54978D628Fe2C5E62", coinTypeBscLegacy, derivationPath, "");
        EXPECT_EQ(key.accounts.size(), 5);
    }

    key.removeAccount(coinTypeBc);
    key.removeAccount(coinTypeBnb);
    key.removeAccount(coinTypeBsc);
    key.removeAccount(coinTypeEth);
    key.removeAccount(coinTypeBscLegacy);
    EXPECT_EQ(key.accounts.size(), 0);
}

TEST(StoredKey, FixAddress) {
    {
        auto key = StoredKey::createWithMnemonic("name", password, mnemonic);
        key.fixAddresses(password);
    }
    {
        const auto privateKey = parse_hex("3a1076bf45ab87712ad64ccb3b10217737f7faacbf2872e88fdd9a537d8fe266");
        auto key = StoredKey::createWithPrivateKeyAddDefaultAddress("name", password, coinTypeBc, privateKey);
        key.fixAddresses(password);
    }
}

TEST(StoredKey, WalletInvalid) {
    const auto privateKey = parse_hex("3a1076bf45ab87712ad64ccb3b10217737f7faacbf2872e88fdd9a537d8fe266");
    auto key = StoredKey::createWithPrivateKeyAddDefaultAddress("name", password, coinTypeBc, privateKey);
    try {
        auto wallet = key.wallet(password);
    } catch (std::invalid_argument&) {
        // expected exception ok
        return;
    }
    FAIL() << "Missing expected exception";
}

TEST(StoredKey, LoadNonexistent) {
    ASSERT_THROW(StoredKey::load(TESTS_ROOT + "/Keystore/Data/nonexistent.json"), invalid_argument);
}

TEST(StoredKey, LoadLegacyPrivateKey) {
    const auto key = StoredKey::load(TESTS_ROOT + "/Keystore/Data/legacy-private-key.json");
    EXPECT_EQ(key.type, StoredKeyType::privateKey);
    EXPECT_EQ(key.id, "3051ca7d-3d36-4a4a-acc2-09e9083732b0");
    EXPECT_EQ(key.accounts[0].coin, TWCoinTypeEthereum);
    EXPECT_EQ(hex(key.payload.decrypt(TW::data("testpassword"))), "7a28b5ba57c53603b0b07b56bba752f7784bf506fa95edc395f5cf6c7514fe9d");
}

TEST(StoredKey, LoadLivepeerKey) {
    const auto key = StoredKey::load(TESTS_ROOT + "/Keystore/Data/livepeer.json");
    EXPECT_EQ(key.type, StoredKeyType::privateKey);
    EXPECT_EQ(key.id, "70ea3601-ee21-4e94-a7e4-66255a987d22");
    EXPECT_EQ(key.accounts[0].coin, TWCoinTypeEthereum);
    EXPECT_EQ(hex(key.payload.decrypt(TW::data("Radchenko"))), "09b4379d9a41a71d94ee36357bccb4d77b45e7fd9307e2c0f673dd54c0558c73");
}

TEST(StoredKey, LoadPBKDF2Key) {
    const auto key = StoredKey::load(TESTS_ROOT + "/Keystore/Data/pbkdf2.json");
    EXPECT_EQ(key.type, StoredKeyType::privateKey);
    EXPECT_EQ(key.id, "3198bc9c-6672-5ab3-d995-4942343ae5b6");

    const auto& payload = key.payload;
    ASSERT_TRUE(payload.kdfParams.which() == 1);
    EXPECT_EQ(boost::get<PBKDF2Parameters>(payload.kdfParams).desiredKeyLength, 32);
    EXPECT_EQ(boost::get<PBKDF2Parameters>(payload.kdfParams).iterations, 262144);
    EXPECT_EQ(hex(boost::get<PBKDF2Parameters>(payload.kdfParams).salt), "ae3cd4e7013836a3df6bd7241b12db061dbe2c6785853cce422d148a624ce0bd");

    EXPECT_EQ(hex(payload.decrypt(TW::data("testpassword"))), "7a28b5ba57c53603b0b07b56bba752f7784bf506fa95edc395f5cf6c7514fe9d");
}

TEST(StoredKey, LoadLegacyMnemonic) {
    const auto key = StoredKey::load(TESTS_ROOT + "/Keystore/Data/legacy-mnemonic.json");
    EXPECT_EQ(key.type, StoredKeyType::mnemonicPhrase);
    EXPECT_EQ(key.id, "629aad29-0b22-488e-a0e7-b4219d4f311c");

    const auto data = key.payload.decrypt(password);
    const auto mnemonic = string(reinterpret_cast<const char*>(data.data()));
    EXPECT_EQ(mnemonic, "ripple scissors kick mammal hire column oak again sun offer wealth tomorrow wagon turn back");

    EXPECT_EQ(key.accounts[0].coin, TWCoinTypeEthereum);
    EXPECT_EQ(key.accounts[0].derivationPath.string(), "m/44'/60'/0'/0/0");
    EXPECT_EQ(key.accounts[0].address, "");
    EXPECT_EQ(key.accounts[1].coin, coinTypeBc);
    EXPECT_EQ(key.accounts[1].derivationPath.string(), "m/84'/0'/0'/0/0");
    EXPECT_EQ(key.accounts[1].address, "");
    EXPECT_EQ(key.accounts[1].extendedPublicKey, "zpub6r97AegwVxVbJeuDAWP5KQgX5y4Q6KyFUrsFQRn8yzSXrnmpwg1ZKHSWwECR1Kiqgr4h93WN5kdS48KC6hVFniuZHqVFXjULZZkCwurqyPn");
}

TEST(StoredKey, LoadFromWeb3j) {
    const auto key = StoredKey::load(TESTS_ROOT + "/Keystore/Data/web3j.json");
    EXPECT_EQ(key.type, StoredKeyType::privateKey);
    EXPECT_EQ(key.id, "86066d8c-8dba-4d81-afd4-934e2a2b72a2");
    const auto password = parse_hex("2d6eefbfbd4622efbfbdefbfbd516718efbfbdefbfbdefbfbdefbfbd59efbfbd30efbfbdefbfbd3a4348efbfbd2aefbfbdefbfbd49efbfbd27efbfbd0638efbfbdefbfbdefbfbd4cefbfbd6befbfbdefbfbd6defbfbdefbfbd63efbfbd5aefbfbd61262b70efbfbdefbfbdefbfbdefbfbdefbfbdc7aa373163417cefbfbdefbfbdefbfbd44efbfbdefbfbd1d10efbfbdefbfbdefbfbd61dc9e5b124befbfbd11efbfbdefbfbd2fefbfbdefbfbd3d7c574868efbfbdefbfbdefbfbd37043b7b5c1a436471592f02efbfbd18efbfbdefbfbd2befbfbdefbfbd7218efbfbd6a68efbfbdcb8e5f3328773ec48174efbfbd67efbfbdefbfbdefbfbdefbfbdefbfbd2a31efbfbd7f60efbfbdd884efbfbd57efbfbd25efbfbd590459efbfbd37efbfbd2bdca20fefbfbdefbfbdefbfbdefbfbd39450113efbfbdefbfbdefbfbd454671efbfbdefbfbdd49fefbfbd47efbfbdefbfbdefbfbdefbfbd00efbfbdefbfbdefbfbdefbfbd05203f4c17712defbfbd7bd1bbdc967902efbfbdc98a77efbfbd707a36efbfbd12efbfbdefbfbd57c78cefbfbdefbfbdefbfbd10efbfbdefbfbdefbfbde1a1bb08efbfbdefbfbd26efbfbdefbfbd58efbfbdefbfbdc4b1efbfbd295fefbfbd0eefbfbdefbfbdefbfbd0e6eefbfbd");
    const auto data = key.payload.decrypt(password);
    EXPECT_EQ(hex(data), "043c5429c7872502531708ec0d821c711691402caf37ef7ba78a8c506f10653b");
}

TEST(StoredKey, ReadWallet) {
    const auto key = StoredKey::load(TESTS_ROOT + "/Keystore/Data/key.json");

    EXPECT_EQ(key.type, StoredKeyType::privateKey);
    EXPECT_EQ(key.id, "e13b209c-3b2f-4327-bab0-3bef2e51630d");
    EXPECT_EQ(key.name, "Test Account");

    const auto header = key.payload;

    EXPECT_EQ(header.cipher, "aes-128-ctr");
    EXPECT_EQ(hex(header.encrypted), "d172bf743a674da9cdad04534d56926ef8358534d458fffccd4e6ad2fbde479c");
    EXPECT_EQ(hex(header.mac), "2103ac29920d71da29f15d75b4a16dbe95cfd7ff8faea1056c33131d846e3097");
    EXPECT_EQ(hex(header.cipherParams.iv), "83dbcc02d8ccb40e466191a123791e0e");

    ASSERT_TRUE(header.kdfParams.which() == 0);
    EXPECT_EQ(boost::get<ScryptParameters>(header.kdfParams).desiredKeyLength, 32);
    EXPECT_EQ(boost::get<ScryptParameters>(header.kdfParams).n, 262144);
    EXPECT_EQ(boost::get<ScryptParameters>(header.kdfParams).p, 8);
    EXPECT_EQ(boost::get<ScryptParameters>(header.kdfParams).r, 1);
    EXPECT_EQ(hex(boost::get<ScryptParameters>(header.kdfParams).salt), "ab0c7876052600dd703518d6fc3fe8984592145b591fc8fb5c6d43190334ba19");
}

TEST(StoredKey, ReadMyEtherWallet) {
    ASSERT_NO_THROW(StoredKey::load(TESTS_ROOT + "/Keystore/Data/myetherwallet.uu"));
}

TEST(StoredKey, InvalidPassword) {
    const auto key = StoredKey::load(TESTS_ROOT + "/Keystore/Data/key.json");

    ASSERT_THROW(key.payload.decrypt(password), DecryptionError);
}

TEST(StoredKey, EmptyAccounts) {
    const auto key = StoredKey::load(TESTS_ROOT + "/Keystore/Data/empty-accounts.json");

    ASSERT_NO_THROW(key.payload.decrypt(TW::data("testpassword")));
}

TEST(StoredKey, Decrypt) {
    const auto key = StoredKey::load(TESTS_ROOT + "/Keystore/Data/key.json");
    const auto privateKey = key.payload.decrypt(TW::data("testpassword"));

    EXPECT_EQ(hex(privateKey), "7a28b5ba57c53603b0b07b56bba752f7784bf506fa95edc395f5cf6c7514fe9d");
}

TEST(StoredKey, CreateWallet) {
    const auto privateKey = parse_hex("3a1076bf45ab87712ad64ccb3b10217737f7faacbf2872e88fdd9a537d8fe266");
    const auto key = StoredKey::createWithPrivateKey("name", password, privateKey);
    const auto decrypted = key.payload.decrypt(password);

    EXPECT_EQ(hex(decrypted), hex(privateKey));
}

TEST(StoredKey, CreateAccounts) {
    string mnemonicPhrase = "team engine square letter hero song dizzy scrub tornado fabric divert saddle";
    auto key = StoredKey::createWithMnemonic("name", password, mnemonicPhrase);
    const auto wallet = key.wallet(password);
    
    EXPECT_EQ(key.account(TWCoinTypeEthereum, &wallet)->address, "0x494f60cb6Ac2c8F5E1393aD9FdBdF4Ad589507F7");
    EXPECT_EQ(key.account(TWCoinTypeEthereum, &wallet)->extendedPublicKey, "");

    EXPECT_EQ(key.account(coinTypeBc, &wallet)->address, "bc1qturc268v0f2srjh4r2zu4t6zk4gdutqd5a6zny");
    EXPECT_EQ(key.account(coinTypeBc, &wallet)->extendedPublicKey, "zpub6qbsWdbcKW9sC6shTKK4VEhfWvDCoWpfLnnVfYKHLHt31wKYUwH3aFDz4WLjZvjHZ5W4qVEyk37cRwzTbfrrT1Gnu8SgXawASnkdQ994atn");
}
    
TEST(StoredKey, DecodingEthereumAddress) {
    const auto key = StoredKey::load(TESTS_ROOT + "/Keystore/Data/key.json");

    EXPECT_EQ(key.accounts[0].address, "0x008AeEda4D805471dF9b2A5B0f38A0C3bCBA786b");
}

TEST(StoredKey, DecodingBitcoinAddress) {
    const auto key = StoredKey::load(TESTS_ROOT + "/Keystore/Data/key_bitcoin.json");

    EXPECT_EQ(key.accounts[0].address, "3PWazDi9n1Hfyq9gXFxDxzADNL8RNYyK2y");
}
    
TEST(StoredKey, RemoveAccount) {
    auto key = StoredKey::load(TESTS_ROOT + "/Keystore/Data/legacy-mnemonic.json");
    EXPECT_EQ(key.accounts.size(), 2);
    key.removeAccount(TWCoinTypeEthereum);
    EXPECT_EQ(key.accounts.size(), 1);
    EXPECT_EQ(key.accounts[0].coin, coinTypeBc);
}

TEST(StoredKey, MissingAddress) {
    auto key = StoredKey::load(TESTS_ROOT + "/Keystore/Data/missing-address.json");
    EXPECT_EQ(key.type, StoredKeyType::mnemonicPhrase);

    const auto wallet = key.wallet(password);
    EXPECT_EQ(wallet.mnemonic, "ripple scissors kick mammal hire column oak again sun offer wealth tomorrow wagon turn fatal");
    EXPECT_TRUE(Mnemonic::isValid(wallet.mnemonic));

    key.fixAddresses(password);

    EXPECT_EQ(key.account(TWCoinTypeEthereum, nullptr)->address, "0xA3Dcd899C0f3832DFDFed9479a9d828c6A4EB2A7");
    EXPECT_EQ(key.account(coinTypeBc, nullptr)->address, "bc1qpsp72plnsqe6e2dvtsetxtww2cz36ztmfxghpd");
}

TEST(StoredKey, EtherWalletAddressNo0x) {
    auto key = StoredKey::load(TESTS_ROOT + "/Keystore/Data/ethereum-wallet-address-no-0x.json");
    key.fixAddresses(TW::data("15748c4e3dca6ae2110535576ab0c398cb79d985707c68ee6c9f9df9d421dd53"));
    EXPECT_EQ(key.account(TWCoinTypeEthereum, nullptr)->address, "0xAc1ec44E4f0ca7D172B7803f6836De87Fb72b309");
}

} // namespace TW::Keystore
