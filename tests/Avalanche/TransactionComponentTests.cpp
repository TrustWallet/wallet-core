// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "HexCoding.h"
#include "PrivateKey.h"
#include "PublicKey.h"
#include "Avalanche/CB58.h"
#include "Avalanche/Transaction.h"

#include <gtest/gtest.h>

using namespace TW;
using namespace TW::Avalanche;

std::vector<Address> generateAddressesForComponent() {
    // build some arbitrary addresses for this test
    std::vector<Address> arbitraryAddresses;
    arbitraryAddresses.push_back(Address("X-avax18lasc0q97jlurnnxuyyqyz0rm6tvl0ecsvkvjj"));
    arbitraryAddresses.push_back(Address("X-avax1c83k5c3ezrynj3ah2nzppt6zsqz7l6z3cgkgxh"));
    arbitraryAddresses.push_back(Address("X-avax1kancmd604dq8wut9plw6qxv2f0sy4ndxn6tng0"));
    return arbitraryAddresses;
}

TEST(AvalancheTransactionComponents, TestInitialState) {
    auto arbitraryAddresses = generateAddressesForComponent();
    // build some arbitrary outputs for this test
    std::vector<std::unique_ptr<TransactionOutput>> arbitraryOutputs;
    // fill outputs with arbitrary transactionoutputs
    arbitraryOutputs.push_back(std::make_unique<SECP256k1TransferOutput>(12345, 54321, 333, arbitraryAddresses));
    arbitraryOutputs.push_back(std::make_unique<SECP256k1TransferOutput>(54321, 54321, 333, arbitraryAddresses));
    arbitraryOutputs.push_back(std::make_unique<SECP256k1TransferOutput>(111, 111, 111, arbitraryAddresses));
    auto stateOne = InitialState(InitialState::FeatureExtension::SECP256k1, std::move(arbitraryOutputs));
    auto stateTwo = InitialState(stateOne); // should be able to build a state from another
    EXPECT_EQ(stateTwo.FxID, InitialState::FeatureExtension::SECP256k1);
    stateOne.FxID = InitialState::FeatureExtension::NFT;
    stateTwo = stateOne; // should be able to assign states to each other
    EXPECT_EQ(stateTwo.FxID, InitialState::FeatureExtension::NFT);
    stateOne.FxID = InitialState::FeatureExtension::SECP256k1;
    EXPECT_TRUE(stateOne < stateTwo); // state one should now be less than state two by lexicographical sort
}

TEST(AvalancheTransactionComponents, TestTransferableInput) {
    auto arbitraryAddresses = generateAddressesForComponent();
    auto assetID = parse_hex("0xdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db"); 
    
    auto addressesInOne = std::vector<uint32_t>{3, 7};
    auto txidOne = parse_hex("0x00e1d1c1b1a191817161514131211101f0e0d0c0b0a090807060504030201000");
    auto utxoIndexOne = 6;
    auto coreInputOne = std::make_unique<SECP256k1TransferInput>(123456789, addressesInOne);
    auto inputOne = TransferableInput(txidOne, utxoIndexOne, assetID, std::move(coreInputOne), arbitraryAddresses);

    auto addressesInTwo = std::vector<uint32_t>{4, 8};
    auto txidTwo = parse_hex("0xf1e1d1c1b1a191817161514131211101f0e0d0c0b0a090807060504030201000");
    auto utxoIndexTwo = 9;
    auto coreInputTwo = std::make_unique<SECP256k1TransferInput>(987654321, addressesInTwo);
    auto inputTwo = TransferableInput(txidTwo, utxoIndexTwo, assetID, std::move(coreInputTwo), arbitraryAddresses);

    auto inputThree = TransferableInput(inputTwo); // should be able to build from each other
    EXPECT_EQ(inputTwo.UTXOIndex, inputThree.UTXOIndex);
    EXPECT_FALSE(inputOne.UTXOIndex == inputThree.UTXOIndex);

    inputThree = inputOne; // should be able to assign inputs to each other
    inputThree = inputThree; // and self-assignment should be a no-op
    EXPECT_TRUE(inputOne.UTXOIndex == inputThree.UTXOIndex);

    EXPECT_TRUE(inputThree < inputTwo); // inputThree should be less than inputTwo by lexicographical sort if it was assigned properly
}

TEST(AvalancheTransactionComponents, TestTransferableOutputAssignmentConstructing) {
    auto assetIDOne = parse_hex("0xdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba530000"); 
    auto addressesOutOne = generateAddressesForComponent();
    auto coreOutputOne = std::make_unique<SECP256k1TransferOutput>(12345, 54321, 5, addressesOutOne);
    auto outputOne = TransferableOutput(assetIDOne, std::move(coreOutputOne));

    auto assetIDTwo = parse_hex("0xdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db"); 
    auto addressesOutTwo = generateAddressesForComponent();
    auto coreOutputTwo = std::make_unique<SECP256k1TransferOutput>(12345, 54321, 6, addressesOutTwo);
    auto outputTwo = TransferableOutput(assetIDTwo, std::move(coreOutputTwo));

    EXPECT_TRUE(outputOne < outputTwo); // outputOne should be less than outputTwo by lexicographical sort

    auto outputThree = TransferableOutput(outputTwo); // should be able to build from each other
    EXPECT_EQ(outputThree.AssetID, outputTwo.AssetID);
    EXPECT_FALSE(outputThree.AssetID == outputOne.AssetID);

    outputThree = outputOne; // should be able to assign from each other
    outputThree = outputThree; // and self-assignment should be a no-op
    EXPECT_EQ(outputThree.AssetID, outputOne.AssetID);
    EXPECT_FALSE(outputThree.AssetID == outputTwo.AssetID);
    EXPECT_TRUE(outputThree < outputTwo); // outputOne should be less than outputTwo by lexicographical sort if assignment successful
}

TEST(AvalancheTransactionComponents, TestOutputDuplicateEncodeEquality) {
    uint32_t groupID = 1;
    uint64_t locktime = 2;
    uint32_t threshold = 3;
    auto addresses = generateAddressesForComponent();

    auto nftMint = NFTMintOutput(groupID, locktime, threshold, addresses);
    auto dupedMintPtr = nftMint.duplicate();
    Data nftMintData, dupedMintData;
    nftMint.encode(nftMintData);
    dupedMintPtr->encode(dupedMintData);
    EXPECT_EQ(hex(nftMintData), hex(dupedMintData));

    auto secpMint = SECP256k1MintOutput(locktime, threshold, addresses);
    auto dupedSecpPtr = secpMint.duplicate();
    Data secpMintData, dupedSecpData;
    secpMint.encode(secpMintData);
    dupedSecpPtr->encode(dupedSecpData);
    EXPECT_EQ(hex(secpMintData), hex(dupedSecpData));

    auto payload = parse_hex("44ef527");
    auto nftTxfer = NFTTransferOutput(groupID, payload, locktime, threshold, addresses);
    auto dupedNftTxferPtr = nftTxfer.duplicate();
    Data nftTxferData, dupedTxferData;
    nftTxfer.encode(nftTxferData);
    dupedNftTxferPtr->encode(dupedTxferData);
    EXPECT_EQ(hex(nftTxferData), hex(dupedTxferData));
}

TEST(AvalancheTransactionComponents, CredentialOperatorLesser) {
    auto higherSig = parse_hex("44ef527f47cab3ed82eb267c27c04869e46531b05db643f5bc97da21148afe161f17634a90f4e22adb810b472062f7e809dde19059fa7048f9972a481fe9390d0044ef527f47cab3ed82eb267c27c04869e46531b05db643f5bc97da21148afe161f17634a90f4e22adb810b472062f7e809dde19059fa7048f9972a481fe9390d00");
    auto higherSigArray = std::vector<Data>{higherSig};
    auto higherCredential = SECP256k1Credential(higherSigArray);
    auto lowerSig = parse_hex("0000007f47cab3ed82eb267c27c04869e46531b05db643f5bc97da21148afe161f17634a90f4e22adb810b472062f7e809dde19059fa7048f9972a481fe9390d0044ef527f47cab3ed82eb267c27c04869e46531b05db643f5bc97da21148afe161f17634a90f4e22adb810b472062f7e809dde19059fa7048f9972a481fe9390d00");
    auto lowerSigArray = std::vector<Data>{lowerSig};
    auto lowerCredential = SECP256k1Credential(lowerSigArray);
    ASSERT_TRUE(lowerCredential < higherCredential);
}

TEST(AvalancheTransactionComponents, TestOutput) {
    // make some arbitrary Outputs
    auto addressesOne = generateAddressesForComponent();
    auto addressesTwo = generateAddressesForComponent();
    auto addressesThree = generateAddressesForComponent();
    auto addressesFour = generateAddressesForComponent();
    addressesFour.push_back(Address("X-avax1wrgqjed292adreyal364nmz4jfjq2x5qkj7ack"));
    
    Output outputOne = std::make_tuple(uint64_t(0), uint32_t(0), addressesOne);
    Output outputTwo = std::make_tuple(uint64_t(1), uint32_t(0), addressesTwo);
    Output outputThree = std::make_tuple(uint64_t(1), uint32_t(1), addressesThree);
    Output outputFour = std::make_tuple(uint64_t(1), uint32_t(1), addressesFour);
    // make a vector that is improperly sorted
    // sort it, and encode it
    // make sure result is as expected
    std::vector<Output> outputs = {outputFour, outputTwo, outputThree, outputOne};
    SortOutputs(outputs);
    EXPECT_EQ(outputs.front(), outputOne);
    EXPECT_EQ(outputs.back(), outputFour);
    Data outputData; 
    EncodeOutputs(outputs, outputData);
    EXPECT_EQ(hexEncoded(outputData), "0x00000004000000000000000000000000000000033ffb0c3c05f4bfc1ce66e1080209e3de96cfbf38c1e36a623910c93947b754c410af428005efe851b7678db74fab407771650fdda0198a4be04acda6000000000000000100000000000000033ffb0c3c05f4bfc1ce66e1080209e3de96cfbf38c1e36a623910c93947b754c410af428005efe851b7678db74fab407771650fdda0198a4be04acda6000000000000000100000001000000033ffb0c3c05f4bfc1ce66e1080209e3de96cfbf38c1e36a623910c93947b754c410af428005efe851b7678db74fab407771650fdda0198a4be04acda6000000000000000100000001000000043ffb0c3c05f4bfc1ce66e1080209e3de96cfbf38c1e36a623910c93947b754c410af428005efe851b7678db74fab407771650fdda0198a4be04acda670d00965aa2abad1e49dfc7559ec559264051a80");
}

BaseTransaction generateBaseTransactionBasedOnSignerTest();

TEST(AvalancheTransactionComponents, TestTransactionFailureCases) {
    auto goodBase = generateBaseTransactionBasedOnSignerTest();
    // try a too-long memo for a BaseTransaction
    Data bigMemo;
    for (int i = 0; i < MAX_MEMO_SIZE + 1; i++) {
        bigMemo.push_back(0xde);
    }
    EXPECT_ANY_THROW(BaseTransaction(goodBase.TypeID, goodBase.NetworkID, goodBase.BlockchainID, goodBase.Inputs, goodBase.Outputs, bigMemo));
    // try a too-long asset name for an UnsignedCreateAssetTransaction
    std::vector<InitialState> emptyStates;
    std::string longAssetName = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    std::string shortSymbol = "";
    EXPECT_TRUE(longAssetName.size() > MAX_ASSET_NAME_CHARS);
    uint8_t denomination = 1;
    EXPECT_ANY_THROW(UnsignedCreateAssetTransaction(goodBase, longAssetName, shortSymbol, denomination, emptyStates));
    // try a too-long symbol for an UnsignedCreateAssetTransaction
    std::string shortName = "";
    std::string longSymbol = "aaaaa";
    EXPECT_TRUE(longSymbol.size() > MAX_SYMBOL_CHARS);
    EXPECT_ANY_THROW(UnsignedCreateAssetTransaction(goodBase, shortName, longSymbol, denomination, emptyStates));
    // try a wrong-size blockchain ID for a BaseTransaction
    auto tooShortBlockchainID = Data{};
    EXPECT_TRUE(tooShortBlockchainID.size() != BLOCKCHAIN_ID_SIZE);
    EXPECT_ANY_THROW(BaseTransaction(goodBase.TypeID, goodBase.NetworkID, tooShortBlockchainID, goodBase.Inputs, goodBase.Outputs, goodBase.Memo));
    // try a wrong-size blockchain ID for source in an UnsignedImportTransaction
    std::vector<TransferableInput> emptyInputs;
    EXPECT_ANY_THROW(UnsignedImportTransaction(goodBase, tooShortBlockchainID, emptyInputs));
    // try a wrong-size blockchain ID for dest in an UnsignedExportTransaction
    std::vector<TransferableOutput> emptyOutputs;
    EXPECT_ANY_THROW(UnsignedExportTransaction(goodBase, tooShortBlockchainID, emptyOutputs));
}

BaseTransaction generateBaseTransactionBasedOnSignerTest() {
    const auto privateKeyOneBytes = CB58::avalanche.decode("ewoqjP7PxY4yr3iLTpLisriqt94hdyDFNgchSxGGztUrTXtNN");
    const std::vector<uint8_t> privateKeyOneBytesNoChecksum(privateKeyOneBytes.begin(), privateKeyOneBytes.begin() + 32); // we just want the first 32 bytes
    const auto privateKeyOne = PrivateKey(privateKeyOneBytesNoChecksum); 
    const auto publicKeyOne = privateKeyOne.getPublicKey(TWPublicKeyTypeSECP256k1);
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

    return BaseTransaction(baseTypeID, netID, blockchainIDBytes, inputs, outputs, memo);
}