// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

import XCTest
import WalletCore

class CoinTypeTests: XCTestCase {

    func testCoinType() {
        XCTAssertEqual(CoinType.bitcoin.rawValue, 0)
        XCTAssertEqual(CoinType.litecoin.rawValue, 2)
        XCTAssertEqual(CoinType.tron.rawValue, 195)
        XCTAssertEqual(CoinType.ethereum.rawValue, 60)
        XCTAssertEqual(CoinType.thunderToken.rawValue, 1001)
        XCTAssertEqual(CoinType.wanchain.rawValue, 5718350)
        XCTAssertEqual(CoinType.callisto.rawValue, 820)
        XCTAssertEqual(CoinType.ethereumClassic.rawValue, 61)
        XCTAssertEqual(CoinType.goChain.rawValue, 6060)
        XCTAssertEqual(CoinType.ontology.rawValue, 1024)
        XCTAssertEqual(CoinType.poanetwork.rawValue, 178)
        XCTAssertEqual(CoinType.veChain.rawValue, 818)
        XCTAssertEqual(CoinType.icon.rawValue, 74)
        XCTAssertEqual(CoinType.tomoChain.rawValue, 889)
        XCTAssertEqual(CoinType.tezos.rawValue, 1729)
        XCTAssertEqual(CoinType.qtum.rawValue, 2301)
        XCTAssertEqual(CoinType.nebulas.rawValue, 2718)
        XCTAssertEqual(CoinType.avalancheXChain.rawValue, 9000)
        XCTAssertEqual(CoinType.avalancheCChain.rawValue, 10009000)
    }
}
