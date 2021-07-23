// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

import XCTest
import WalletCore

class EthereumTests: XCTestCase {

    func testAddress() {
        let anyAddress = AnyAddress(string: "0x7d8bf18c7ce84b3e175b339c4ca93aed1dd166f1", coin: .ethereum)

        XCTAssertEqual(anyAddress?.description, "0x7d8bf18C7cE84b3E175b339c4Ca93aEd1dD166F1")
        XCTAssertEqual(anyAddress?.coin, .ethereum)

        let invalid = "0xMQqpqMQgCBuiPkoXfgZZsJvuzCeI1zc00z6vHJj4"
        XCTAssertNil(Data(hexString: invalid))
        XCTAssertNil(AnyAddress(string: invalid, coin: .ethereum))
        XCTAssertFalse(AnyAddress.isValid(string: invalid, coin: .ethereum))
    }

    func testSigner() {
        let input = EthereumSigningInput.with {
            $0.chainID = Data(hexString: "01")!
            $0.nonce = Data(hexString: "09")!
            $0.gasPrice = Data(hexString: "04a817c800")!
            $0.gasLimit = Data(hexString: "5208")!
            $0.toAddress = "0x3535353535353535353535353535353535353535"
            $0.privateKey = Data(hexString: "0x4646464646464646464646464646464646464646464646464646464646464646")!
            $0.transaction = EthereumTransaction.with {
                $0.transfer = EthereumTransaction.Transfer.with {
                    $0.amount = Data(hexString: "0de0b6b3a7640000")!
                }
            }
        }

        let output: EthereumSigningOutput = AnySigner.sign(input: input, coin: .ethereum)

        XCTAssertEqual(output.encoded.hexString, "f86c098504a817c800825208943535353535353535353535353535353535353535880de0b6b3a76400008025a028ef61340bd939bc2195fe537567866003e1a15d3c71ff63e1590620aa636276a067cbe9d8997f761aecb703304b3800ccf555c9f3dc64214b297fb1966a3b6d83")
    }

    func testSignERC20Transfer() {
        let input = EthereumSigningInput.with {
            $0.chainID = Data(hexString: "01")!
            $0.nonce = Data(hexString: "00")!
            $0.gasPrice = Data(hexString: "09c7652400")! // 42000000000
            $0.gasLimit = Data(hexString: "0130B9")! // 78009
            $0.toAddress = "0x6b175474e89094c44da98b954eedeac495271d0f" // DAI
            $0.privateKey = Data(hexString: "0x608dcb1742bb3fb7aec002074e3420e4fab7d00cced79ccdac53ed5b27138151")!
            $0.transaction = EthereumTransaction.with {
                $0.erc20Transfer = EthereumTransaction.ERC20Transfer.with {
                    $0.to = "0x5322b34c88ed0691971bf52a7047448f0f4efc84"
                    $0.amount = Data(hexString: "1bc16d674ec80000")! // 2000000000000000000
                }
            }
        }
        let output: EthereumSigningOutput = AnySigner.sign(input: input, coin: .ethereum)

        XCTAssertEqual(output.encoded.hexString, "f8aa808509c7652400830130b9946b175474e89094c44da98b954eedeac495271d0f80b844a9059cbb0000000000000000000000005322b34c88ed0691971bf52a7047448f0f4efc840000000000000000000000000000000000000000000000001bc16d674ec8000025a0724c62ad4fbf47346b02de06e603e013f26f26b56fdc0be7ba3d6273401d98cea0032131cae15da7ddcda66963e8bef51ca0d9962bfef0547d3f02597a4a58c931")
        XCTAssertEqual(output.data.hexString, "a9059cbb0000000000000000000000005322b34c88ed0691971bf52a7047448f0f4efc840000000000000000000000000000000000000000000000001bc16d674ec80000")
    }

    func testSignERC20Transfer_1559() {
        let input = EthereumSigningInput.with {
            $0.chainID = Data(hexString: "01")!
            $0.nonce = Data(hexString: "00")!
            $0.gasLimit = Data(hexString: "0130B9")! // 78009
            $0.maxInclusionFeePerGas = Data(hexString: "0077359400")! // 2000000000
            $0.maxFeePerGas = Data(hexString: "00B2D05E00")! // 3000000000
            $0.toAddress = "0x6b175474e89094c44da98b954eedeac495271d0f" // DAI
            $0.privateKey = Data(hexString: "0x608dcb1742bb3fb7aec002074e3420e4fab7d00cced79ccdac53ed5b27138151")!
            $0.transaction = EthereumTransaction.with {
                $0.erc20Transfer = EthereumTransaction.ERC20Transfer.with {
                    $0.to = "0x5322b34c88ed0691971bf52a7047448f0f4efc84"
                    $0.amount = Data(hexString: "1bc16d674ec80000")! // 2000000000000000000
                }
            }
        }
        let output: EthereumSigningOutput = AnySigner.sign(input: input, coin: .ethereum)

        XCTAssertEqual(output.encoded.hexString, "02f8b00180847735940084b2d05e00830130b9946b175474e89094c44da98b954eedeac495271d0f80b844a9059cbb0000000000000000000000005322b34c88ed0691971bf52a7047448f0f4efc840000000000000000000000000000000000000000000000001bc16d674ec80000c080a0adfcfdf98d4ed35a8967a0c1d78b42adb7c5d831cf5a3272654ec8f8bcd7be2ea011641e065684f6aa476f4fd250aa46cd0b44eccdb0a6e1650d658d1998684cdf")
    }

    func testSignERC20Approve() {
        let input = EthereumSigningInput.with {
            $0.chainID = Data(hexString: "01")!
            $0.nonce = Data(hexString: "00")!
            $0.gasPrice = Data(hexString: "09c7652400")! // 42000000000
            $0.gasLimit = Data(hexString: "0130B9")! // 78009
            $0.toAddress = "0x6b175474e89094c44da98b954eedeac495271d0f" // DAI
            $0.privateKey = Data(hexString: "0x608dcb1742bb3fb7aec002074e3420e4fab7d00cced79ccdac53ed5b27138151")!
            $0.transaction = EthereumTransaction.with {
                $0.erc20Approve = EthereumTransaction.ERC20Approve.with {
                    $0.spender = "0x5322b34c88ed0691971bf52a7047448f0f4efc84"
                    $0.amount = Data(hexString: "1bc16d674ec80000")! // 2000000000000000000
                }
            }
        }
        let output: EthereumSigningOutput = AnySigner.sign(input: input, coin: .ethereum)

        XCTAssertEqual(output.encoded.hexString, "f8aa808509c7652400830130b9946b175474e89094c44da98b954eedeac495271d0f80b844095ea7b30000000000000000000000005322b34c88ed0691971bf52a7047448f0f4efc840000000000000000000000000000000000000000000000001bc16d674ec8000025a0d8136d66da1e0ba8c7208d5c4f143167f54b89a0fe2e23440653bcca28b34dc1a049222a79339f1a9e4641cb4ad805c49c225ae704299ffc10627bf41c035c464a")
    }

    func testSignERC721Transfer() {
        // https://etherscan.io/tx/0x3cde660762810d951d5cc57f8d907fbf229054908f1a770c3fb0b7b375cf4f82
        // real key 1p
        let input = EthereumSigningInput.with {
            $0.chainID = Data(hexString: "01")!
            $0.nonce = Data(hexString: "02de")! // 734
            $0.gasPrice = Data(hexString: "22ecb25c00")! // 150000000000
            $0.gasLimit = Data(hexString: "0130b9")! // 78009
            $0.toAddress = "0x0d8c864DA1985525e0af0acBEEF6562881827bd5" // contract
            $0.privateKey = Data(hexString: "0x608dcb1742bb3fb7aec002074e3420e4fab7d00cced79ccdac53ed5b27138151")!
            $0.transaction = EthereumTransaction.with {
                $0.erc721Transfer = EthereumTransaction.ERC721Transfer.with {
                    $0.from = "0x7d8bf18C7cE84b3E175b339c4Ca93aEd1dD166F1"
                    $0.to = "0x47331175b23C2f067204B506CA1501c26731C990"
                    $0.tokenID = Data(hexString: "0fd8")! // 4056
                }
            }
        }
        let output: EthereumSigningOutput = AnySigner.sign(input: input, coin: .ethereum)

        XCTAssertEqual(output.encoded.hexString, "f8cc8202de8522ecb25c00830130b9940d8c864da1985525e0af0acbeef6562881827bd580b86423b872dd0000000000000000000000007d8bf18c7ce84b3e175b339c4ca93aed1dd166f100000000000000000000000047331175b23c2f067204b506ca1501c26731c9900000000000000000000000000000000000000000000000000000000000000fd825a04c5d8242a8c2db1cfa352a3486dd85c82824e01b9bcf0ce4170fcd2329fb7bcaa02d85ab09e750a73fd4dd26b142830ada1e991f8474795b43d96d93e65caaefe7")
        XCTAssertEqual(output.data.hexString, "23b872dd0000000000000000000000007d8bf18c7ce84b3e175b339c4ca93aed1dd166f100000000000000000000000047331175b23c2f067204b506ca1501c26731c9900000000000000000000000000000000000000000000000000000000000000fd8")
    }

    func testSignERC1155Transfer() {
        let input = EthereumSigningInput.with {
            $0.chainID = Data(hexString: "01")!
            $0.nonce = Data(hexString: "00")!
            $0.gasPrice = Data(hexString: "09C7652400")! // 42000000000
            $0.gasLimit = Data(hexString: "0130b9")! // 78009
            $0.toAddress = "0x4e45e92ed38f885d39a733c14f1817217a89d425" // contract
            $0.privateKey = Data(hexString: "0x608dcb1742bb3fb7aec002074e3420e4fab7d00cced79ccdac53ed5b27138151")!
            $0.transaction = EthereumTransaction.with {
                $0.erc1155Transfer = EthereumTransaction.ERC1155Transfer.with {
                    $0.from = "0x718046867b5b1782379a14eA4fc0c9b724DA94Fc"
                    $0.to = "0x5322b34c88ed0691971bf52a7047448f0f4efc84"
                    $0.tokenID = Data(hexString: "0x23c47ee5")!
                    $0.value = Data(hexString: "0x1BC16D674EC80000")! // 2000000000000000000
                    $0.data = Data(hexString: "0x01020304")!
                }
            }
        }
        let output: EthereumSigningOutput = AnySigner.sign(input: input, coin: .ethereum)

        XCTAssertEqual(output.encoded.hexString, "f9014a808509c7652400830130b9944e45e92ed38f885d39a733c14f1817217a89d42580b8e4f242432a000000000000000000000000718046867b5b1782379a14ea4fc0c9b724da94fc0000000000000000000000005322b34c88ed0691971bf52a7047448f0f4efc840000000000000000000000000000000000000000000000000000000023c47ee50000000000000000000000000000000000000000000000001bc16d674ec8000000000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000000000004010203040000000000000000000000000000000000000000000000000000000026a010315488201ac801ce346bffd1570de147615462d7e7db3cf08cf558465c6b79a06643943b24593bc3904a9fda63bb169881730994c973ab80f07d66a698064573")
        XCTAssertEqual(output.data.hexString, "f242432a000000000000000000000000718046867b5b1782379a14ea4fc0c9b724da94fc0000000000000000000000005322b34c88ed0691971bf52a7047448f0f4efc840000000000000000000000000000000000000000000000000000000023c47ee50000000000000000000000000000000000000000000000001bc16d674ec8000000000000000000000000000000000000000000000000000000000000000000a000000000000000000000000000000000000000000000000000000000000000040102030400000000000000000000000000000000000000000000000000000000")
    }

    func testSignJSON() {
        let json = """
        {
            "chainId": "AQ==",
            "gasPrice": "1pOkAA==",
            "gasLimit": "Ugg=",
            "toAddress": "0x7d8bf18C7cE84b3E175b339c4Ca93aEd1dD166F1",
            "transaction": {
                "transfer": {
                    "amount":"A0i8paFgAA=="
                }
            }
        }
        """
        let key = Data(hexString: "17209af590a86462395d5881e60d11c7fa7d482cfb02b5a01b93c2eeef243543")!
        let result = AnySigner.signJSON(json, key: key, coin: .ethereum)

        XCTAssertEqual(result, "f86a8084d693a400825208947d8bf18c7ce84b3e175b339c4ca93aed1dd166f1870348bca5a160008025a0fe5802b49e04c6b1705088310e133605ed8b549811a18968ad409ea02ad79f21a05bf845646fb1e1b9365f63a7fd5eb5e984094e3ed35c3bed7361aebbcbf41f10")
    }
}
