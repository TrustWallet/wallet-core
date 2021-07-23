// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

import XCTest
import WalletCore

class CoinAddressDerivationTests: XCTestCase {

    func testDerive() {
        let wallet = HDWallet(mnemonic: "shoot island position soft burden budget tooth cruel issue economy destroy above", passphrase: "")!

        for _ in 0..<4 {
            for coin in CoinType.allCases {
                let privateKey = wallet.getKeyForCoin(coin: coin)
                let derivedAddress = coin.deriveAddress(privateKey: privateKey)
                let address = coin.address(string: derivedAddress)

                switch coin {
                case .aeternity:
                    let expectedResult = "ak_QDHJSfvHG9sDHBobaWt2TAGhuhipYjEqZEH34bWugpJfJc3GN"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .aion:
                    let expectedResult = "0xa0629f34c9ea4757ad0b275628d4d02e3db6c9009ba2ceeba76a5b55fb2ca42e"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .algorand:
                    let expectedResult = "JTJWO524JXIHVPGBDWFLJE7XUIA32ECOZOBLF2QP3V5TQBT3NKZSCG67BQ"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .bandChain:
                    let expectedResult = "band1624hqgend0s3d94z68fyka2y5jak6vd7u0l50r"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .binance:
                    let expectedResult = "bnb12vtaxl9952zm6rwf7v8jerq74pvaf77fcmvzhw"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .bitcoin:
                    let expectedResult = "bc1quvuarfksewfeuevuc6tn0kfyptgjvwsvrprk9d"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .bitcoinCash:
                    let expectedResult = "bitcoincash:qpzl3jxkzgvfd9flnd26leud5duv795fnv7vuaha70"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .bitcoinGold:
                    let expectedResult = "btg1qwz9sed0k4neu6ycrudzkca6cnqe3zweq35hvtg"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .callisto:
                    let expectedResult = "0x3E6FFC80745E6669135a76F4A7ce6BCF02436e04"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .cardano:
                    let expectedResult = "addr1snpa4z7ntyfszv7ckquprdw75w4qjqh0qmya9jtkpxxlzxghlqyvv7l0yjamh8fxraw06p3ua8sj2g2gv98v4849s43t9g2999kquuu5egnprk"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .cosmos:
                    let expectedResult = "cosmos142j9u5eaduzd7faumygud6ruhdwme98qsy2ekn"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .dash:
                    let expectedResult = "XqHiz8EXYbTAtBEYs4pWTHh7ipEDQcNQeT"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .decred:
                    let expectedResult = "DsidJiDGceqHTyqiejABy1ZQ3FX4SiWZkYG"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .digiByte:
                    let expectedResult = "dgb1qtjgmerfqwdffyf8ghcrkgy52cghsqptynmyswu"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .dogecoin:
                    let expectedResult = "DJRFZNg8jkUtjcpo2zJd92FUAzwRjitw6f"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .elrond:
                    let expectedResult = "erd1jfcy8aeru6vlx4fe6h3pc3vlpe2cnnur5zetxdhp879yagq7vqvs8na4f8"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .eos:
                    let expectedResult = "EOS6hs8sRvGSzuQtq223zwJipMzqTJpXUVjyvHPvPwBSZWWrJTJkg"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .ethereum, .smartChain, .polygon, .optimism, .arbitrum, .ecochain:
                    let expectedResult = "0x8f348F300873Fd5DA36950B2aC75a26584584feE"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .ethereumClassic:
                    let expectedResult = "0x078bA3228F3E6C08bEEac9A005de0b7e7089aD1c"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .filecoin:
                    let expectedResult = "f1zzykebxldfcakj5wdb5n3n7priul522fnmjzori"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .fio:
                    let expectedResult = "FIO7MN1LuSfFgrbVHmrt9cVa2FYAs857Ppr9dzvEXoD1miKSxm3n3"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .goChain:
                    let expectedResult = "0x5940ce4A14210d4Ccd0ac206CE92F21828016aC2"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .groestlcoin:
                    let expectedResult = "grs1qexwmshts5pdpeqglkl39zyl6693tmfwp0cue4j"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .harmony:
                    let expectedResult = "one12fk20wmvgypdkn59n4hq8e3aa5899xfx4vsu09"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .icon:
                    let expectedResult = "hx18b380b53c23dc4ee9f6666bc20d1be02f3fe106"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .ioTeX:
                    let expectedResult = "io1qw9cccecw09q7p5kzyqtuhfhvah2mhfrc84jfk"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .litecoin:
                    let expectedResult = "ltc1qhd8fxxp2dx3vsmpac43z6ev0kllm4n53t5sk0u"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .monacoin:
                    let expectedResult = "M9xFZzZdZhCDxpx42cM8bQHnLwaeX1aNja"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .nano:
                    let expectedResult = "nano_39gsbcishxn3n7wd17ono4otq5wazwzusqgqigztx73wbrh5jwbdbshfnumc"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .near:
                    let expectedResult = "0c91f6106ff835c0195d5388565a2d69e25038a7e23d26198f85caf6594117ec"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .nebulas:
                    let expectedResult = "n1ZVgEidtdseYv9ogmGz69Cz4mbqmHYSNqJ"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .neo:
                    let expectedResult = "AT6w7PJvwPcSqHvtbNBY2aHPDv12eW5Uuf"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .nimiq:
                    let expectedResult = "NQ76 7AVR EHDA N05U X7SY XB14 XJU7 8ERV GM6H"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .nuls:
                    let expectedResult = "NULSd6HgU8MoRnNjBgvJpa9tqvGxYdv5ne4en"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .kava:
                    let expectedResult = "kava1drpa0x9ptz0fql3frv562rcrhj2nstuz3pas87"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .kin:
                    let expectedResult = "GBL3MT2ICHHM5OJ2QJ44CAHGDK6MLPINVXBKOKLHGBWQDVRWTWQ7U2EA"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .kusama:
                    let expectedResult = "G9xV2EatmrjRC1FLPexc3ddqNRRzCsAdURU8RFiAAJX6ppY"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .ontology:
                    let expectedResult = "AHKTnybvnWo3TeY8uvNXekvYxMrXogUjeT"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .poanetwork:
                    let expectedResult = "0xe8a3e8bE17E172B6926130eAfB521e9D2849aca9"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .polkadot:
                    let expectedResult = "13nN6BGAoJwd7Nw1XxeBCx5YcBXuYnL94Mh7i3xBprqVSsFk"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .qtum:
                    let expectedResult = "QhceuaTdeCZtcxmVc6yyEDEJ7Riu5gWFoF"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .ravencoin:
                    let expectedResult = "RHoCwPc2FCQqwToYnSiAb3SrCET4zEHsbS"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .solana:
                    let expectedResult = "2bUBiBNZyD29gP1oV6de7nxowMLoDBtopMMTGgMvjG5m"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .stellar:
                    let expectedResult = "GA3H6I4C5XUBYGVB66KXR27JV5KS3APSTKRUWOIXZ5MVWZKVTLXWKZ2P"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .terra:
                    let expectedResult = "terra1rh402g98t7sly8trzqw5cyracntlep6qe3smug"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .tezos:
                    let expectedResult = "tz1acnY9VbMagps26Kj3RfoGRWD9nYG5qaRX"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .theta:
                    let expectedResult = "0x0d1fa20c218Fec2f2C55d52aB267940485fa5DA4"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .thunderToken:
                    let expectedResult = "0x4b92b3ED6d8b24575Bf5ce4C6a86ED261DA0C8d7"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .tomoChain:
                    let expectedResult = "0xC74b6D8897cBa9A4b659d43fEF73C9cA852cE424"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .tron:
                    let expectedResult = "TQ5NMqJjhpQGK7YJbESKtNCo86PJ89ujio"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .veChain:
                    let expectedResult = "0x1a553275dF34195eAf23942CB7328AcF9d48c160"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .viacoin:
                    let expectedResult = "via1qnmsgjd6cvfprnszdgmyg9kewtjfgqflz67wwhc"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .wanchain:
                    let expectedResult = "0xD5ca90b928279FE5D06144136a25DeD90127aC15"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .waves:
                    let expectedResult = "3P63vkaHhyE9pPv9EfsjwGKqmZYcCRHys4n"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .xrp:
                    let expectedResult = "rPwE3gChNKtZ1mhH3Ko8YFGqKmGRWLWXV3"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .zcash:
                    let expectedResult = "t1YYnByMzdGhQv3W3rnjHMrJs6HH4Y231gy"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .zcoin:
                    let expectedResult = "aEd5XFChyXobvEics2ppAqgK3Bgusjxtik"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .zelcash:
                    let expectedResult = "t1UKbRPzL4WN8Rs8aZ8RNiWoD2ftCMHKGUf"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .zilliqa:
                    let expectedResult = "zil1mk6pqphhkmaguhalq6n3cq0h38ltcehg0rfmv6"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .smartChainLegacy:
                    let expectedResult = "0x49784f90176D8D9d4A3feCDE7C1373dAAb5b13b8"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .oasis:
                    let expectedResult = "oasis1qzcpavvmuw280dk0kd4lxjhtpf0u3ll27yf7sqps"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .thorchain:
                    let expectedResult = "thor1c8jd7ad9pcw4k3wkuqlkz4auv95mldr2kyhc65"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                case .bluzelle:
                    let expectedResult = "bluzelle1xccvees6ev4wm2r49rc6ptulsdxa8x8jfpmund"
                    assertCoinDerivation(coin, expectedResult, derivedAddress, address)
                @unknown default:
                    fatalError()
                }
            }
        }
    }

    private func assertCoinDerivation(_ coin: CoinType, _ expected: String, _ derivedAddress: String, _ address: AnyAddress?) {
        XCTAssertNotNil(address, "\(coin) is not implemented CoinType.address(string: String)")
        XCTAssertEqual(expected, derivedAddress, "\(coin) failed to match address")
        XCTAssertEqual(expected, address?.description, "\(coin) is not implemented CoinType.address(string: String)")
    }
}
