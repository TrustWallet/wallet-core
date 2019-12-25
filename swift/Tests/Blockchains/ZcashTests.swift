// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

import XCTest
import TrustWalletCore

class ZcashTests: XCTestCase {

    let zcash = CoinType.zcash

    func testValidAddresses() {

        XCTAssertTrue(zcash.validate(address: "t1TKCtCETHPrAdA6eY1fdhhnTkTmb371oPt"))
        XCTAssertTrue(zcash.validate(address: "t3Pnbg7XjP7FGPBUuz75H65aczphHgkpoJW"))

        // XCTAssertTrue(zcash.validate(address: "zcXkH2LxmKKQH9D1q7xCrFT6gEKscT1K5f2HGFKxtXUfwczjLy1BZXKMuCdCU616yAVSkrdwzazN6DDh3zL82ULqpggEw51"))
        // XCTAssertTrue(zcash.validate(address: "zs10lpewlejk9lvdnnx4jwf7vssstcag2whqxj8e6s38kdwu6xtsh85d25fhqcrt6fz9wpxvsw0nsl"))
    }

    func testInvalidAddresses() {
        for addr in [
            "bc1qvtvte5tzlqlfhcdmph94lxk8jcz54q6psyvgla",
            "18eqGohuqvrZLL3LMR4Wv81qvKeDHsGpjH",
            "LV7LV7Z4bWDEjYkfx9dQo6k6RjGbXsg6hS",
            "Xm1iDLBP5tdxTxc6t7uJBCVjC4L2A5vB2J",
            "TKjdnbJxP4yHeLTHZ86DGnFFY6QhTjuBv2",
            ] {
            XCTAssertFalse(zcash.validate(address: addr))
        }
    }

    func testDeriveFromXpub() {
        let xpub = "xpub6C7HhMqpir3KBA6ammv5B58RT3XFTJqoZFoj3J56dz9XwehZ2puSH38ERtnz7HaXGxaZP8AHT4M2bSRHpBXUZrbsJ2xg3xs53DGKYCqj8mr"
        let pubkey = HDWallet.derive(from: xpub, at: DerivationPath(purpose: zcash.purpose, coinType: zcash))!
        let address = ZcashTAddress(publicKey: pubkey, prefix: zcash.p2pkhPrefix)!

        XCTAssertEqual(address.description, "t1TKCtCETHPrAdA6eY1fdhhnTkTmb371oPt")
    }

    func testSign() throws {
        let utxos = [
            BitcoinUnspentTransaction.with {
                $0.outPoint.hash = Data(hexString: "53685b8809efc50dd7d5cb0906b307a1b8aa5157baa5fc1bd6fe2d0344dd193a")!
                $0.outPoint.index = 0
                $0.outPoint.sequence = UINT32_MAX
                $0.amount = 494000
                $0.script = Data(hexString: "76a914f84c7f4dd3c3dc311676444fdead6e6d290d50e388ac")!
            }
        ]
        let input = BitcoinSigningInput.with {
            $0.hashType = BitcoinSigHashType.all.rawValue
            $0.amount = 488000
            $0.toAddress = "t1QahNjDdibyE4EdYkawUSKBBcVTSqv64CS"
            $0.coinType = CoinType.zcash.rawValue
            $0.privateKey = [Data(hexString: "a9684f5bebd0e1208aae2e02bc9e9163bd1965ad23d8538644e1df8b99b99559")!]
        }
        let plan = BitcoinTransactionPlan.with {
            $0.amount = 488000
            $0.fee = 6000
            $0.change = 0
            $0.branchID = Data(hexString: "0xbb09b876")!
            $0.utxos = utxos
        }

        let signer = ZcashTransactionSigner(input: input, plan: plan)
        let result = signer.sign()
        let output = try BitcoinSigningOutput(unpackingAny: result.objects[0])

        XCTAssertTrue(result.success)
        XCTAssertEqual(output.encoded.hexString, "0400008085202f890153685b8809efc50dd7d5cb0906b307a1b8aa5157baa5fc1bd6fe2d0344dd193a000000006b483045022100ca0be9f37a4975432a52bb65b25e483f6f93d577955290bb7fb0060a93bfc92002203e0627dff004d3c72a957dc9f8e4e0e696e69d125e4d8e275d119001924d3b48012103b243171fae5516d1dc15f9178cfcc5fdc67b0a883055c117b01ba8af29b953f6ffffffff0140720700000000001976a91449964a736f3713d64283fd0018626ba50091c7e988ac00000000000000000000000000000000000000")
    }
}
