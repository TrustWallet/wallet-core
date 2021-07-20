// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

package com.trustwallet.core.app.blockchains.avalanche

import com.trustwallet.core.app.utils.toHex
import com.trustwallet.core.app.utils.toHexByteArray
import org.junit.Assert.assertEquals
import org.junit.Test
import wallet.core.jni.*

class TestAvalancheAddress {

    init {
        System.loadLibrary("TrustWalletCore")
    }

    @Test
    fun testAddress() {
        val key = PrivateKey("0x98cb077f972feb0481f1d894f272c6a1e3c15e272a1658ff716444f465200070".toHexByteArray())
        val pubkey = key.getPublicKeySecp256k1(false)
        val address = AnyAddress(pubkey, CoinType.AVALANCHEXCHAIN)
        val expected = AnyAddress("X-avax1apmh7wxg3js48fhacfv5y9md9065jxuft30vup", CoinType.AVALANCHEXCHAIN)

        assertEquals(pubkey.data().toHex(), "0x04b33c917f2f6103448d7feb42614037d05928433cb25e78f01a825aa829bb3c27299a9fb02dd081d37876f145b8d3aa47fe9764145a9d90d4f941105d7de85c82")
        assertEquals(address.description(), expected.description())
    }
}
