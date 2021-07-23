package com.trustwallet.core.app.utils

import com.trustwallet.core.app.utils.Numeric
import wallet.core.jni.CoinType
import wallet.core.jni.Curve
import wallet.core.jni.HDVersion
import wallet.core.jni.HDWallet
import wallet.core.jni.Mnemonic
import wallet.core.jni.Purpose
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test


class TestHDWallet {
    init {
        System.loadLibrary("TrustWalletCore");
    }

    val words =
        "ripple scissors kick mammal hire column oak again sun offer wealth tomorrow wagon turn fatal"
    val password = "TREZOR"

    @Test
    fun testCreateFromMnemonic() {
        val hd = HDWallet(words, password)
        assertEquals(hd.mnemonic(), words)
        assertEquals(Numeric.toHexString(hd.entropy()), "0xba5821e8c356c05ba5f025d9532fe0f21f65d594")
        assertEquals(Numeric.toHexString(hd.seed()), "0x7ae6f661157bda6492f6162701e570097fc726b6235011ea5ad09bf04986731ed4d92bc43cbdee047b60ea0dd1b1fa4274377c9bf5bd14ab1982c272d8076f29")
    }

    @Test
    fun testCreateFromMnemonicNoPass() {
        val hd = HDWallet(words, "")
        assertEquals(Numeric.toHexString(hd.seed()), "0x354c22aedb9a37407adc61f657a6f00d10ed125efa360215f36c6919abd94d6dbc193a5f9c495e21ee74118661e327e84a5f5f11fa373ec33b80897d4697557d")
    }

    @Test
    fun testCreateFromMnemonicInvalid() {
        var exception = false
        try {
            val hd = HDWallet("THIS IS AN INVALID MNEMONIC", "")
        } catch (ex: Exception) {
            exception = true
        }
        assertTrue(exception)
    }

    @Test
    fun testGenerate() {
        val hd = HDWallet(128, "")
        assertTrue(Mnemonic.isValid(hd.mnemonic()))
    }

    @Test
    fun tesCreateFromEntropy() {
        val hd = HDWallet(Numeric.hexStringToByteArray("ba5821e8c356c05ba5f025d9532fe0f21f65d594"), "TREZOR")
        assertEquals(hd.mnemonic(), words)
        assertEquals(Numeric.toHexString(hd.entropy()), "0xba5821e8c356c05ba5f025d9532fe0f21f65d594")
    }

    @Test
    fun testDerive() {
        val wallet = HDWallet(words, password)
        val key0 = wallet.getKeyForCoin(CoinType.ETHEREUM)

        val publicKey0 = key0.getPublicKeySecp256k1(false)
        val publicKey0Data = Numeric.toHexString(publicKey0.data())

        assertEquals(
            publicKey0Data, "0x0414acbe5a06c68210fcbb77763f9612e45a526990aeb69d692d705f276f558a5ae68268e9389bb099ed5ac84d8d6861110f63644f6e5b447e3f86b4bab5dee011")
    }

    @Test
    fun testDeriveBitcoin() {
        val wallet = HDWallet(words, password)
        val key = wallet.getKeyForCoin(CoinType.BITCOIN)
        val publicKey = key.getPublicKeySecp256k1(false)
        val publicKeyData = publicKey.data()

        assertEquals(
            Numeric.toHexString(publicKeyData), "0x047ea5dff03f677502c4a1d73c5ac897200e56b155e876774c8fba0cc22f80b9414ec07cda7b1c9a84c2e04ea2746c21afacc5e91b47427c453c3f1a4a3e983ce5")
    }

    @Test
    fun testExtendedKeys() {
        val words =
            "abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon about"
        val wallet = HDWallet(words, "")

        val xprv = wallet.getExtendedPrivateKey(Purpose.BIP44, CoinType.BITCOIN, HDVersion.XPRV)
        val xpub = wallet.getExtendedPublicKey(Purpose.BIP44, CoinType.BITCOIN, HDVersion.XPUB)

        assertEquals(xprv, "xprv9xpXFhFpqdQK3TmytPBqXtGSwS3DLjojFhTGht8gwAAii8py5X6pxeBnQ6ehJiyJ6nDjWGJfZ95WxByFXVkDxHXrqu53WCRGypk2ttuqncb")
        assertEquals(xpub, "xpub6BosfCnifzxcFwrSzQiqu2DBVTshkCXacvNsWGYJVVhhawA7d4R5WSWGFNbi8Aw6ZRc1brxMyWMzG3DSSSSoekkudhUd9yLb6qx39T9nMdj")

        val yprv = wallet.getExtendedPrivateKey(Purpose.BIP49, CoinType.BITCOIN, HDVersion.YPRV)
        val ypub = wallet.getExtendedPublicKey(Purpose.BIP49, CoinType.BITCOIN, HDVersion.YPUB)
        assertEquals(yprv, "yprvAHwhK6RbpuS3dgCYHM5jc2ZvEKd7Bi61u9FVhYMpgMSuZS613T1xxQeKTffhrHY79hZ5PsskBjcc6C2V7DrnsMsNaGDaWev3GLRQRgV7hxF");
        assertEquals(ypub, "ypub6Ww3ibxVfGzLrAH1PNcjyAWenMTbbAosGNB6VvmSEgytSER9azLDWCxoJwW7Ke7icmizBMXrzBx9979FfaHxHcrArf3zbeJJJUZPf663zsP");

        val zprv = wallet.getExtendedPrivateKey(Purpose.BIP84, CoinType.BITCOIN, HDVersion.ZPRV)
        val zpub = wallet.getExtendedPublicKey(Purpose.BIP84, CoinType.BITCOIN, HDVersion.ZPUB)
        assertEquals(zprv, "zprvAdG4iTXWBoARxkkzNpNh8r6Qag3irQB8PzEMkAFeTRXxHpbF9z4QgEvBRmfvqWvGp42t42nvgGpNgYSJA9iefm1yYNZKEm7z6qUWCroSQnE");
        assertEquals(zpub, "zpub6rFR7y4Q2AijBEqTUquhVz398htDFrtymD9xYYfG1m4wAcvPhXNfE3EfH1r1ADqtfSdVCToUG868RvUUkgDKf31mGDtKsAYz2oz2AGutZYs");
    }

    @Test
    fun testPublicKeyFromX() {
        val xpub = "xpub6BosfCnifzxcFwrSzQiqu2DBVTshkCXacvNsWGYJVVhhawA7d4R5WSWGFNbi8Aw6ZRc1brxMyWMzG3DSSSSoekkudhUd9yLb6qx39T9nMdj"
        val xpubAddr2 = HDWallet.getPublicKeyFromExtended(xpub, CoinType.BITCOINCASH,"m/44'/145'/0'/0/2")
        val xpubAddr9 = HDWallet.getPublicKeyFromExtended(xpub, CoinType.BITCOINCASH,"m/44'/145'/0'/0/9")

        assertEquals(
            Numeric.toHexString(xpubAddr2.data()),
            "0x0338994349b3a804c44bbec55c2824443ebb9e475dfdad14f4b1a01a97d42751b3"
        )
        assertEquals(
            Numeric.toHexString(xpubAddr9.data()),
            "0x03786c1d274f2c804ff9a57d8e7289c281d4aef15e17187ad9f9c3722d81a6ae66"
        )
    }

    @Test
    fun testInitHDWallet() {
        val hdWallet2 = HDWallet(160, "")
        assertFalse(hdWallet2.mnemonic().isNullOrBlank())
        val wordList2 = hdWallet2.mnemonic().split(" ")
        Mnemonic.isValid(wordList2.joinToString { " " })

        val hdWallet3 = HDWallet(160, "")
        assertFalse(hdWallet3.mnemonic().isNullOrBlank())
        val wordList3 = hdWallet3.mnemonic().split(" ")
        Mnemonic.isValid(wordList3.joinToString { " " })
    }

    @Test
    fun testInitHDWalletWithPhrase() {
        val hdWallet = HDWallet(words, "")
        assertEquals(hdWallet.mnemonic(), words)
    }
}
