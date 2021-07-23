// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

import Cocoa
import WalletCore

class ViewController: NSViewController {

    override func viewDidLoad() {
        super.viewDidLoad()

        // 'Import' a wallet
        let wallet = HDWallet(mnemonic: "ripple scissors kick mammal hire column oak again sun offer wealth tomorrow wagon turn fatal", passphrase: "")!
        print("Mnemonic: ", wallet.mnemonic)

        // Ethereum example
        var coin: CoinType = .ethereum
        // Get the default address
        let addressEth = wallet.getAddressForCoin(coin: coin)
        print("Default ETH address: ", addressEth)

        // Signing a transaction (using AnySigner)
        let secretPrivateKeyEth = wallet.getKeyForCoin(coin: coin)
        let dummyReceiverAddress = "0xC37054b3b48C3317082E7ba872d7753D13da4986"
        let signerInput = EthereumSigningInput.with {
            $0.chainID = Data(hexString: "01")!
            $0.gasPrice = Data(hexString: "d693a400")! // decimal 3600000000
            $0.gasLimit = Data(hexString: "5208")! // decimal 21000
            $0.toAddress = dummyReceiverAddress

            $0.transaction = EthereumTransaction.with {
                $0.transfer = EthereumTransaction.Transfer.with {
                    $0.amount = Data(hexString: "0348bca5a16000")!
                }
            }
            $0.privateKey = secretPrivateKeyEth.data
        }
        let outputEth: EthereumSigningOutput = AnySigner.sign(input: signerInput, coin: coin)
        print("Signed transaction:")
        print(" data:   ", outputEth.encoded.hexString)
        
        // Bitcoin example
        coin = .bitcoin
        // Get the default address
        let addressBtc = wallet.getAddressForCoin(coin: coin)
        print("Default BTC address: ", addressBtc)

        // Build a transaction
        let toAddress = "1Bp9U1ogV3A14FMvKbRJms7ctyso4Z4Tcx"
        let changeAddress = "1FQc5LdgGHMHEN9nwkjmz6tWkxhPpxBvBU"
        let secretPrivateKeyBtc = wallet.getKeyForCoin(coin: coin)
        let outPoint = BitcoinOutPoint.with {
            $0.hash = Data(hexString: "050d00e2e18ef13969606f1ceee290d3f49bd940684ce39898159352952b8ce2")!
            $0.index = 2
        }
        let utxo = BitcoinUnspentTransaction.with {
            $0.amount = 5151
            $0.outPoint = outPoint
            $0.script = BitcoinScript.lockScriptForAddress(address: addressBtc, coin: coin).data
        }
        var input = BitcoinSigningInput.with {
            $0.amount = 600
            $0.hashType = TWBitcoinSigHashTypeAll.rawValue
            $0.toAddress = toAddress
            $0.changeAddress = changeAddress
            $0.byteFee = 2
            $0.coinType = coin.rawValue
        }
        input.utxo.append(utxo)
        input.privateKey.append(secretPrivateKeyBtc.data)

        // Calculate fee (plan a tranaction)
        let plan: BitcoinTransactionPlan = AnySigner.plan(input: input, coin: coin)
        print("Planned fee:  ", plan.fee, "amount:", plan.amount, "avail_amount:", plan.availableAmount, "change:", plan.change)

        // Set the precomputed plan
        input.plan = plan
        input.amount = plan.amount

        let outputBtc: BitcoinSigningOutput = AnySigner.sign(input: input, coin: coin)

        print("Signed transaction:")
        print(" data:   ", outputBtc.encoded.hexString)
    }
}
