package com.trustwallet.core.app.blockchains

import kotlinx.coroutines.*
import org.junit.Assert.assertEquals
import org.junit.Test
import wallet.core.jni.CoinType
import wallet.core.jni.CoinType.*
import wallet.core.jni.HDWallet

class CoinAddressDerivationTests {

    init {
        System.loadLibrary("TrustWalletCore")
    }

    @Test
    fun testDeriveAddressesFromPhrase() = runBlocking {
        val wallet = HDWallet("shoot island position soft burden budget tooth cruel issue economy destroy above", "")

        val scope = CoroutineScope(Dispatchers.IO)
        val jobs = mutableListOf<Deferred<Unit>>()
        for (i in 0..4) {
            CoinType.values().forEach { coin ->
                val job = scope.async {
                    val privateKey = wallet.getKeyForCoin(coin)
                    val address = coin.deriveAddress(privateKey)
                    runDerivationChecks(coin, address)
                }
                jobs.add(job)
            }
        }
        jobs.forEach { it.await() }
    }

    private fun runDerivationChecks(coin: CoinType, address: String?) = when (coin) {
        BINANCE -> assertEquals("bnb12vtaxl9952zm6rwf7v8jerq74pvaf77fcmvzhw", address)
        BITCOIN -> assertEquals("bc1quvuarfksewfeuevuc6tn0kfyptgjvwsvrprk9d", address)
        BITCOINCASH -> assertEquals("bitcoincash:qpzl3jxkzgvfd9flnd26leud5duv795fnv7vuaha70", address)
        BITCOINGOLD -> assertEquals("btg1qwz9sed0k4neu6ycrudzkca6cnqe3zweq35hvtg", address)
        CALLISTO -> assertEquals("0x3E6FFC80745E6669135a76F4A7ce6BCF02436e04", address)
        DASH -> assertEquals("XqHiz8EXYbTAtBEYs4pWTHh7ipEDQcNQeT", address)
        DIGIBYTE -> assertEquals("dgb1qtjgmerfqwdffyf8ghcrkgy52cghsqptynmyswu", address)
        ETHEREUM, SMARTCHAIN, POLYGON, AVALANCHECCHAIN -> assertEquals("0x8f348F300873Fd5DA36950B2aC75a26584584feE", address)
        ETHEREUMCLASSIC -> assertEquals("0x078bA3228F3E6C08bEEac9A005de0b7e7089aD1c", address)
        GOCHAIN -> assertEquals("0x5940ce4A14210d4Ccd0ac206CE92F21828016aC2", address)
        GROESTLCOIN -> assertEquals("grs1qexwmshts5pdpeqglkl39zyl6693tmfwp0cue4j", address)
        ICON -> assertEquals("hx18b380b53c23dc4ee9f6666bc20d1be02f3fe106", address)
        LITECOIN -> assertEquals("ltc1qhd8fxxp2dx3vsmpac43z6ev0kllm4n53t5sk0u", address)
        ONTOLOGY -> assertEquals("AHKTnybvnWo3TeY8uvNXekvYxMrXogUjeT", address)
        POANETWORK -> assertEquals("0xe8a3e8bE17E172B6926130eAfB521e9D2849aca9", address)
        XRP -> assertEquals("rPwE3gChNKtZ1mhH3Ko8YFGqKmGRWLWXV3", address)
        TEZOS -> assertEquals("tz1acnY9VbMagps26Kj3RfoGRWD9nYG5qaRX", address)
        THUNDERTOKEN -> assertEquals("0x4b92b3ED6d8b24575Bf5ce4C6a86ED261DA0C8d7", address)
        TOMOCHAIN -> assertEquals("0xC74b6D8897cBa9A4b659d43fEF73C9cA852cE424", address)
        TRON -> assertEquals("TQ5NMqJjhpQGK7YJbESKtNCo86PJ89ujio", address)
        VECHAIN -> assertEquals("0x1a553275dF34195eAf23942CB7328AcF9d48c160", address)
        WANCHAIN -> assertEquals("0xD5ca90b928279FE5D06144136a25DeD90127aC15", address)
        ZCASH -> assertEquals("t1YYnByMzdGhQv3W3rnjHMrJs6HH4Y231gy", address)
        ZCOIN -> assertEquals("aEd5XFChyXobvEics2ppAqgK3Bgusjxtik", address)
        NIMIQ -> assertEquals("NQ76 7AVR EHDA N05U X7SY XB14 XJU7 8ERV GM6H", address)
        STELLAR -> assertEquals("GA3H6I4C5XUBYGVB66KXR27JV5KS3APSTKRUWOIXZ5MVWZKVTLXWKZ2P", address)
        AION -> assertEquals("0xa0629f34c9ea4757ad0b275628d4d02e3db6c9009ba2ceeba76a5b55fb2ca42e", address)
        NANO -> assertEquals("nano_39gsbcishxn3n7wd17ono4otq5wazwzusqgqigztx73wbrh5jwbdbshfnumc", address)
        NEBULAS -> assertEquals("n1ZVgEidtdseYv9ogmGz69Cz4mbqmHYSNqJ", address)
        NEAR -> assertEquals("0c91f6106ff835c0195d5388565a2d69e25038a7e23d26198f85caf6594117ec", address)
        THETA -> assertEquals("0x0d1fa20c218Fec2f2C55d52aB267940485fa5DA4", address)
        COSMOS -> assertEquals("cosmos142j9u5eaduzd7faumygud6ruhdwme98qsy2ekn", address)
        DECRED -> assertEquals("DsidJiDGceqHTyqiejABy1ZQ3FX4SiWZkYG", address)
        DOGECOIN -> assertEquals("DJRFZNg8jkUtjcpo2zJd92FUAzwRjitw6f", address)
        KIN -> assertEquals("GBL3MT2ICHHM5OJ2QJ44CAHGDK6MLPINVXBKOKLHGBWQDVRWTWQ7U2EA", address)
        VIACOIN -> assertEquals("via1qnmsgjd6cvfprnszdgmyg9kewtjfgqflz67wwhc", address)
        QTUM -> assertEquals("QhceuaTdeCZtcxmVc6yyEDEJ7Riu5gWFoF", address)
        NULS -> assertEquals("NULSd6HgU8MoRnNjBgvJpa9tqvGxYdv5ne4en", address)
        EOS -> assertEquals("EOS6hs8sRvGSzuQtq223zwJipMzqTJpXUVjyvHPvPwBSZWWrJTJkg", address)
        IOTEX -> assertEquals("io1qw9cccecw09q7p5kzyqtuhfhvah2mhfrc84jfk", address)
        ZILLIQA -> assertEquals("zil1mk6pqphhkmaguhalq6n3cq0h38ltcehg0rfmv6", address)
        ZELCASH -> assertEquals("t1UKbRPzL4WN8Rs8aZ8RNiWoD2ftCMHKGUf", address)
        RAVENCOIN -> assertEquals("RHoCwPc2FCQqwToYnSiAb3SrCET4zEHsbS", address)
        WAVES -> assertEquals("3P63vkaHhyE9pPv9EfsjwGKqmZYcCRHys4n", address)
        AETERNITY -> assertEquals("ak_QDHJSfvHG9sDHBobaWt2TAGhuhipYjEqZEH34bWugpJfJc3GN", address)
        TERRA -> assertEquals("terra1rh402g98t7sly8trzqw5cyracntlep6qe3smug", address)
        MONACOIN -> assertEquals("M9xFZzZdZhCDxpx42cM8bQHnLwaeX1aNja", address)
        FIO -> assertEquals("FIO7MN1LuSfFgrbVHmrt9cVa2FYAs857Ppr9dzvEXoD1miKSxm3n3", address)
        HARMONY -> assertEquals("one12fk20wmvgypdkn59n4hq8e3aa5899xfx4vsu09", address)
        SOLANA -> assertEquals("2bUBiBNZyD29gP1oV6de7nxowMLoDBtopMMTGgMvjG5m", address)
        ALGORAND -> assertEquals("JTJWO524JXIHVPGBDWFLJE7XUIA32ECOZOBLF2QP3V5TQBT3NKZSCG67BQ", address)
        KUSAMA -> assertEquals("G9xV2EatmrjRC1FLPexc3ddqNRRzCsAdURU8RFiAAJX6ppY", address)
        POLKADOT -> assertEquals("13nN6BGAoJwd7Nw1XxeBCx5YcBXuYnL94Mh7i3xBprqVSsFk", address)
        KAVA -> assertEquals("kava1drpa0x9ptz0fql3frv562rcrhj2nstuz3pas87", address)
        CARDANO -> assertEquals("addr1snpa4z7ntyfszv7ckquprdw75w4qjqh0qmya9jtkpxxlzxghlqyvv7l0yjamh8fxraw06p3ua8sj2g2gv98v4849s43t9g2999kquuu5egnprk", address)
        NEO -> assertEquals("AT6w7PJvwPcSqHvtbNBY2aHPDv12eW5Uuf", address)
        FILECOIN -> assertEquals("f1zzykebxldfcakj5wdb5n3n7priul522fnmjzori", address)
        ELROND -> assertEquals("erd1jfcy8aeru6vlx4fe6h3pc3vlpe2cnnur5zetxdhp879yagq7vqvs8na4f8", address)
        BANDCHAIN -> assertEquals("band1624hqgend0s3d94z68fyka2y5jak6vd7u0l50r", address)
        SMARTCHAINLEGACY -> assertEquals("0x49784f90176D8D9d4A3feCDE7C1373dAAb5b13b8", address)
        AVALANCHEXCHAIN -> assertEquals("X-avax1x8nslqfqha3aywycds3jfnykp3khffrk2scya2", address)
        OASIS -> assertEquals("oasis1qzcpavvmuw280dk0kd4lxjhtpf0u3ll27yf7sqps", address)
        THORCHAIN -> assertEquals("thor1c8jd7ad9pcw4k3wkuqlkz4auv95mldr2kyhc65", address)
        BLUZELLE -> assertEquals("bluzelle1xccvees6ev4wm2r49rc6ptulsdxa8x8jfpmund", address)
    }
}
