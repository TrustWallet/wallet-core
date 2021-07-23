// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

import XCTest
import WalletCore

class BinanceChainTests: XCTestCase {

    let testKey = PrivateKey(data: Data(hexString: "eeba3f6f2db26ced519a3d4c43afff101db957a21d54d25dc7fd235c404d7a5d")!)!

    func testAddress() {
        let publicKey = PublicKey(data: Data(hexString: "0x026a35920088d98c3888ca68c53dfc93f4564602606cbb87f0fe5ee533db38e502")!, type: .secp256k1)!
        let address = AnyAddress(publicKey: publicKey, coin: .binance)

        XCTAssertEqual("bnb1grpf0955h0ykzq3ar5nmum7y6gdfl6lxfn46h2", address.description)
    }

    func testBinanceMainnet() {
        let wallet = HDWallet(mnemonic: "rabbit tilt arm protect banner ill produce vendor april bike much identify pond upset front easily glass gallery address hair priority focus forest angle", passphrase: "")!
        let key = wallet.getKeyForCoin(coin: .binance)
        let address = CoinType.binance.deriveAddress(privateKey: key)

        XCTAssertEqual(key.data.hexString, "727f677b390c151caf9c206fd77f77918f56904b5504243db9b21e51182c4c06")
        XCTAssertEqual("bnb1devga6q804tx9fqrnx0vtu5r36kxgp9tmk4xkm", address.description)
    }

    func testSignSendOrder() {
        let privateKey = PrivateKey(data: Data(hexString: "95949f757db1f57ca94a5dff23314accbe7abee89597bf6a3c7382c84d7eb832")!)!
        let publicKey = privateKey.getPublicKeySecp256k1(compressed: true)

        let token = BinanceSendOrder.Token.with {
            $0.denom = "BNB" // BNB or BEP2 token symbol
            $0.amount = 1
        }

        let orderInput = BinanceSendOrder.Input.with {
            $0.address = AnyAddress(publicKey: publicKey, coin: .binance).data
            $0.coins = [token]
        }

        let orderOutput = BinanceSendOrder.Output.with {
            $0.address = AnyAddress(string: "bnb1hlly02l6ahjsgxw9wlcswnlwdhg4xhx38yxpd5", coin: .binance)!.data
            $0.coins = [token]
        }

        let input = BinanceSigningInput.with {
            $0.chainID = "Binance-Chain-Nile" // Chain id (network id),                 from /v1/node-info api
            $0.accountNumber = 0              // On chain account / address number,     from /v1/account/<address> api
            $0.sequence = 0                   // Sequence number, plus 1 for new order, from /v1/account/<address> api
            $0.source = 0                     // BEP10 source id
            $0.privateKey = privateKey.data
            $0.memo = ""
            $0.sendOrder = BinanceSendOrder.with {
                $0.inputs = [orderInput]
                $0.outputs = [orderOutput]
            }
        }

        let output: BinanceSigningOutput = AnySigner.sign(input: input, coin: .binance)

        // swiftlint:disable:next line_length
        XCTAssertEqual(output.encoded.hexString, "b801f0625dee0a462a2c87fa0a1f0a1440c2979694bbc961023d1d27be6fc4d21a9febe612070a03424e421001121f0a14bffe47abfaede50419c577f1074fee6dd1535cd112070a03424e421001126a0a26eb5ae98721026a35920088d98c3888ca68c53dfc93f4564602606cbb87f0fe5ee533db38e50212401b1181faec30b60a2ddaa2804c253cf264c69180ec31814929b5de62088c0c5a45e8a816d1208fc5366bb8b041781a6771248550d04094c3d7a504f9e8310679")
    }

    func testSignTransferOutOrder() throws {
        let key = testKey
        let pubkey = key.getPublicKeySecp256k1(compressed: true)
        let input = BinanceSigningInput.with {
            $0.chainID = "test-chain"
            $0.accountNumber = 15
            $0.sequence = 1
            $0.privateKey = key.data
            $0.transferOutOrder = BinanceTransferOut.with {
                $0.from = AnyAddress(publicKey: pubkey, coin: .binance).data
                $0.to = AnyAddress(string: "0x35552c16704d214347f29Fa77f77DA6d75d7C752", coin: .ethereum)!.data
                $0.amount = BinanceSendOrder.Token.with {
                    $0.denom = "BNB"
                    $0.amount = 100000000
                }
                $0.expireTime = 12345678
            }
        }

        let output: BinanceSigningOutput = AnySigner.sign(input: input, coin: .binance)

        // swiftlint:disable:next line_length
        XCTAssertEqual(output.encoded.hexString, "b701f0625dee0a41800819c00a1408c7c918f6b72c3c0c21b7d08eb6fc66509998e1121435552c16704d214347f29fa77f77da6d75d7c7521a0a0a03424e421080c2d72f20cec2f105126e0a26eb5ae9872103a9a55c040c8eb8120f3d1b32193250841c08af44ea561aac993dbe0f6b6a8fc712407eda148e1167b1be1271a788ccf4e3eade1c7e1773e9d2093982d7f802f8f85f35ef550049011728206e4eda1a272f9e96fd95ef3983cad85a29cd14262c22e0180f2001")
    }

    func testSignStakeOrder() throws {
        let key = testKey
        let pubkey = key.getPublicKeySecp256k1(compressed: true)
        let input = BinanceSigningInput.with {
            $0.chainID = "test-chain"
            $0.accountNumber = 15
            $0.sequence = 1
            $0.privateKey = key.data
            $0.sideDelegateOrder = BinanceSideChainDelegate.with {
                $0.delegatorAddr = AnyAddress(publicKey: pubkey, coin: .binance).data
                $0.validatorAddr = AnyAddress(string: "bva10npy5809y303f227g4leqw7vs3s6ep5ul26sq2", coin: .binance)!.data
                $0.delegation = BinanceSendOrder.Token.with {
                    $0.denom = "BNB"
                    $0.amount = 200000000
                }
                $0.chainID = "chapel"
            }
        }

        let output: BinanceSigningOutput = AnySigner.sign(input: input, coin: .binance)

        // swiftlint:disable:next line_length
        XCTAssertEqual(output.encoded.hexString, "ba01f0625dee0a44e3a07fd20a1408c7c918f6b72c3c0c21b7d08eb6fc66509998e112147cc24a1de5245f14a95e457f903bcc8461ac869c1a0a0a03424e42108084af5f220663686170656c126e0a26eb5ae9872103a9a55c040c8eb8120f3d1b32193250841c08af44ea561aac993dbe0f6b6a8fc7124039302c9975fb2a09ac2b6b6fb1d3b9fb5b4c03630d3d7a7da42b1c6736d6127142a3fcdca0b70a3d065da8d4f4df8b5d9d8f46aeb3627a7d7aa901fe186af34c180f2001")
    }

    func testSignTimeLock() throws {
        // https://explorer.binance.org/tx/802824DCB6811339EA90118D33A57916D4698C9F982A78506BAC3AB7F1B6977B
        // real key 1p
        let key = testKey
        let from = AnyAddress(publicKey: key.getPublicKeySecp256k1(compressed: true), coin: .binance)

        let input = BinanceSigningInput.with {
            $0.chainID = "Binance-Chain-Tigris"
            $0.accountNumber = 29
            $0.sequence = 1313
            $0.privateKey = key.data
            $0.timeLockOrder = BinanceTimeLockOrder.with {
                $0.fromAddress = from.data
                $0.description_p = "test"
                $0.amount = [
                    BinanceSendOrder.Token.with {
                        $0.amount = 10000000
                        $0.denom = "BUSD-BD1"
                    }
                ]
                $0.lockTime = 1603002246
            }
        }
        let output: BinanceSigningOutput = AnySigner.sign(input: input, coin: .binance)
        XCTAssertEqual(output.encoded.hexString, "ae01f0625dee0a37079215310a1408c7c918f6b72c3c0c21b7d08eb6fc66509998e11204746573741a0f0a08425553442d4244311080ade2042086bfaffc05126f0a26eb5ae9872103a9a55c040c8eb8120f3d1b32193250841c08af44ea561aac993dbe0f6b6a8fc71240ef7ef0f792c2ec0b06181a176a842d6f475c21e784eb0cbc93fc27795cf5573a58b2364e57b9889dbf1da6175de2ba7c6dc8e7c9b596e8bf4abe2b1fd32165a9181d20a10a")
    }

    func testSignUnlock() throws {
        // https://explorer.binance.org/tx/97074A501A69DE20391C22CA8F79F991FC7D174727D235C2EA345DA485ECEAA0
        let key = testKey
        let from = AnyAddress(publicKey: key.getPublicKeySecp256k1(compressed: true), coin: .binance)
        let input = BinanceSigningInput.with {
            $0.chainID = "Binance-Chain-Tigris"
            $0.accountNumber = 29
            $0.sequence = 1314
            $0.privateKey = key.data
            $0.timeUnlockOrder = BinanceTimeUnlockOrder.with {
                $0.fromAddress = from.data
                $0.id = 1312
            }
        }
        let output: BinanceSigningOutput = AnySigner.sign(input: input, coin: .binance)
        XCTAssertEqual(output.encoded.hexString, "9401f0625dee0a1dc4050c6c0a1408c7c918f6b72c3c0c21b7d08eb6fc66509998e110a00a126f0a26eb5ae9872103a9a55c040c8eb8120f3d1b32193250841c08af44ea561aac993dbe0f6b6a8fc7124036ae67e8301f7b229ead7f299429308e70c5b44a28d173ecf951c5fe94507a767dcbda27c11610845210d0b9a8fec82e111a6c18c933a32d702d001900b02601181d20a20a")
    }

    func testSignRelock() throws {
        // https://explorer.binance.org/tx/1DAE8A0871A15D06907FF8DAD29D657309DA8A9004CAF5B94FF9B61F8B52B5E5
        let key = testKey
        let from = AnyAddress(publicKey: key.getPublicKeySecp256k1(compressed: true), coin: .binance)
        let input = BinanceSigningInput.with {
            $0.chainID = "Binance-Chain-Tigris"
            $0.accountNumber = 29
            $0.sequence = 1315
            $0.privateKey = key.data
            $0.timeRelockOrder = BinanceTimeRelockOrder.with {
                $0.fromAddress = from.data
                $0.id = 1314
                $0.description_p = "test_relock"
                $0.lockTime = 1603022621
            }
        }
        let output: BinanceSigningOutput = AnySigner.sign(input: input, coin: .binance)
        XCTAssertEqual(output.encoded.hexString, "a701f0625dee0a30504711da0a1408c7c918f6b72c3c0c21b7d08eb6fc66509998e110a20a1a0b746573745f72656c6f636b289ddeb0fc05126f0a26eb5ae9872103a9a55c040c8eb8120f3d1b32193250841c08af44ea561aac993dbe0f6b6a8fc71240b8df4f80b8f7714d13280134ee16dc04c826cbc50a0fb20532e91879320b24f812698f736b3fccb3060b3bdc0ffcdbd2ddf2026aeda32c6d159205f183c712cf181d20a30a")
    }

    func testSignJSON() {
        let json = """
        {
            "chainId": "Binance-Chain-Tigris",
            "accountNumber": "13186",
            "source": "2",
            "memo": "Testing",
            "sendOrder": {
                "inputs": [{
                    "address": "EuZU7e+eUIuDNzaph9Bp2lqJrts=",
                    "coins": [{
                        "denom": "BNB",
                        "amount": "1345227"
                    }]
                }],
                "outputs": [{
                    "address": "M7vzB7mBRvE9IGk8+UbC13pMryg=",
                    "coins": [{
                        "denom": "BNB",
                        "amount": "1345227"
                    }]
                }]
            }
        }
        """
        let key = Data(hexString: "f947b3554a1c2fa70e1caa9de53fbda353348d1e856c593848f3a29737d31f11")!
        let result = AnySigner.signJSON(json, key: key, coin: .binance)

        XCTAssertEqual(result, "ca01f0625dee0a4a2a2c87fa0a210a1412e654edef9e508b833736a987d069da5a89aedb12090a03424e4210cb8d5212210a1433bbf307b98146f13d20693cf946c2d77a4caf2812090a03424e4210cb8d52126d0a26eb5ae9872102e58176f271a9796b4288908be85094a2ac978e25535fd59a37b58626e3a84d9e1240015b4beb3d6ef366a7a92fd283f66b8f0d8cdb6b152a9189146b27f84f507f047e248517cf691a36ebc2b7f3b7f64e27585ce1c40f1928d119c31af428efcf3e1882671a0754657374696e672002")
    }

    func testSignOrderOneThread() {
        let n = 50
        for _ in 1...n {
            testSignSendOrder()
        }
    }

    func testMultiThreadedSign() {
        let nThread = 5
        let queue = OperationQueue()
        for _ in 1...nThread {
            queue.addOperation {
                self.testSignOrderOneThread()
            }
        }
        queue.waitUntilAllOperationsAreFinished()
    }
}
