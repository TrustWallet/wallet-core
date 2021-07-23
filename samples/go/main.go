package main

// #cgo CFLAGS: -I../../include
// #cgo LDFLAGS: -L../../build -L../../build/trezor-crypto -lTrustWalletCore -lprotobuf -lTrezorCrypto -lc++ -lm
// #include <TrustWalletCore/TWHDWallet.h>
// #include <TrustWalletCore/TWPrivateKey.h>
// #include <TrustWalletCore/TWPublicKey.h>
// #include <TrustWalletCore/TWBitcoinScript.h>
// #include <TrustWalletCore/TWAnySigner.h>
import "C"

import (
	"encoding/hex"
	"fmt"
	"tw/protos/bitcoin"
	"tw/types"

	"github.com/golang/protobuf/proto"
)

func main() {
	fmt.Println("==> calling wallet core from go")
	str := types.TWStringCreateWithGoString("confirm bleak useless tail chalk destroy horn step bulb genuine attract split")
	emtpy := types.TWStringCreateWithGoString("")
	defer C.TWStringDelete(str)
	defer C.TWStringDelete(emtpy)

	fmt.Println("==> mnemonic is valid: ", C.TWMnemonicIsValid(str))

	wallet := C.TWHDWalletCreateWithMnemonic(str, emtpy)
	defer C.TWHDWalletDelete(wallet)

	key := C.TWHDWalletGetKeyForCoin(wallet, C.TWCoinTypeBitcoin)
	keyData := C.TWPrivateKeyData(key)
	defer C.TWDataDelete(keyData)

	fmt.Println("<== bitcoin private key: ", types.TWDataHexString(keyData))

	pubKey, _ := hex.DecodeString("0288be7586c41a0498c1f931a0aaf08c15811ee2651a5fe0fa213167dcaba59ae8")
	pubKeyData := types.TWDataCreateWithGoBytes(pubKey)
	defer C.TWDataDelete(pubKeyData)
	fmt.Println("==> bitcoin public key is valid: ", C.TWPublicKeyIsValid(pubKeyData, C.TWPublicKeyTypeSECP256k1))

	address := C.TWHDWalletGetAddressForCoin(wallet, C.TWCoinTypeBitcoin)
	defer C.TWStringDelete(address)
	fmt.Println("<== bitcoin address: ", types.TWStringGoString(address))

	script := C.TWBitcoinScriptLockScriptForAddress(address, C.TWCoinTypeBitcoin)
	scriptData := C.TWBitcoinScriptData(script)
	defer C.TWBitcoinScriptDelete(script)
	defer C.TWDataDelete(scriptData)
	fmt.Println("<== bitcoin address lock script: ", types.TWDataHexString(scriptData))

	utxoHash, _ := hex.DecodeString("fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f")

	utxo := bitcoin.UnspentTransaction{
		OutPoint: &bitcoin.OutPoint{
			Hash:     utxoHash,
			Index:    0,
			Sequence: 4294967295,
		},
		Amount: 625000000,
		Script: types.TWDataGoBytes(scriptData),
	}

	input := bitcoin.SigningInput{
		HashType:      1, // TWBitcoinSigHashTypeAll
		Amount:        1000000,
		ByteFee:       1,
		ToAddress:     "1Bp9U1ogV3A14FMvKbRJms7ctyso4Z4Tcx",
		ChangeAddress: "1FQc5LdgGHMHEN9nwkjmz6tWkxhPpxBvBU",
		PrivateKey:    [][]byte{types.TWDataGoBytes(keyData)},
		Utxo:          []*bitcoin.UnspentTransaction{&utxo},
		CoinType:      0, // TWCoinTypeBitcoin
	}

	inputBytes, _ := proto.Marshal(&input)
	inputData := types.TWDataCreateWithGoBytes(inputBytes)
	defer C.TWDataDelete(inputData)

	outputData := C.TWAnySignerSign(inputData, C.TWCoinTypeBitcoin)
	defer C.TWDataDelete(outputData)

	var output bitcoin.SigningOutput
	_ = proto.Unmarshal(types.TWDataGoBytes(outputData), &output)
	fmt.Println("<== bitcoin signed tx: ", hex.EncodeToString(output.Encoded))
}
