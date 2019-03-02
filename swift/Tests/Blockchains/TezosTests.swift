// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

import TrustWalletCore
import XCTest

class TezosTests: XCTestCase {
  public let tezos = Tezos()

  public func testCoinType() {
    XCTAssertEqual(tezos.coinType, .tezos)
  }

  public func testAddressFromString_validAddress() {
    let validAddressString = "tz1eZwq8b5cvE2bPKokatLkVMzkxz24z3Don"
    let address = tezos.address(string: validAddressString)

    XCTAssertNotNil(address)
    XCTAssertEqual(address?.description, validAddressString)
  }

  public func testAddressFromString_invalidAddress() {
    let invalidAddressString = "tz1eZwq8b5cvE2bPKokatLkVMzkxz24z3AAA" // Invalid checksum
    let address = tezos.address(string: invalidAddressString)

    XCTAssertNil(address)
  }

  public func testAddressFromData_validAddress() {
    let validAddressString = "tz1eZwq8b5cvE2bPKokatLkVMzkxz24z3Don"
    guard let validAddressData = validAddressString.data(using: .utf8) else {
      XCTFail("Couldn't decode address to data")
      return
    }

    let address = tezos.address(data: validAddressData)

    XCTAssertNotNil(address)
    XCTAssertEqual(address?.data, Base58.decode(string: validAddressString))
  }

  public func testAddressFromData_invalidAddress() {
    let invalidAddressString = "tz1eZwq8b5cvE2bPKokatLkVMzkxz24z3AAA" // Invalid checksum
    guard let invalidAddressData = invalidAddressString.data(using: .utf8) else {
      XCTFail("Couldn't decode address to data")
      return
    }

    let address = tezos.address(data: invalidAddressData)

    XCTAssertNil(address)
  }
}
