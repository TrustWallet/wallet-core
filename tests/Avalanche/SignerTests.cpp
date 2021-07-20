// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Avalanche/Signer.h"
#include "Avalanche/Address.h"
#include "HexCoding.h"
#include "PrivateKey.h"
#include "PublicKey.h"
#include "Avalanche/CB58.h"

#include <gtest/gtest.h>

using namespace TW;
using namespace TW::Avalanche;



// This test is modeled after the avalanchejs signer tests
// many example values are likewise modeled after the docs.avax.network tx serialization examples

std::pair<PrivateKey, BaseTransaction> getPrivateKeyAndTransaction();
std::vector<Address> generateAddressesForSigner();

TEST(AvalancheSigner, SignBasicTransaction) {
    auto defaults = getPrivateKeyAndTransaction();
    auto privateKey = defaults.first;
    auto transaction = defaults.second;
    
    Data encodedUnsignedTransaction;
    transaction.encode(encodedUnsignedTransaction);

    ASSERT_EQ(hex(encodedUnsignedTransaction), "0000000000003039d891ad56056d9c01f18f43f58b5c784ad07a4a49cf3d1f11623804b5cba2c6bf00000002dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000700000000000003e8000000000000000000000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29cdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db000000070000000000003039000000000000d43100000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29c00000002f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd15000000020000000300000007f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd1500000002000000030000000700000004deadbeef");
    std::vector<PrivateKey> keyRing = {privateKey};
    auto encodedSignedTransaction = Signer::sign(keyRing, transaction);
    ASSERT_EQ(hex(encodedSignedTransaction), "00000000000000003039d891ad56056d9c01f18f43f58b5c784ad07a4a49cf3d1f11623804b5cba2c6bf00000002dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000700000000000003e8000000000000000000000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29cdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db000000070000000000003039000000000000d43100000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29c00000002f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd15000000020000000300000007f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd1500000002000000030000000700000004deadbeef00000002000000090000000244ef527f47cab3ed82eb267c27c04869e46531b05db643f5bc97da21148afe161f17634a90f4e22adb810b472062f7e809dde19059fa7048f9972a481fe9390d0044ef527f47cab3ed82eb267c27c04869e46531b05db643f5bc97da21148afe161f17634a90f4e22adb810b472062f7e809dde19059fa7048f9972a481fe9390d00000000090000000244ef527f47cab3ed82eb267c27c04869e46531b05db643f5bc97da21148afe161f17634a90f4e22adb810b472062f7e809dde19059fa7048f9972a481fe9390d0044ef527f47cab3ed82eb267c27c04869e46531b05db643f5bc97da21148afe161f17634a90f4e22adb810b472062f7e809dde19059fa7048f9972a481fe9390d00");
}

TEST(AvalancheSigner, SignCreateAssetTransaction) {
    auto defaults = getPrivateKeyAndTransaction();
    auto privateKey = defaults.first;
    auto baseTransaction = defaults.second;
    
    auto arbitraryAddresses = generateAddressesForSigner();
    // build some arbitrary outputs for this test
    // fill outputs with arbitrary transactionoutputs
    // although the arbitrary data is the same in both cases, must construct it twice otherwise undefined behavior occurs 
    std::vector<std::unique_ptr<TransactionOutput>> arbitraryOutputsOne;
    arbitraryOutputsOne.push_back(std::make_unique<SECP256k1TransferOutput>(12345, 54321, 333, arbitraryAddresses));
    arbitraryOutputsOne.push_back(std::make_unique<SECP256k1TransferOutput>(54321, 54321, 333, arbitraryAddresses));
    arbitraryOutputsOne.push_back(std::make_unique<SECP256k1TransferOutput>(111, 111, 111, arbitraryAddresses));
    auto stateOne = InitialState(InitialState::FeatureExtension::SECP256k1, std::move(arbitraryOutputsOne));
    std::vector<std::unique_ptr<TransactionOutput>> arbitraryOutputsTwo;
    arbitraryOutputsTwo.push_back(std::make_unique<SECP256k1TransferOutput>(12345, 54321, 333, arbitraryAddresses));
    arbitraryOutputsTwo.push_back(std::make_unique<SECP256k1TransferOutput>(54321, 54321, 333, arbitraryAddresses));
    arbitraryOutputsTwo.push_back(std::make_unique<SECP256k1TransferOutput>(111, 111, 111, arbitraryAddresses));
    auto stateTwo = InitialState(InitialState::FeatureExtension::SECP256k1, std::move(arbitraryOutputsTwo));
    std::vector<InitialState> states = {stateOne, stateTwo};
    std::string name = "testcoin";
    std::string symbol = "test";
    uint8_t denomination = 1;
    auto transaction = UnsignedCreateAssetTransaction(baseTransaction, name, symbol, denomination, states);
    Data encodedUnsignedTransaction;
    transaction.encode(encodedUnsignedTransaction);
    ASSERT_EQ(hex(encodedUnsignedTransaction), "0000000100003039d891ad56056d9c01f18f43f58b5c784ad07a4a49cf3d1f11623804b5cba2c6bf00000002dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000700000000000003e8000000000000000000000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29cdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db000000070000000000003039000000000000d43100000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29c00000002f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd15000000020000000300000007f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd1500000002000000030000000700000004deadbeef0874657374636f696e04746573740100000002000000000000000300000007000000000000006f000000000000006f0000006f00000003355a58440e3647f394aff711aca083e5a5181426e8777f38c88ca153a6fdc25942176d2bf5491b89ee93c2aaa8b4b93d45ce77237e9f65192be9a39b000000070000000000003039000000000000d4310000014d00000003355a58440e3647f394aff711aca083e5a5181426e8777f38c88ca153a6fdc25942176d2bf5491b89ee93c2aaa8b4b93d45ce77237e9f65192be9a39b00000007000000000000d431000000000000d4310000014d00000003355a58440e3647f394aff711aca083e5a5181426e8777f38c88ca153a6fdc25942176d2bf5491b89ee93c2aaa8b4b93d45ce77237e9f65192be9a39b000000000000000300000007000000000000006f000000000000006f0000006f00000003355a58440e3647f394aff711aca083e5a5181426e8777f38c88ca153a6fdc25942176d2bf5491b89ee93c2aaa8b4b93d45ce77237e9f65192be9a39b000000070000000000003039000000000000d4310000014d00000003355a58440e3647f394aff711aca083e5a5181426e8777f38c88ca153a6fdc25942176d2bf5491b89ee93c2aaa8b4b93d45ce77237e9f65192be9a39b00000007000000000000d431000000000000d4310000014d00000003355a58440e3647f394aff711aca083e5a5181426e8777f38c88ca153a6fdc25942176d2bf5491b89ee93c2aaa8b4b93d45ce77237e9f65192be9a39b");

    std::vector<PrivateKey> keyRing = {privateKey};
    auto encodedSignedTransaction = Signer::sign(keyRing, transaction);
    ASSERT_EQ(hex(encodedSignedTransaction), "00000000000100003039d891ad56056d9c01f18f43f58b5c784ad07a4a49cf3d1f11623804b5cba2c6bf00000002dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000700000000000003e8000000000000000000000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29cdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db000000070000000000003039000000000000d43100000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29c00000002f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd15000000020000000300000007f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd1500000002000000030000000700000004deadbeef0000000200000009000000021690ee4d10718b83bd43ce9cec43bf91d23b9380755de49d59331f7c25be60eb228ffcc273393597dcd7109af08eb59e9fcc09e256f2c24f04c58dbd6d1f04af011690ee4d10718b83bd43ce9cec43bf91d23b9380755de49d59331f7c25be60eb228ffcc273393597dcd7109af08eb59e9fcc09e256f2c24f04c58dbd6d1f04af0100000009000000021690ee4d10718b83bd43ce9cec43bf91d23b9380755de49d59331f7c25be60eb228ffcc273393597dcd7109af08eb59e9fcc09e256f2c24f04c58dbd6d1f04af011690ee4d10718b83bd43ce9cec43bf91d23b9380755de49d59331f7c25be60eb228ffcc273393597dcd7109af08eb59e9fcc09e256f2c24f04c58dbd6d1f04af01");
}

TEST(AvalancheSigner, SignOperationTransaction) {
    auto defaults = getPrivateKeyAndTransaction();
    auto privateKey = defaults.first;
    auto baseTransaction = defaults.second;
    auto assetID = parse_hex("0xdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db"); 
    auto txid = parse_hex("0xf1e1d1c1b1a191817161514131211101f0e0d0c0b0a090807060504030201000");

    auto utxoIDOne = std::make_pair(txid, uint32_t(1));
    auto utxoIDTwo = std::make_pair(txid, uint32_t(2));
    std::vector<TransferableOp::UTXOID> utxoIDsOne = {utxoIDTwo, utxoIDOne}; // create it backwards to force a sort
    std::vector<uint32_t> addressIndicesOne = {1, 2, 3};
    uint32_t groupIDOne = 1;
    Data payloadOne = {0xfe, 0xed, 0xfe, 0xed};
    std::vector<Output> outputsOne;
    auto outputOne = std::make_tuple(uint64_t(1), uint32_t(2), generateAddressesForSigner());
    outputsOne.push_back(outputOne);
    auto nftOp = std::make_unique<NFTMintOperation>(addressIndicesOne, groupIDOne, payloadOne, outputsOne);
    auto opOne = TransferableOp(assetID, utxoIDsOne, std::move(nftOp));
    
    auto differingTxid = parse_hex("0x0000d1c1b1a191817161514131211101f0e0d0c0b0a090807060504030201000");
    auto utxoIDThree = std::make_pair(differingTxid, uint32_t(3));
    auto utxoIDFour = std::make_pair(txid, uint32_t(3));
    std::vector<TransferableOp::UTXOID> utxoIDsTwo = {utxoIDFour, utxoIDThree}; // create it backwards to force a sort
    std::vector<uint32_t> addressIndicesTwo = {2, 3, 4};
    auto mintOutputAddrs = generateAddressesForSigner();
    auto mintOutput = SECP256k1MintOutput(uint64_t(10), uint32_t(20), mintOutputAddrs);
    auto transferOutputAddrs = generateAddressesForSigner();
    auto transferOutput = SECP256k1TransferOutput(uint64_t(30), uint64_t(40), uint32_t(5), transferOutputAddrs);
    auto secpOp = std::make_unique<SECP256k1MintOperation>(addressIndicesTwo, mintOutput, transferOutput);
    auto opTwo = TransferableOp(assetID, utxoIDsTwo, std::move(secpOp));
    
    auto utxoIDFive = std::make_pair(txid, uint32_t(5));
    auto utxoIDSix = std::make_pair(txid, uint32_t(6));
    std::vector<TransferableOp::UTXOID> utxoIDsThree = {utxoIDSix, utxoIDFive}; // create it backwards to force a sort
    std::vector<uint32_t> addressIndicesThree = {3, 4, 5};
    auto nftTransferAddrs = generateAddressesForSigner();
    auto nftPayload = Data{0xfe, 0xde, 0xfe, 0xde};
    auto nftTransferOut = NFTTransferOutput(uint32_t(1), nftPayload, uint64_t(2), uint32_t(3), nftTransferAddrs);
    auto nftTransferOp = std::make_unique<NFTTransferOperation>(addressIndicesThree, nftTransferOut);
    auto opThree = TransferableOp(assetID, utxoIDsThree, std::move(nftTransferOp));

    std::vector<TransferableOp> ops = {opOne, opTwo, opThree};
    auto transaction = UnsignedOperationTransaction(baseTransaction, ops); 
    Data encodedUnsignedTransaction;
    transaction.encode(encodedUnsignedTransaction);

    ASSERT_EQ(hex(encodedUnsignedTransaction), "0000000200003039d891ad56056d9c01f18f43f58b5c784ad07a4a49cf3d1f11623804b5cba2c6bf00000002dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000700000000000003e8000000000000000000000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29cdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db000000070000000000003039000000000000d43100000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29c00000002f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd15000000020000000300000007f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd1500000002000000030000000700000004deadbeef00000003dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db000000020000d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000003f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000003000000080000000300000002000000030000000400000006000000000000000a0000001400000003355a58440e3647f394aff711aca083e5a5181426e8777f38c88ca153a6fdc25942176d2bf5491b89ee93c2aaa8b4b93d45ce77237e9f65192be9a39b00000007000000000000001e00000000000000280000000500000003355a58440e3647f394aff711aca083e5a5181426e8777f38c88ca153a6fdc25942176d2bf5491b89ee93c2aaa8b4b93d45ce77237e9f65192be9a39bdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db00000002f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000001f1e1d1c1b1a191817161514131211101f0e0d0c0b0a090807060504030201000000000020000000c000000030000000100000002000000030000000100000004feedfeed0000000100000000000000010000000200000003e8777f38c88ca153a6fdc25942176d2bf5491b89355a58440e3647f394aff711aca083e5a5181426ee93c2aaa8b4b93d45ce77237e9f65192be9a39bdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db00000002f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005f1e1d1c1b1a191817161514131211101f0e0d0c0b0a090807060504030201000000000060000000d000000030000000300000004000000050000000100000004fedefede00000000000000020000000300000003355a58440e3647f394aff711aca083e5a5181426e8777f38c88ca153a6fdc25942176d2bf5491b89ee93c2aaa8b4b93d45ce77237e9f65192be9a39b");
    std::vector<PrivateKey> keyRing = {privateKey};
    auto encodedSignedTransaction = Signer::sign(keyRing, transaction);
    ASSERT_EQ(hex(encodedSignedTransaction), "00000000000200003039d891ad56056d9c01f18f43f58b5c784ad07a4a49cf3d1f11623804b5cba2c6bf00000002dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000700000000000003e8000000000000000000000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29cdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db000000070000000000003039000000000000d43100000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29c00000002f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd15000000020000000300000007f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd1500000002000000030000000700000004deadbeef0000000200000009000000028d718dfbf41202d03e495cda3eeb2464dfdc0e7e1e73e3189e5b4752dae1a919360e43aeb3c31c1258c8cc80028bbcd83f15d00aa40dffbcbc9ad35e4f57ac8b008d718dfbf41202d03e495cda3eeb2464dfdc0e7e1e73e3189e5b4752dae1a919360e43aeb3c31c1258c8cc80028bbcd83f15d00aa40dffbcbc9ad35e4f57ac8b0000000009000000028d718dfbf41202d03e495cda3eeb2464dfdc0e7e1e73e3189e5b4752dae1a919360e43aeb3c31c1258c8cc80028bbcd83f15d00aa40dffbcbc9ad35e4f57ac8b008d718dfbf41202d03e495cda3eeb2464dfdc0e7e1e73e3189e5b4752dae1a919360e43aeb3c31c1258c8cc80028bbcd83f15d00aa40dffbcbc9ad35e4f57ac8b00");
}

TEST(AvalancheSigner, SignImportTransaction) {
    auto defaults = getPrivateKeyAndTransaction();
    auto privateKey = defaults.first;
    auto baseTransaction = defaults.second;
    
    auto blockchainID = CB58::avalanche.decode("2eNy1mUFdmaxXNj1eQHUe7Np4gju9sJsEtWQ4MX3ToiNKuADed");
    Data sourceChain(blockchainID.begin(), blockchainID.begin() + BLOCKCHAIN_ID_SIZE); // we just want the first 32 bytes, no checksum

    auto assetID = parse_hex("0xdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db"); 
    auto spendableAddresses = generateAddressesForSigner();

    auto addressesInOne = std::vector<uint32_t>{3, 7};
    auto txidOne = parse_hex("0xf1e1d1c1b1a191817161514131211101f0e0d0c0b0a090807060504030201000");
    auto utxoIndexOne = 5;
    auto coreInputOne = std::make_unique<SECP256k1TransferInput>(123456789, addressesInOne);
    auto inputOne = TransferableInput(txidOne, utxoIndexOne, assetID, std::move(coreInputOne), spendableAddresses);

    auto addressesInTwo = std::vector<uint32_t>{3, 7};
    auto txidTwo = parse_hex("0xf1e1d1c1b1a191817161514131211101f0e0d0c0b0a090807060504030201000");
    auto utxoIndexTwo = 5;
    auto coreInputTwo = std::make_unique<SECP256k1TransferInput>(123456789, addressesInTwo);
    auto inputTwo = TransferableInput(txidTwo, utxoIndexTwo, assetID, std::move(coreInputTwo), spendableAddresses);

    std::vector<TransferableInput> inputs = {inputOne, inputTwo};
    auto transaction = UnsignedImportTransaction(baseTransaction, sourceChain, inputs);
    Data encodedUnsignedTransaction;
    transaction.encode(encodedUnsignedTransaction);

    ASSERT_EQ(hex(encodedUnsignedTransaction), "0000000300003039d891ad56056d9c01f18f43f58b5c784ad07a4a49cf3d1f11623804b5cba2c6bf00000002dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000700000000000003e8000000000000000000000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29cdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db000000070000000000003039000000000000d43100000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29c00000002f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd15000000020000000300000007f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd1500000002000000030000000700000004deadbeefd891ad56056d9c01f18f43f58b5c784ad07a4a49cf3d1f11623804b5cba2c6bf00000002f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd15000000020000000300000007f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd15000000020000000300000007");
    std::vector<PrivateKey> keyRing = {privateKey};
    auto encodedSignedTransaction = Signer::sign(keyRing, transaction);
    ASSERT_EQ(hex(encodedSignedTransaction), "00000000000300003039d891ad56056d9c01f18f43f58b5c784ad07a4a49cf3d1f11623804b5cba2c6bf00000002dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000700000000000003e8000000000000000000000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29cdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db000000070000000000003039000000000000d43100000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29c00000002f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd15000000020000000300000007f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd1500000002000000030000000700000004deadbeef000000020000000900000002f7393f970599a401f43e8bc3082968ae9b4bb01401d9401f83f523fb599c2a9670fa12e7634f6037e39ed3cf0e6e1cda8f89ac696af78245106a55a0fc0aa39a01f7393f970599a401f43e8bc3082968ae9b4bb01401d9401f83f523fb599c2a9670fa12e7634f6037e39ed3cf0e6e1cda8f89ac696af78245106a55a0fc0aa39a010000000900000002f7393f970599a401f43e8bc3082968ae9b4bb01401d9401f83f523fb599c2a9670fa12e7634f6037e39ed3cf0e6e1cda8f89ac696af78245106a55a0fc0aa39a01f7393f970599a401f43e8bc3082968ae9b4bb01401d9401f83f523fb599c2a9670fa12e7634f6037e39ed3cf0e6e1cda8f89ac696af78245106a55a0fc0aa39a01");
}

TEST(AvalancheSigner, SignExportTransaction) {
    auto defaults = getPrivateKeyAndTransaction();
    auto privateKey = defaults.first;
    auto baseTransaction = defaults.second;
    
    auto blockchainID = CB58::avalanche.decode("2eNy1mUFdmaxXNj1eQHUe7Np4gju9sJsEtWQ4MX3ToiNKuADed");
    Data destChain(blockchainID.begin(), blockchainID.begin() + BLOCKCHAIN_ID_SIZE); // we just want the first 32 bytes, no checksum
    auto assetID = parse_hex("0xdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db"); 

    auto addressesOutOne = generateAddressesForSigner();
    auto coreOutputOne = std::make_unique<SECP256k1TransferOutput>(12345, 54321, 1, addressesOutOne);
    auto outputOne = TransferableOutput(assetID, std::move(coreOutputOne));

    auto addressesOutTwo = generateAddressesForSigner();
    auto coreOutputTwo = std::make_unique<SECP256k1TransferOutput>(3, 2, 1, addressesOutTwo);
    auto outputTwo = TransferableOutput(assetID, std::move(coreOutputTwo));

    auto outputs = std::vector<TransferableOutput>{outputOne, outputTwo};

    auto transaction = UnsignedExportTransaction(baseTransaction, destChain, outputs);
    Data encodedUnsignedTransaction;
    transaction.encode(encodedUnsignedTransaction);

    ASSERT_EQ(hex(encodedUnsignedTransaction), "0000000400003039d891ad56056d9c01f18f43f58b5c784ad07a4a49cf3d1f11623804b5cba2c6bf00000002dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000700000000000003e8000000000000000000000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29cdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db000000070000000000003039000000000000d43100000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29c00000002f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd15000000020000000300000007f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd1500000002000000030000000700000004deadbeefd891ad56056d9c01f18f43f58b5c784ad07a4a49cf3d1f11623804b5cba2c6bf00000002dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db00000007000000000000000300000000000000020000000100000003355a58440e3647f394aff711aca083e5a5181426e8777f38c88ca153a6fdc25942176d2bf5491b89ee93c2aaa8b4b93d45ce77237e9f65192be9a39bdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db000000070000000000003039000000000000d4310000000100000003355a58440e3647f394aff711aca083e5a5181426e8777f38c88ca153a6fdc25942176d2bf5491b89ee93c2aaa8b4b93d45ce77237e9f65192be9a39b");
    std::vector<PrivateKey> keyRing = {privateKey};
    auto encodedSignedTransaction = Signer::sign(keyRing, transaction);
    ASSERT_EQ(hex(encodedSignedTransaction), "00000000000400003039d891ad56056d9c01f18f43f58b5c784ad07a4a49cf3d1f11623804b5cba2c6bf00000002dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000700000000000003e8000000000000000000000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29cdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db000000070000000000003039000000000000d43100000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29c00000002f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd15000000020000000300000007f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd1500000002000000030000000700000004deadbeef000000020000000900000002a964ef14456ddfa1b4f969c18ec1a139806da2a3869b4205f160a5ad998b075f692b4a052680587a677f1a609cec9e147f2053f676c8b989366f7f84fba17a4e00a964ef14456ddfa1b4f969c18ec1a139806da2a3869b4205f160a5ad998b075f692b4a052680587a677f1a609cec9e147f2053f676c8b989366f7f84fba17a4e000000000900000002a964ef14456ddfa1b4f969c18ec1a139806da2a3869b4205f160a5ad998b075f692b4a052680587a677f1a609cec9e147f2053f676c8b989366f7f84fba17a4e00a964ef14456ddfa1b4f969c18ec1a139806da2a3869b4205f160a5ad998b075f692b4a052680587a677f1a609cec9e147f2053f676c8b989366f7f84fba17a4e00");
}

std::pair<PrivateKey, BaseTransaction> getPrivateKeyAndTransaction() {
    const auto privateKeyBytes = CB58::avalanche.decode("ewoqjP7PxY4yr3iLTpLisriqt94hdyDFNgchSxGGztUrTXtNN");
    const std::vector<uint8_t> privateKeyBytesNoChecksum(privateKeyBytes.begin(), privateKeyBytes.begin() + 32); // we just want the first 32 bytes
    const auto privateKey = PrivateKey(privateKeyBytesNoChecksum); 
    const auto publicKeyOne = privateKey.getPublicKey(TWPublicKeyTypeSECP256k1);
    const auto addressOne = Address(publicKeyOne);

    std::vector<Address> spendableAddresses = {addressOne, addressOne, addressOne, addressOne, addressOne, addressOne, addressOne, addressOne, addressOne, addressOne};

    auto blockchainID = CB58::avalanche.decode("2eNy1mUFdmaxXNj1eQHUe7Np4gju9sJsEtWQ4MX3ToiNKuADed");
    Data blockchainIDBytes(blockchainID.begin(), blockchainID.begin() + BLOCKCHAIN_ID_SIZE); // we just want the first 32 bytes, no checksum
    uint32_t netID = 12345;
    auto assetID = parse_hex("0xdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db"); 
    uint32_t baseTypeID = 0;
    Data memo = {0xde, 0xad, 0xbe, 0xef};
    auto amount = 1000;   
    auto locktime = 0;
    auto threshold = 1;

    auto addressesInOne = std::vector<uint32_t>{3, 7};
    auto txidOne = parse_hex("0xf1e1d1c1b1a191817161514131211101f0e0d0c0b0a090807060504030201000");
    auto utxoIndexOne = 5;
    auto coreInputOne = std::make_unique<SECP256k1TransferInput>(123456789, addressesInOne);
    auto inputOne = TransferableInput(txidOne, utxoIndexOne, assetID, std::move(coreInputOne), spendableAddresses);

    auto addressesInTwo = std::vector<uint32_t>{3, 7};
    auto txidTwo = parse_hex("0xf1e1d1c1b1a191817161514131211101f0e0d0c0b0a090807060504030201000");
    auto utxoIndexTwo = 5;
    auto coreInputTwo = std::make_unique<SECP256k1TransferInput>(123456789, addressesInTwo);
    auto inputTwo = TransferableInput(txidTwo, utxoIndexTwo, assetID, std::move(coreInputTwo), spendableAddresses);
    
    auto inputs = std::vector<TransferableInput>{inputOne, inputTwo};

    auto addressesOutOne = std::vector<Address>{addressOne};
    auto coreOutputOne = std::make_unique<SECP256k1TransferOutput>(12345, 54321, threshold, addressesOutOne);
    auto outputOne = TransferableOutput(assetID, std::move(coreOutputOne));

    auto addressesOutTwo = std::vector<Address>{addressOne};
    auto coreOutputTwo = std::make_unique<SECP256k1TransferOutput>(amount, locktime, threshold, addressesOutTwo);
    auto outputTwo = TransferableOutput(assetID, std::move(coreOutputTwo));

    auto outputs = std::vector<TransferableOutput>{outputOne, outputTwo};

    auto transaction = BaseTransaction(baseTypeID, netID, blockchainIDBytes, inputs, outputs, memo);

    return std::make_pair(privateKey, transaction);
}

std::vector<Address> generateAddressesForSigner() {
    // build some arbitrary addresses for this test
    std::vector<Address> arbitraryAddresses;
    {   
        auto privateKey = PrivateKey(parse_hex("0x98cb077f972feb0481f1d894f272c6a1e3c15e272a1658ff716444f465200070"));
        const auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeSECP256k1);
        arbitraryAddresses.push_back(Address(publicKey));
    }
    {   
        auto privateKey = PrivateKey(parse_hex("0x88cb770f972feb0481f1d894f272c6a1e3c15e272a1658ff716444f465200070"));
        const auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeSECP256k1);
        arbitraryAddresses.push_back(Address(publicKey));
    }
    {   
        auto privateKey = PrivateKey(parse_hex("0x78cb077f972fe00481f0d894f272c6a1e3c15e272a1658ff716444f465200070"));
        const auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeSECP256k1);
        arbitraryAddresses.push_back(Address(publicKey));
    }
    return arbitraryAddresses;
}
