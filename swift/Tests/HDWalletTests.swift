// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

import WalletCore
import XCTest

extension HDWallet {
    static let test = HDWallet(mnemonic: "ripple scissors kick mammal hire column oak again sun offer wealth tomorrow wagon turn fatal", passphrase: "TREZOR")!
}

class HDWalletTests: XCTestCase {
    func testCreateFromMnemonic() {
        let wallet = HDWallet.test

        XCTAssertEqual(wallet.mnemonic, "ripple scissors kick mammal hire column oak again sun offer wealth tomorrow wagon turn fatal")
        XCTAssertEqual(wallet.entropy.hexString, "ba5821e8c356c05ba5f025d9532fe0f21f65d594")
        XCTAssertEqual(wallet.seed.hexString, "7ae6f661157bda6492f6162701e570097fc726b6235011ea5ad09bf04986731ed4d92bc43cbdee047b60ea0dd1b1fa4274377c9bf5bd14ab1982c272d8076f29")
    }

    func testCreateFromMnemonicNoPassword() {
        let wallet = HDWallet(mnemonic: HDWallet.test.mnemonic, passphrase: "")!

        XCTAssertEqual(wallet.seed.hexString, "354c22aedb9a37407adc61f657a6f00d10ed125efa360215f36c6919abd94d6dbc193a5f9c495e21ee74118661e327e84a5f5f11fa373ec33b80897d4697557d")
    }

    func testCreateFromMnemonicInvalid() {
        let wallet = HDWallet(mnemonic: "THIS IS AN INVALID MNEMONIC", passphrase: "")
        XCTAssertNil(wallet)
    }

    func testGenerate() {
        let wallet = HDWallet(strength: 128, passphrase: "")!
        XCTAssertTrue(Mnemonic.isValid(mnemonic: wallet.mnemonic))
    }

    func testCreateFromEntropy() {
        let wallet = HDWallet(entropy: Data(hexString: "ba5821e8c356c05ba5f025d9532fe0f21f65d594")!, passphrase: "TREZOR")!
        XCTAssertEqual(wallet.mnemonic, "ripple scissors kick mammal hire column oak again sun offer wealth tomorrow wagon turn fatal")
        XCTAssertEqual(wallet.entropy.hexString, "ba5821e8c356c05ba5f025d9532fe0f21f65d594")
    }

    func testMasterKey() {
        let wallet = HDWallet(mnemonic: "tiny escape drive pupil flavor endless love walk gadget match filter luxury", passphrase: "")!
        XCTAssertEqual(wallet.seed.hexString, "d430216f5b506dfd281d6ff6e92150d205868923df00774bc301e5ffdc2f4d1ad38a602017ddea6fc7d6315345d8b9cadbd8213ed2ffce5dfc550fa918665eb8")
        let masterKey = wallet.getMasterKey(curve: Curve.secp256k1)
        XCTAssertEqual(masterKey.data.hexString, "e120fc1ef9d193a851926ebd937c3985dc2c4e642fb3d0832317884d5f18f3b3")
    }

    func testDerive() {
        let wallet = HDWallet.test

        let key0 = wallet.getDerivedKey(coin: .ethereum, account: 0, change: 0, address: 0)
        let key1 = wallet.getDerivedKey(coin: .ethereum, account: 0, change: 0, address: 1)

        XCTAssertEqual(AnyAddress(publicKey: key0.getPublicKeySecp256k1(compressed: false), coin: .ethereum).description, "0x27Ef5cDBe01777D62438AfFeb695e33fC2335979")
        XCTAssertEqual(AnyAddress(publicKey: key1.getPublicKeySecp256k1(compressed: false), coin: .ethereum).description, "0x98f5438cDE3F0Ff6E11aE47236e93481899d1C47")
    }

    func testWanchain() {
        let wanChain = CoinType.wanchain
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: wanChain)
        let address = wanChain.deriveAddress(privateKey: key)

        XCTAssertEqual(address, "0x4DDa26870B4b3fa3FbA32222159359038f588318")
    }

    func testDeriveBitcoin() {
        let bitcoin = CoinType.bitcoin
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: bitcoin)
        let address = bitcoin.deriveAddress(privateKey: key)

        XCTAssertEqual("bc1qumwjg8danv2vm29lp5swdux4r60ezptzz7ce85", address)
    }

    func testDeriveDigiByte() {
        let digibye = CoinType.digiByte
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: digibye)
        let address = digibye.deriveAddress(privateKey: key)

        XCTAssertEqual("dgb1q7qk2vvetgldgq0eeh3ytsky2380l9wuessmhe3", address)
    }

    func testDeriveEthereum() {
        let ethereum = CoinType.ethereum
        let key = HDWallet.test.getKeyForCoin(coin: ethereum)
        let address = ethereum.deriveAddress(privateKey: key)

        XCTAssertEqual("0x27Ef5cDBe01777D62438AfFeb695e33fC2335979", address)
    }

    func testDeriveLitecoin() {
        let litecoin = CoinType.litecoin
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: litecoin)
        let address = litecoin.deriveAddress(privateKey: key)

        XCTAssertEqual("ltc1qmj6hw649d7q2teduv599zv9ls9akz60gkdwnp7", address)
    }

    func testDeriveTron() {
        let tron = CoinType.tron
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: tron)
        let address = tron.deriveAddress(privateKey: key)

        XCTAssertEqual("THJrqfbBhoB1vX97da6S6nXWkafCxpyCNB", address)
    }

    func testDeriveIcon() {
        let icon = CoinType.icon
        let wallet = HDWallet.test
        let key0 = wallet.getKeyForCoin(coin: icon)
        let key1 = wallet.getDerivedKey(coin: icon, account: 0, change: 0, address: 1)
        let address0 = icon.deriveAddress(privateKey: key0)
        let address1 = icon.deriveAddress(privateKey: key1)

        XCTAssertEqual("hx78c6f744c68d48793cd64716189c181c66907b24", address0)
        XCTAssertEqual("hx92373c16531761b31a7124c94718da43db8c9d89", address1)
    }

    func testDeriveOntology() {
        let ontology = CoinType.ontology
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: ontology)
        let address = ontology.deriveAddress(privateKey: key)

        XCTAssertEqual("AH11LGtFk6VU9Z7suuM5eNpho1bAoE5Gbz", address)
    }

    func testDeriveBitcoinCash() {
        let bitcoinCash = CoinType.bitcoinCash
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: bitcoinCash)
        let address = bitcoinCash.deriveAddress(privateKey: key)

        XCTAssertEqual("bitcoincash:qqxktqe0pzf0yepvap9rf2g8zxq8t5mqx50dwpqlxl", address)
    }

    func testDeriveDash() {
        let dash = CoinType.dash
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: dash)
        let address = dash.deriveAddress(privateKey: key)

        XCTAssertEqual("XsJg3pJaoyKEf5jMYTb5wGf3TKp9W3KX5a", address)
    }

    func testDeriveZcoin() {
        let zcoin = CoinType.zcoin
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: zcoin)
        let address = zcoin.deriveAddress(privateKey: key)

        XCTAssertEqual("a5jgmKczLE7fbgBmVkDTvvAQx8pYZKL7LP", address)
    }

    func testDeriveBinanceChain() {
        let binance = CoinType.binance
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: binance)
        let address = AnyAddress(publicKey: key.getPublicKeySecp256k1(compressed: true), coin: binance)

        XCTAssertEqual("bnb1wk7kxw0qrvxe2pj9mk6ydjx0t4j9jla8pja0td", address.description)
    }

    func testDeriveZcash() {
        let zcash = CoinType.zcash
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: zcash)
        let address = zcash.deriveAddress(privateKey: key)
        XCTAssertEqual("t1RygJmrLdNGgi98gUgEJDTVaELTAYWoMBy", address)
    }

    func testDeriveRipple() {
        let ripple = CoinType.xrp
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: ripple)
        let address = ripple.deriveAddress(privateKey: key)

        XCTAssertEqual("r36yxStAh7qgTQNHTzjZvXybCTzUFhrfav", address)
    }

    func testDeriveAion() {
        let aion = CoinType.aion
        let key = HDWallet.test.getKeyForCoin(coin: aion)
        let address = aion.deriveAddress(privateKey: key)

        XCTAssertEqual("0xa0dcc9e5e3bbd6a5a092f6b4975f6c5856e8eb750f37b7079bf7888e8cc1deb8", address)
    }

    func testDevieStellar() {
        let stellar = CoinType.stellar
        let key = HDWallet.test.getKeyForCoin(coin: stellar)
        let address = stellar.deriveAddress(privateKey: key)

        XCTAssertEqual(key.data.hexString, "4fd1cb3c9c15c171b7b90dc3fefc7b2fc54de09b869cc9d6708d26b114e8d9a5")
        XCTAssertEqual(address.description, "GCRWFRVQP5XS7I4SFCL374VKV6OHJ3L3H3SDVGH7FW73N7LSNYJXOLDK")
    }

    func testDeriveTezos() {
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: .tezos)
        let pubkey = key.getPublicKeyEd25519()
        let address = AnyAddress(publicKey: pubkey, coin: .tezos)

        XCTAssertEqual(pubkey.data.hexString, "c834147f97bcf95bf01f234455646a197f70b25e93089591ffde8122370ad371")
        XCTAssertEqual("tz1RsC3AREfrMwh6Hdu7qGKxBwb1VgwJv1qw", address.description)
    }

    func testDeriveTezos2() {
        let wallet = HDWallet(mnemonic: "kidney setup media hat relief plastic ghost census mouse science expect movie", passphrase: "")!

        let key = wallet.getKeyForCoin(coin: .tezos)
        let address = CoinType.tezos.deriveAddress(privateKey: key)

        XCTAssertEqual("tz1M9ZMG1kthqQFK5dFi8rDCahqw6gHr1zoZ", address.description)
    }

    func testDeriveNimiq() {
        // mnemonic is from https://github.com/Eligioo/nimiq-hd-wallet, compatible with ledger
        // but it's not compatible with safe.nimiq.com (can't import)
        let wallet = HDWallet(mnemonic: "insane mixed health squeeze physical trust pipe possible garage hero flock stand profit power tooth review note camera express vicious clock machine entire heavy", passphrase: "")!
        let coin = CoinType.nimiq
        let key = wallet.getKeyForCoin(coin: coin)
        let address = coin.deriveAddress(privateKey: key)

        XCTAssertEqual("NQ77 XYYH YUNC V52U 5ADV 5JAY QXMD 2F9C Q440", address.description)
    }

    func testDeriveDecred() {
        let wallet = HDWallet.test
        let coin = CoinType.decred
        let key = wallet.getKeyForCoin(coin: coin)
        let address = coin.deriveAddress(privateKey: key)

        XCTAssertEqual("DsksmLD2wDoA8g8QfFvm99ASg8KsZL8eJFd", address.description)
    }

    func testDeriveGroestlcoin() {
        let groestlcoin = CoinType.groestlcoin
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: groestlcoin)
        let address = groestlcoin.deriveAddress(privateKey: key)

        XCTAssertEqual("grs1qsjpmsmm4x34wlt6kk4zef9u0jtculguktwgwg4", address)
    }

    func testDeriveDoge() {
        let doge = CoinType.dogecoin
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: doge)
        let address = doge.deriveAddress(privateKey: key)

        XCTAssertEqual("DJRoWqKj6hVmZMEMPahJ7UsqaYCtEJ3xv9", address)
    }

    func testDeriveZilliqa() {
        let zil = CoinType.zilliqa
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: .zilliqa)
        let address = zil.deriveAddress(privateKey: key)

        XCTAssertEqual(key.data.hexString, "b49a9fb16cd2b46ee538be807f712073009ea528e407a25a4bf91a63c3e49f99")
        XCTAssertEqual(address.description, "zil1vhs6jdq7prgsumeuqzfse6recklesqcesfe685")
    }

    func testSignHash() {
        let eth = CoinType.ethereum
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: eth)
        let hash = Data(hexString: "3F891FDA3704F0368DAB65FA81EBE616F4AA2A0854995DA4DC0B59D2CADBD64F")!
        let result = key.sign(digest: hash, curve: .secp256k1)!

        let publicKey = key.getPublicKeySecp256k1(compressed: false)
        XCTAssertEqual(result.count, 65)
        XCTAssertTrue(publicKey.verify(signature: result, message: hash))
    }

    func testExtendedKeys() {
        let wallet = HDWallet(mnemonic: "abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon about", passphrase: "")!

        let xprv = wallet.getExtendedPrivateKey(purpose: .bip44, coin: .bitcoin, version: .xprv)
        let xpub = wallet.getExtendedPublicKey(purpose: .bip44, coin: .bitcoin, version: .xpub)

        XCTAssertEqual(xprv, "xprv9xpXFhFpqdQK3TmytPBqXtGSwS3DLjojFhTGht8gwAAii8py5X6pxeBnQ6ehJiyJ6nDjWGJfZ95WxByFXVkDxHXrqu53WCRGypk2ttuqncb")
        XCTAssertEqual(xpub, "xpub6BosfCnifzxcFwrSzQiqu2DBVTshkCXacvNsWGYJVVhhawA7d4R5WSWGFNbi8Aw6ZRc1brxMyWMzG3DSSSSoekkudhUd9yLb6qx39T9nMdj")

        let yprv = wallet.getExtendedPrivateKey(purpose: .bip49, coin: .bitcoin, version: .yprv)
        let ypub = wallet.getExtendedPublicKey(purpose: .bip49, coin: .bitcoin, version: .ypub)

        XCTAssertEqual(yprv, "yprvAHwhK6RbpuS3dgCYHM5jc2ZvEKd7Bi61u9FVhYMpgMSuZS613T1xxQeKTffhrHY79hZ5PsskBjcc6C2V7DrnsMsNaGDaWev3GLRQRgV7hxF")
        XCTAssertEqual(ypub, "ypub6Ww3ibxVfGzLrAH1PNcjyAWenMTbbAosGNB6VvmSEgytSER9azLDWCxoJwW7Ke7icmizBMXrzBx9979FfaHxHcrArf3zbeJJJUZPf663zsP")

        let zprv = wallet.getExtendedPrivateKey(purpose: .bip84, coin: .bitcoin, version: .zprv)
        let zpub = wallet.getExtendedPublicKey(purpose: .bip84, coin: .bitcoin, version: .zpub)

        XCTAssertEqual(zprv, "zprvAdG4iTXWBoARxkkzNpNh8r6Qag3irQB8PzEMkAFeTRXxHpbF9z4QgEvBRmfvqWvGp42t42nvgGpNgYSJA9iefm1yYNZKEm7z6qUWCroSQnE")
        XCTAssertEqual(zpub, "zpub6rFR7y4Q2AijBEqTUquhVz398htDFrtymD9xYYfG1m4wAcvPhXNfE3EfH1r1ADqtfSdVCToUG868RvUUkgDKf31mGDtKsAYz2oz2AGutZYs")
    }

    func testDeriveFromXPub() {
        let xpub = "xpub6BosfCnifzxcFwrSzQiqu2DBVTshkCXacvNsWGYJVVhhawA7d4R5WSWGFNbi8Aw6ZRc1brxMyWMzG3DSSSSoekkudhUd9yLb6qx39T9nMdj"

        let bitcoin = CoinType.bitcoinCash
        let xpub2 = HDWallet.getPublicKeyFromExtended(
            extended: xpub,
            coin: bitcoin,
            derivationPath: DerivationPath(purpose: .bip44, coin: bitcoin.slip44Id, account: 0, change: 0, address: 2).description
        )!
        let xpub9 = HDWallet.getPublicKeyFromExtended(
            extended: xpub,
            coin: bitcoin,
            derivationPath: DerivationPath(purpose: .bip44, coin: bitcoin.slip44Id, account: 0, change: 0, address: 9).description
        )!

        let xpubAddr2 = BitcoinAddress(publicKey: xpub2, prefix: CoinType.bitcoin.p2pkhPrefix)!
        let xpubAddr9 = BitcoinAddress(publicKey: xpub9, prefix: CoinType.bitcoin.p2pkhPrefix)!

        XCTAssertEqual(xpubAddr2.description, "1MNF5RSaabFwcbtJirJwKnDytsXXEsVsNb")
        XCTAssertEqual(xpubAddr9.description, "1DUrqK4hj6vNNUTWXADpbqyjVWUYFD7xTZ")
    }

    func testDeriveFromYPub() {
        let ypub = "ypub6Ww3ibxVfGzLrAH1PNcjyAWenMTbbAosGNB6VvmSEgytSER9azLDWCxoJwW7Ke7icmizBMXrzBx9979FfaHxHcrArf3zbeJJJUZPf663zsP"

        let bitcoin = CoinType.bitcoin
        let ypub3 = HDWallet.getPublicKeyFromExtended(
            extended: ypub,
            coin: bitcoin,
            derivationPath: DerivationPath(purpose: .bip49, coin: bitcoin.slip44Id, account: 0, change: 0, address: 3).description
        )!
        let ypub10 = HDWallet.getPublicKeyFromExtended(
            extended: ypub,
            coin: bitcoin,
            derivationPath: DerivationPath(purpose: .bip49, coin: bitcoin.slip44Id, account: 0, change: 0, address: 10).description
        )!

        let ypubAddr3 = BitcoinAddress.compatibleAddress(publicKey: ypub3, prefix: CoinType.bitcoin.p2shPrefix)
        let ypubAddr10 = BitcoinAddress.compatibleAddress(publicKey: ypub10, prefix: CoinType.bitcoin.p2shPrefix)
        XCTAssertEqual(ypubAddr3.description, "38CahkVftQneLonbWtfWxiiaT2fdnzsEAN")
        XCTAssertEqual(ypubAddr10.description, "38mWd5D48ShYPJMZngtmxPQVYhQR5DGgfF")
    }

    func testDeriveFromZPub() {
        let zpub = "zpub6rFR7y4Q2AijBEqTUquhVz398htDFrtymD9xYYfG1m4wAcvPhXNfE3EfH1r1ADqtfSdVCToUG868RvUUkgDKf31mGDtKsAYz2oz2AGutZYs"
        let bitcoin = CoinType.bitcoin
        let zpubAddr4 = HDWallet.getPublicKeyFromExtended(
            extended: zpub,
            coin: bitcoin,
            derivationPath: DerivationPath(purpose: .bip84, coin: bitcoin.slip44Id, account: 0, change: 0, address: 4).description
        )!
        let zpubAddr11 = HDWallet.getPublicKeyFromExtended(
            extended: zpub,
            coin: bitcoin,
            derivationPath: DerivationPath(purpose: .bip84, coin: bitcoin.slip44Id, account: 0, change: 0, address: 11).description
        )!

        XCTAssertEqual(bitcoin.deriveAddressFromPublicKey(publicKey: zpubAddr4).description, "bc1qm97vqzgj934vnaq9s53ynkyf9dgr05rargr04n")
        XCTAssertEqual(bitcoin.deriveAddressFromPublicKey(publicKey: zpubAddr11).description, "bc1qxr4fjkvnxjqphuyaw5a08za9g6qqh65t8qwgum")
    }

    func testDeriveFromZPub2() {
        let zpub = "zpub6qeA5j9oSq8tZaYEBTp1X61ZSjeen6HbiUBSG4KLPD8d65Pi7eSMPNuxCqgbLdtnim2hgnJEzmE6jhFoJXtJdRxRKRdNFQBJ6iidx9BHGyk"

        let bitcoin = CoinType.bitcoin
        let path = DerivationPath(purpose: bitcoin.purpose, coin: bitcoin.slip44Id, account: 0, change: 0, address: 0)
        let pubkey = HDWallet.getPublicKeyFromExtended(extended: zpub, coin: bitcoin, derivationPath: path.description)!
        let address = bitcoin.deriveAddressFromPublicKey(publicKey: pubkey)

        XCTAssertEqual(pubkey.data.hexString, "039fdd3652495d01b6a363f8db8b3adce09f83ea5c43ff872ad0a39192340256b0")
        XCTAssertEqual(address.description, "bc1qearv5ezm3xfgy2t98denkzxwst4f35fvz608wa")
    }

    func testDeriveRavencoin() {
        let ravencoin = CoinType.ravencoin
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: ravencoin)
        let address = ravencoin.deriveAddress(privateKey: key)

        XCTAssertEqual("RHQmrg7nNFnRUwg2mH7GafhRY3ZaF6FB2x", address)
    }

    func testDeriveTerra() {
        let coin = CoinType.terra
        let key = HDWallet.test.getKeyForCoin(coin: coin)
        let address = CoinType.terra.deriveAddress(privateKey: key)

        XCTAssertEqual(address, "terra1jf9aaj9myrzsnmpdr7twecnaftzmku2mhs2hfe")
    }

    func testDeriveMonacoin() {
        let monacoin = CoinType.monacoin
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: monacoin)
        let address = monacoin.deriveAddress(privateKey: key)

        XCTAssertEqual("MHkScH5duuiaAkdQ22NkLWmXqWnjq3hThM", address)
    }

    func testDeriveFIO() {
        let fio = CoinType.fio

        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: fio)
        let address = fio.deriveAddress(privateKey: key)

        XCTAssertEqual("FIO5J2xdfWygeNdHZNZRzRws8YGbVxjUXtp4eP8KoGkGKoLFQ7CaU", address)
    }

    func testDeriveAlgorand() {
        let algo = CoinType.algorand
        let wallet = HDWallet.test
        let key = wallet.getKeyForCoin(coin: algo)

        let address = algo.deriveAddress(privateKey: key)

        XCTAssertEqual("VEFQ2IIW7CVKDLFEY53BMVJYKURC7KJTCW6U6R2CMQLQXZI52SCFSYASEY", address)
    }

    func testDeriveKava() {
        let coin = CoinType.kava
        let key = HDWallet.test.getKeyForCoin(coin: coin)
        let address = coin.deriveAddress(privateKey: key)

        XCTAssertEqual(address, "kava1zrst72upua78pylhku9csxd5zmhsyrk7xhrdlf")
    }

    func testDeriveBandChain() {
        let coin = CoinType.bandChain
        let key = HDWallet.test.getKeyForCoin(coin: coin)
        let address = coin.deriveAddress(privateKey: key)

        XCTAssertEqual(address, "band1pe8xm2r46rmctsukuqu7gl900vzprfsp4sguc3")
    }
}
