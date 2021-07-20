package com.trustwallet.core.app.blockchains

import wallet.core.jni.CoinType
import wallet.core.jni.Curve
import wallet.core.jni.Purpose
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Test


class TestCoinType {
    init {
        System.loadLibrary("TrustWalletCore");
    }

    @Test
    fun testCoinType() {
        assertEquals(CoinType.BITCOIN.value(), 0)
        assertEquals(CoinType.LITECOIN.value(), 2)
        assertEquals(CoinType.TRON.value(), 195)
        assertEquals(CoinType.ETHEREUM.value(), 60)
        assertEquals(CoinType.THUNDERTOKEN.value(), 1001)
        assertEquals(CoinType.WANCHAIN.value(), 5718350)
        assertEquals(CoinType.CALLISTO.value(), 820)
        assertEquals(CoinType.ETHEREUMCLASSIC.value(), 61)
        assertEquals(CoinType.GOCHAIN.value(), 6060)
        assertEquals(CoinType.ONTOLOGY.value(), 1024)
        assertEquals(CoinType.POANETWORK.value(), 178)
        assertEquals(CoinType.VECHAIN.value(), 818)
        assertEquals(CoinType.ICON.value(), 74)
        assertEquals(CoinType.TOMOCHAIN.value(), 889)
        assertEquals(CoinType.TEZOS.value(), 1729)
        assertEquals(CoinType.QTUM.value(), 2301)
        assertEquals(CoinType.NEBULAS.value(), 2718)
        assertEquals(CoinType.AVALANCHEXCHAIN.value(), 9000)
        assertEquals(CoinType.AVALANCHECCHAIN.value(), 10009000)
   }

    @Test
    fun testCoinPurpose() {
        assertEquals(Purpose.BIP84, CoinType.BITCOIN.purpose())
    }

    @Test
    fun testCoinCurve() {
        assertEquals(Curve.SECP256K1, CoinType.BITCOIN.curve())
    }
}
