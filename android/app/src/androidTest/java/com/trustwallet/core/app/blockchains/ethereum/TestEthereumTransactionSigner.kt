package com.trustwallet.core.app.blockchains.ethereum

import com.google.protobuf.ByteString
import com.trustwallet.core.app.utils.toHexByteArray
import org.junit.Assert.assertEquals
import org.junit.Test
import wallet.core.jni.PrivateKey
import wallet.core.java.AnySigner
import wallet.core.jni.CoinType
import wallet.core.jni.CoinType.ETHEREUM
import wallet.core.jni.proto.Ethereum
import wallet.core.jni.proto.Ethereum.SigningOutput
import com.trustwallet.core.app.utils.Numeric
import org.junit.Assert.assertArrayEquals

class TestEthereumTransactionSigner {

    init {
        System.loadLibrary("TrustWalletCore")
    }

    @Test
    fun testEthereumTransactionSigning() {
        val signingInput = Ethereum.SigningInput.newBuilder()
        signingInput.apply {
            privateKey = ByteString.copyFrom(PrivateKey("0x4646464646464646464646464646464646464646464646464646464646464646".toHexByteArray()).data())
            toAddress = "0x3535353535353535353535353535353535353535"
            chainId = ByteString.copyFrom("0x1".toHexByteArray())
            nonce = ByteString.copyFrom("0x9".toHexByteArray())
            gasPrice = ByteString.copyFrom("0x04a817c800".toHexByteArray())
            gasLimit = ByteString.copyFrom("0x5208".toHexByteArray())
            transaction = Ethereum.Transaction.newBuilder().apply {
                transfer = Ethereum.Transaction.Transfer.newBuilder().apply {
                    amount = ByteString.copyFrom("0x0de0b6b3a7640000".toHexByteArray())
                }.build()
            }.build()
        }

        val output = AnySigner.sign(signingInput.build(), ETHEREUM, SigningOutput.parser())

        assertEquals(Numeric.toHexString(output.encoded.toByteArray()), "0xf86c098504a817c800825208943535353535353535353535353535353535353535880de0b6b3a76400008025a028ef61340bd939bc2195fe537567866003e1a15d3c71ff63e1590620aa636276a067cbe9d8997f761aecb703304b3800ccf555c9f3dc64214b297fb1966a3b6d83")
    }

    @Test
    fun testEthereumERC20Signing() {
        val signingInput = Ethereum.SigningInput.newBuilder()
        signingInput.apply {
            privateKey = ByteString.copyFrom(PrivateKey("0x608dcb1742bb3fb7aec002074e3420e4fab7d00cced79ccdac53ed5b27138151".toHexByteArray()).data())
            toAddress = "0x6b175474e89094c44da98b954eedeac495271d0f" // DAI
            chainId = ByteString.copyFrom("0x1".toHexByteArray())
            nonce = ByteString.copyFrom("0x0".toHexByteArray())
            gasPrice = ByteString.copyFrom("0x09c7652400".toHexByteArray())
            gasLimit = ByteString.copyFrom("0x0130B9".toHexByteArray())
            transaction = Ethereum.Transaction.newBuilder().apply {
                erc20Transfer = Ethereum.Transaction.ERC20Transfer.newBuilder().apply {
                    to = "0x5322b34c88ed0691971bf52a7047448f0f4efc84"
                    amount = ByteString.copyFrom("0x1bc16d674ec80000".toHexByteArray())       
                }.build()
            }.build()
        }

        val output = AnySigner.sign(signingInput.build(), ETHEREUM, SigningOutput.parser())

        assertEquals(Numeric.toHexString(output.encoded.toByteArray()), "0xf8aa808509c7652400830130b9946b175474e89094c44da98b954eedeac495271d0f80b844a9059cbb0000000000000000000000005322b34c88ed0691971bf52a7047448f0f4efc840000000000000000000000000000000000000000000000001bc16d674ec8000025a0724c62ad4fbf47346b02de06e603e013f26f26b56fdc0be7ba3d6273401d98cea0032131cae15da7ddcda66963e8bef51ca0d9962bfef0547d3f02597a4a58c931")
        assertEquals(Numeric.toHexString(output.data.toByteArray()), "0xa9059cbb0000000000000000000000005322b34c88ed0691971bf52a7047448f0f4efc840000000000000000000000000000000000000000000000001bc16d674ec80000")
    }

    @Test
    fun testEthereumERC20_1559_Signing() {
        val signingInput = Ethereum.SigningInput.newBuilder()
        signingInput.apply {
            privateKey = ByteString.copyFrom(PrivateKey("0x608dcb1742bb3fb7aec002074e3420e4fab7d00cced79ccdac53ed5b27138151".toHexByteArray()).data())
            toAddress = "0x6b175474e89094c44da98b954eedeac495271d0f" // DAI
            chainId = ByteString.copyFrom("0x1".toHexByteArray())
            nonce = ByteString.copyFrom("0x0".toHexByteArray())
            maxInclusionFeePerGas = ByteString.copyFrom("0x77359400".toHexByteArray()) // 2000000000
            maxFeePerGas = ByteString.copyFrom("0xB2D05E00".toHexByteArray()) // 3000000000
            gasLimit = ByteString.copyFrom("0x0130B9".toHexByteArray())
            transaction = Ethereum.Transaction.newBuilder().apply {
                erc20Transfer = Ethereum.Transaction.ERC20Transfer.newBuilder().apply {
                    to = "0x5322b34c88ed0691971bf52a7047448f0f4efc84"
                    amount = ByteString.copyFrom("0x1bc16d674ec80000".toHexByteArray())       
                }.build()
            }.build()
        }

        val output = AnySigner.sign(signingInput.build(), ETHEREUM, SigningOutput.parser())

        assertEquals(Numeric.toHexString(output.encoded.toByteArray()), "0x02f8b00180847735940084b2d05e00830130b9946b175474e89094c44da98b954eedeac495271d0f80b844a9059cbb0000000000000000000000005322b34c88ed0691971bf52a7047448f0f4efc840000000000000000000000000000000000000000000000001bc16d674ec80000c080a0adfcfdf98d4ed35a8967a0c1d78b42adb7c5d831cf5a3272654ec8f8bcd7be2ea011641e065684f6aa476f4fd250aa46cd0b44eccdb0a6e1650d658d1998684cdf")
    }

    @Test
    fun testEthereumERC721Signing() {
        val signingInput = Ethereum.SigningInput.newBuilder()
        signingInput.apply {
            privateKey = ByteString.copyFrom(PrivateKey("0x608dcb1742bb3fb7aec002074e3420e4fab7d00cced79ccdac53ed5b27138151".toHexByteArray()).data())
            toAddress = "0x0d8c864DA1985525e0af0acBEEF6562881827bd5"
            chainId = ByteString.copyFrom("0x1".toHexByteArray())
            nonce = ByteString.copyFrom("0x02de".toHexByteArray())
            gasPrice = ByteString.copyFrom("0x22ecb25c00".toHexByteArray()) // 150 Gwei
            gasLimit = ByteString.copyFrom("0x0130b9".toHexByteArray())
            transaction = Ethereum.Transaction.newBuilder().apply {
                erc721Transfer = Ethereum.Transaction.ERC721Transfer.newBuilder().apply {
                    from = "0x7d8bf18C7cE84b3E175b339c4Ca93aEd1dD166F1"
                    to = "0x47331175b23C2f067204B506CA1501c26731C990"
                    tokenId = ByteString.copyFrom("0x0fd8".toHexByteArray())
                }.build()
            }.build()
        }

        val output = AnySigner.sign(signingInput.build(), ETHEREUM, SigningOutput.parser())

        assertEquals(Numeric.toHexString(output.encoded.toByteArray()), "0xf8cc8202de8522ecb25c00830130b9940d8c864da1985525e0af0acbeef6562881827bd580b86423b872dd0000000000000000000000007d8bf18c7ce84b3e175b339c4ca93aed1dd166f100000000000000000000000047331175b23c2f067204b506ca1501c26731c9900000000000000000000000000000000000000000000000000000000000000fd825a04c5d8242a8c2db1cfa352a3486dd85c82824e01b9bcf0ce4170fcd2329fb7bcaa02d85ab09e750a73fd4dd26b142830ada1e991f8474795b43d96d93e65caaefe7")
        assertEquals(Numeric.toHexString(output.data.toByteArray()), "0x23b872dd0000000000000000000000007d8bf18c7ce84b3e175b339c4ca93aed1dd166f100000000000000000000000047331175b23c2f067204b506ca1501c26731c9900000000000000000000000000000000000000000000000000000000000000fd8")
    }

    @Test
    fun testEthereumERC1155Signing() {
        val signingInput = Ethereum.SigningInput.newBuilder()
        signingInput.apply {
            privateKey = ByteString.copyFrom(PrivateKey("0x608dcb1742bb3fb7aec002074e3420e4fab7d00cced79ccdac53ed5b27138151".toHexByteArray()).data())
            toAddress = "0x4e45e92ed38f885d39a733c14f1817217a89d425" // contract
            chainId = ByteString.copyFrom("0x01".toHexByteArray())
            nonce = ByteString.copyFrom("0x00".toHexByteArray())
            gasPrice = ByteString.copyFrom("0x09C7652400".toHexByteArray()) // 42000000000
            gasLimit = ByteString.copyFrom("0x0130b9".toHexByteArray()) // 78009
            transaction = Ethereum.Transaction.newBuilder().apply {
                erc1155Transfer = Ethereum.Transaction.ERC1155Transfer.newBuilder().apply {
                    from = "0x718046867b5b1782379a14eA4fc0c9b724DA94Fc"
                    to = "0x5322b34c88ed0691971bf52a7047448f0f4efc84"
                    tokenId = ByteString.copyFrom("0x23c47ee5".toHexByteArray())
                    value = ByteString.copyFrom("0x1BC16D674EC80000".toHexByteArray()) // 2000000000000000000
                    data = ByteString.copyFrom("0x01020304".toHexByteArray())
                }.build()
            }.build()
        }

        val output = AnySigner.sign(signingInput.build(), ETHEREUM, SigningOutput.parser())

        assertEquals(Numeric.toHexString(output.encoded.toByteArray()), "0xf9014a808509c7652400830130b9944e45e92ed38f885d39a733c14f1817217a89d42580b8e4f242432a000000000000000000000000718046867b5b1782379a14ea4fc0c9b724da94fc0000000000000000000000005322b34c88ed0691971bf52a7047448f0f4efc840000000000000000000000000000000000000000000000000000000023c47ee50000000000000000000000000000000000000000000000001bc16d674ec8000000000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000000000004010203040000000000000000000000000000000000000000000000000000000026a010315488201ac801ce346bffd1570de147615462d7e7db3cf08cf558465c6b79a06643943b24593bc3904a9fda63bb169881730994c973ab80f07d66a698064573")
        assertEquals(Numeric.toHexString(output.data.toByteArray()), "0xf242432a000000000000000000000000718046867b5b1782379a14ea4fc0c9b724da94fc0000000000000000000000005322b34c88ed0691971bf52a7047448f0f4efc840000000000000000000000000000000000000000000000000000000023c47ee50000000000000000000000000000000000000000000000001bc16d674ec8000000000000000000000000000000000000000000000000000000000000000000a000000000000000000000000000000000000000000000000000000000000000040102030400000000000000000000000000000000000000000000000000000000")
    }

    @Test
    fun testSignJSON() {
        val json = """
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
        val key = "17209af590a86462395d5881e60d11c7fa7d482cfb02b5a01b93c2eeef243543".toHexByteArray()
        val result = AnySigner.signJSON(json, key, CoinType.ETHEREUM.value())

        assertEquals("f86a8084d693a400825208947d8bf18c7ce84b3e175b339c4ca93aed1dd166f1870348bca5a160008025a0fe5802b49e04c6b1705088310e133605ed8b549811a18968ad409ea02ad79f21a05bf845646fb1e1b9365f63a7fd5eb5e984094e3ed35c3bed7361aebbcbf41f10", result)
    }
}
