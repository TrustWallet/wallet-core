// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

import WalletCore
import XCTest

class GroestlcoinTests: XCTestCase {
    func testAddress() {
        let privateKey1 = PrivateKey(data: Data(hexString: "3c3385ddc6fd95ba7282051aeb440bc75820b8c10db5c83c052d7586e3e98e84")!)!
        let publicKey1 = privateKey1.getPublicKeySecp256k1(compressed: true)
        let legacyAddress = GroestlcoinAddress(publicKey: publicKey1, prefix: CoinType.groestlcoin.p2pkhPrefix)
        XCTAssertEqual(GroestlcoinAddress(string: "Fj62rBJi8LvbmWu2jzkaUX1NFXLEqDLoZM")!.description, legacyAddress.description)

        let privateKey2 = PrivateKey(data: Data(hexString: "8c59c0a6f433a961109d4fd485c4562f87e0f1ad0ece32e1db406a84c5028391")!)!
        let publicKey2 = privateKey2.getPublicKeySecp256k1(compressed: true)
        let bech32Address = SegwitAddress(hrp: .groestlcoin, publicKey: publicKey2)
        XCTAssertEqual(SegwitAddress(string: "grs1qsjpmsmm4x34wlt6kk4zef9u0jtculguktwgwg4")!.description, bech32Address.description)
    }

    func testGroestlcoinBlockchain() {
        let chain = CoinType.groestlcoin
        XCTAssertTrue(chain.validate(address:  "Fj62rBJi8LvbmWu2jzkaUX1NFXLEqDLoZM"))
        XCTAssertFalse(chain.validate(address: "Fj62rBJi8LvbmWu2jzkaUX1NFXLEsNpjgw")) // sha256d checksum instead of groestl512d
        XCTAssertFalse(chain.validate(address: "mvbu1Gdy8SUjTenqerxUaZyYjmvedc787y")) // valid checksum, but testnet prefix
        XCTAssertTrue(chain.validate(address:  "31inaRqambLsd9D7Ke4USZmGEVd3PHkh7P"))
        XCTAssertFalse(chain.validate(address: "31inaRqambLsd9D7Ke4USZmGEVd3LVt8yd")) // sha256d checksum instead of groestl512d
        XCTAssertFalse(chain.validate(address: "2N4Q5FhU2497BryFfUgbqkAJE87aKDv3V3e"))// valid checksum, but testnet prefix
        XCTAssertTrue(chain.validate(address:  "grs1qw4teyraux2s77nhjdwh9ar8rl9dt7zww8r6lne"))
        XCTAssertFalse(chain.validate(address: "bc1qhkfq3zahaqkkzx5mjnamwjsfpq2jk7z00ppggv"))
    }

    func testExtendedKeys() {
        let wallet = HDWallet(mnemonic: "all all all all all all all all all all all all", passphrase: "")!

        // .bip44
        let xprv = wallet.getExtendedPrivateKey(purpose: .bip44, coin: .groestlcoin, version: .xprv)
        let xpub = wallet.getExtendedPublicKey(purpose: .bip44, coin: .groestlcoin, version: .xpub)

        XCTAssertEqual(xprv, "xprv9zHDfLCJPTf5UrS16CrJ56WzSSoAYhJriX8Lfsco3TtPhG2DkwkVXjaDxZKU5USfmq5xjp1CZhpSrpHAPFwZWN75egb19TxWmMMmkd3csxP")
        XCTAssertEqual(xpub, "xpub6DGa4qjCDqDNhLWUCEPJSETizUdexA2i5k3wUG2QboRNa4MNJV4k5XthorGcogStY5K5iJ6NHtsznNK599ir8PmA3d1jqEoZHsixDTddNA9")

        // .bip49
        let yprv = wallet.getExtendedPrivateKey(purpose: .bip49, coin: .groestlcoin, version: .yprv)
        let ypub = wallet.getExtendedPublicKey(purpose: .bip49, coin: .groestlcoin, version: .ypub)
        XCTAssertEqual(yprv, "yprvAJkRD9AD6QrU1hvSdcJT1Cdc1DwEMsBHFt4Gqd5NsK8Vhdn3ArEHYGaJhWotcn24VWx9rC6dDutHNea9zws8owL1qWEt3pVD2GGk4DSXyvm")
        XCTAssertEqual(ypub, "ypub6Xjmceh6vnQmEBzujdqTNLaLZFmimKu8d6yse1UzRefUaS7BiPYY64tnYpQQydp1gnb2cGkccBd1RtHRDtGXagqmRLxTStV88GWaeYh8ndG")

        // .bip84
        let zprv = wallet.getExtendedPrivateKey(purpose: .bip84, coin: .groestlcoin, version: .zprv)
        let zpub = wallet.getExtendedPublicKey(purpose: .bip84, coin: .groestlcoin, version: .zpub)
        XCTAssertEqual(zprv, "zprvAcXuP1BeFt59rhLMnqTEL9j2TUz5mzXkj8NPcfvLKGzHm5mofJAeJMvFzzbNizahKxVEvptBpSxdhBcGbxdbaFP58caWLWAjZWMT7Jb6pFW")
        XCTAssertEqual(zpub, "zpub6qXFnWiY6FdT5BQptrzEhHfm1WpaBTFc6MHzR4KwscXGdt6xCqUtrAEjrHdeEsjaYEwVMgjtTvENQ83yo2fmkYYGjTpJoH7vFWKQJp1bg1X")
    }

    func testDeriveFromZPub() {
        let zpub = "zpub6qXFnWiY6FdT5BQptrzEhHfm1WpaBTFc6MHzR4KwscXGdt6xCqUtrAEjrHdeEsjaYEwVMgjtTvENQ83yo2fmkYYGjTpJoH7vFWKQJp1bg1X"
        let groestlcoin = CoinType.groestlcoin
        let zpubAddr4 = HDWallet.getPublicKeyFromExtended(
            extended: zpub,
            coin: groestlcoin,
            derivationPath: DerivationPath(purpose: groestlcoin.purpose, coin: groestlcoin.slip44Id, account: 0, change: 0, address: 4).description
        )!
        let zpubAddr11 = HDWallet.getPublicKeyFromExtended(
            extended: zpub,
            coin: groestlcoin,
            derivationPath: DerivationPath(purpose: groestlcoin.purpose, coin: groestlcoin.slip44Id, account: 0, change: 0, address: 11).description
        )!

        XCTAssertEqual(SegwitAddress(hrp: .groestlcoin, publicKey: zpubAddr4).description, "grs1quwq6ml2r8rc25tue5ltfa6uc4pdzhtzul3c0rk")
        XCTAssertEqual(SegwitAddress(hrp: .groestlcoin, publicKey: zpubAddr11).description, "grs1ql0a7czm8wrj253h78dm2h5j2k89zwpy2qjq0q9")
    }
}
