// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.


#include "proto/Avalanche.pb.h"
#include <TrustWalletCore/TWAnySigner.h>
#include "HexCoding.h"
#include "Avalanche/CB58.h"
#include "PrivateKey.h"
#include "Avalanche/Address.h"
#include "Avalanche/InitialState.h"
#include "Avalanche/Transaction.h"
#include "../interface/TWTestUtilities.h"
#include <gtest/gtest.h>

using namespace TW;
using namespace TW::Avalanche;

std::vector<PublicKey> generateAddressesForAnySigner();
const std::vector<uint8_t> getDefaultPrivateKeyBytes();
void setUpDefaultPrivateKeyData(Proto::SigningInput &input);
void setUpDefaultBaseTx(TW::Avalanche::Proto::BaseTx &baseTx);

TEST(TWAnySignerAvalanche, SignBasicTransaction) {
    Proto::SigningInput input;
    setUpDefaultPrivateKeyData(input);
    auto &inputTx = *input.mutable_input_tx();
    auto &baseTx = *inputTx.mutable_base_tx();
    setUpDefaultBaseTx(baseTx);
    Proto::SigningOutput output;
    ANY_SIGN(input, TWCoinTypeAvalancheXChain);

    ASSERT_EQ(hex(output.encoded()), "00000000000000003039d891ad56056d9c01f18f43f58b5c784ad07a4a49cf3d1f11623804b5cba2c6bf00000002dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000700000000000003e8000000000000000000000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29cdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db000000070000000000003039000000000000d43100000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29c00000002f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd15000000020000000300000007f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd1500000002000000030000000700000004deadbeef00000002000000090000000244ef527f47cab3ed82eb267c27c04869e46531b05db643f5bc97da21148afe161f17634a90f4e22adb810b472062f7e809dde19059fa7048f9972a481fe9390d0044ef527f47cab3ed82eb267c27c04869e46531b05db643f5bc97da21148afe161f17634a90f4e22adb810b472062f7e809dde19059fa7048f9972a481fe9390d00000000090000000244ef527f47cab3ed82eb267c27c04869e46531b05db643f5bc97da21148afe161f17634a90f4e22adb810b472062f7e809dde19059fa7048f9972a481fe9390d0044ef527f47cab3ed82eb267c27c04869e46531b05db643f5bc97da21148afe161f17634a90f4e22adb810b472062f7e809dde19059fa7048f9972a481fe9390d00");
}

TEST(TWAnySignerAvalanche, SignCreateAssetTransaction) {
    Proto::SigningInput input;
    setUpDefaultPrivateKeyData(input);
    auto &inputTx = *input.mutable_input_tx();
    auto &createAssetTx = *inputTx.mutable_create_asset_tx();
    auto &baseTx = *createAssetTx.mutable_base_tx();
    setUpDefaultBaseTx(baseTx);

    auto arbitraryAddresses = generateAddressesForAnySigner();

    auto stateOne = createAssetTx.add_initial_states();
    stateOne->set_fx_id(Avalanche::InitialState::FeatureExtension::SECP256k1);
    // amount, locktime, threshold, addresses
    {
        auto firstCoreOutput = new Proto::SECP256K1TransferOutput(); //12345, 54321, 333, arbitraryAddresses
        firstCoreOutput->set_amount(12345);
        firstCoreOutput->set_amount(54321);
        firstCoreOutput->set_threshold(333);
        for (auto i = 0; i < arbitraryAddresses.size(); ++i) {
            firstCoreOutput->add_addresses();
            firstCoreOutput->set_addresses(i, arbitraryAddresses[i].bytes.data(), arbitraryAddresses[i].bytes.size());
        }
        stateOne->add_outputs();
        auto wrappedOutputOne = stateOne->mutable_outputs(0);
        wrappedOutputOne->set_allocated_secp_transfer_output(firstCoreOutput);
   
        auto secondCoreOutput = new Proto::SECP256K1TransferOutput(); //54321, 54321, 333, arbitraryAddresses
        secondCoreOutput->set_amount(54321);
        secondCoreOutput->set_amount(54321);
        secondCoreOutput->set_threshold(333);
        for (auto i = 0; i < arbitraryAddresses.size(); ++i) {
            secondCoreOutput->add_addresses();
            secondCoreOutput->set_addresses(i, arbitraryAddresses[i].bytes.data(), arbitraryAddresses[i].bytes.size());
        }
        stateOne->add_outputs();
        auto wrappedOutputTwo = stateOne->mutable_outputs(1);
        wrappedOutputTwo->set_allocated_secp_transfer_output(secondCoreOutput);
   
        auto thirdCoreOutput = new Proto::SECP256K1TransferOutput(); //111, 111, 111, arbitraryAddresses
        thirdCoreOutput->set_amount(111);
        thirdCoreOutput->set_amount(111);
        thirdCoreOutput->set_threshold(111);
        for (auto i = 0; i < arbitraryAddresses.size(); ++i) {
            thirdCoreOutput->add_addresses();
            thirdCoreOutput->set_addresses(i, arbitraryAddresses[i].bytes.data(), arbitraryAddresses[i].bytes.size());
        }
        stateOne->add_outputs();
        auto wrappedOutputThree = stateOne->mutable_outputs(2);
        wrappedOutputThree->set_allocated_secp_transfer_output(thirdCoreOutput);
    }
    auto stateTwo = createAssetTx.add_initial_states();
    stateTwo->set_fx_id(Avalanche::InitialState::FeatureExtension::SECP256k1);
    // amount, locktime, threshold, addresses
    {
        auto firstCoreOutput = new Proto::SECP256K1TransferOutput(); //12345, 54321, 333, arbitraryAddresses
        firstCoreOutput->set_amount(12345);
        firstCoreOutput->set_amount(54321);
        firstCoreOutput->set_threshold(333);
        for (auto i = 0; i < arbitraryAddresses.size(); ++i) {
            firstCoreOutput->add_addresses();
            firstCoreOutput->set_addresses(i, arbitraryAddresses[i].bytes.data(), arbitraryAddresses[i].bytes.size());
        }
        auto wrappedOutputOne = stateTwo->add_outputs();
        wrappedOutputOne->set_allocated_secp_transfer_output(firstCoreOutput);
   
        auto secondCoreOutput = new Proto::SECP256K1TransferOutput(); //54321, 54321, 333, arbitraryAddresses
        secondCoreOutput->set_amount(54321);
        secondCoreOutput->set_amount(54321);
        secondCoreOutput->set_threshold(333);
        for (auto i = 0; i < arbitraryAddresses.size(); ++i) {
            secondCoreOutput->add_addresses();
            secondCoreOutput->set_addresses(i, arbitraryAddresses[i].bytes.data(), arbitraryAddresses[i].bytes.size());
        }
        stateTwo->add_outputs();
        auto wrappedOutputTwo = stateTwo->mutable_outputs(1);
        wrappedOutputTwo->set_allocated_secp_transfer_output(secondCoreOutput);
   
        auto thirdCoreOutput = new Proto::SECP256K1TransferOutput(); //111, 111, 111, arbitraryAddresses
        thirdCoreOutput->set_amount(111);
        thirdCoreOutput->set_amount(111);
        thirdCoreOutput->set_threshold(111);
        for (auto i = 0; i < arbitraryAddresses.size(); ++i) {
            thirdCoreOutput->add_addresses();
            thirdCoreOutput->set_addresses(i, arbitraryAddresses[i].bytes.data(), arbitraryAddresses[i].bytes.size());
        }
        stateTwo->add_outputs();
        auto wrappedOutputThree = stateTwo->mutable_outputs(2);
        wrappedOutputThree->set_allocated_secp_transfer_output(thirdCoreOutput);
    }
    std::string name = "testcoin";
    createAssetTx.set_name(name);
    
    std::string symbol = "test";
    createAssetTx.set_symbol(symbol);

    uint8_t denomination = 1;
    createAssetTx.set_denomination(denomination);

    Proto::SigningOutput output;
    ANY_SIGN(input, TWCoinTypeAvalancheXChain);

    ASSERT_EQ(hex(output.encoded()), "00000000000100003039d891ad56056d9c01f18f43f58b5c784ad07a4a49cf3d1f11623804b5cba2c6bf00000002dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000700000000000003e8000000000000000000000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29cdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db000000070000000000003039000000000000d43100000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29c00000002f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd15000000020000000300000007f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd1500000002000000030000000700000004deadbeef0000000200000009000000021690ee4d10718b83bd43ce9cec43bf91d23b9380755de49d59331f7c25be60eb228ffcc273393597dcd7109af08eb59e9fcc09e256f2c24f04c58dbd6d1f04af011690ee4d10718b83bd43ce9cec43bf91d23b9380755de49d59331f7c25be60eb228ffcc273393597dcd7109af08eb59e9fcc09e256f2c24f04c58dbd6d1f04af0100000009000000021690ee4d10718b83bd43ce9cec43bf91d23b9380755de49d59331f7c25be60eb228ffcc273393597dcd7109af08eb59e9fcc09e256f2c24f04c58dbd6d1f04af011690ee4d10718b83bd43ce9cec43bf91d23b9380755de49d59331f7c25be60eb228ffcc273393597dcd7109af08eb59e9fcc09e256f2c24f04c58dbd6d1f04af01");
}

TEST(TWAnySignerAvalanche, SignOperationTransaction) {
    Proto::SigningInput input;
    setUpDefaultPrivateKeyData(input);
    auto &inputTx = *input.mutable_input_tx();
    auto &operationTx = *inputTx.mutable_operation_tx();
    auto &baseTx = *operationTx.mutable_base_tx();
    setUpDefaultBaseTx(baseTx);

    auto assetIDBytes = parse_hex("0xdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db");
    auto txIDBytes = parse_hex("0xf1e1d1c1b1a191817161514131211101f0e0d0c0b0a090807060504030201000");

    //TODO typeids of ops are possibly off!
    {
        auto op = operationTx.add_ops();
        op->set_asset_id(assetIDBytes.data(), assetIDBytes.size());
        {
            auto utxoID = op->add_utxo_ids();
            utxoID->set_tx_id(txIDBytes.data(), txIDBytes.size());
            utxoID->set_utxo_index(1);
        }
        {
            auto utxoID = op->add_utxo_ids();
            utxoID->set_tx_id(txIDBytes.data(), txIDBytes.size());
            utxoID->set_utxo_index(2);
        }
        auto transferOp = op->mutable_transfer_op(); 
        auto nftOp = transferOp->mutable_nft_mint_op();
        auto output = nftOp->add_outputs();
        auto addresses = generateAddressesForAnySigner();
        output->set_locktime(1);
        output->set_locktime(2);
        for (int i = 0; i < addresses.size(); ++i) {
            output->add_addresses();
            output->set_addresses(i, addresses[i].bytes.data(), addresses[i].bytes.size());
        }
        std::vector<uint32_t> addressIndices = {1, 2, 3};
        uint32_t groupID = 1;
        Data payload = {0xfe, 0xed, 0xfe, 0xed};
        for (auto& addressIndex: addressIndices) {
            nftOp->add_address_indices(addressIndex);
        }
        nftOp->set_group_id(groupID);
        nftOp->set_payload(payload.data(), payload.size());
    }

    {
        auto op = operationTx.add_ops();
        op->set_asset_id(assetIDBytes.data(), assetIDBytes.size());
        {
            auto utxoID = op->add_utxo_ids();
            utxoID->set_tx_id(txIDBytes.data(), txIDBytes.size());
            utxoID->set_utxo_index(3);
        }
        {
            auto differingTxIDBytes = parse_hex("0x0000d1c1b1a191817161514131211101f0e0d0c0b0a090807060504030201000");
            auto utxoID = op->add_utxo_ids();
            utxoID->set_tx_id(differingTxIDBytes.data(), differingTxIDBytes.size());
            utxoID->set_utxo_index(3);
        }
        auto transferOp = op->mutable_transfer_op(); 
        auto secpMintOp = transferOp->mutable_secp_mint_op();
        std::vector<uint32_t> addressIndices = {2, 3, 4};
        for (auto& addressIndex: addressIndices) {
            secpMintOp->add_address_indices(addressIndex);
        }
        auto mintOutput = secpMintOp->mutable_mint_output();
        auto mintOutputAddrs = generateAddressesForAnySigner();
        mintOutput->set_locktime(10);
        mintOutput->set_threshold(20);
        for (int i = 0; i < mintOutputAddrs.size(); ++i) {
            mintOutput->add_addresses();
            mintOutput->set_addresses(i, mintOutputAddrs[i].bytes.data(), mintOutputAddrs[i].bytes.size());
        }
        auto transferOutput = secpMintOp->mutable_transfer_output();
        auto transferOutputAddrs = generateAddressesForAnySigner();
        transferOutput->set_amount(30);
        transferOutput->set_locktime(40);
        transferOutput->set_threshold(5);
        for (int i = 0; i < transferOutputAddrs.size(); ++i) {
            transferOutput->add_addresses();
            transferOutput->set_addresses(i, transferOutputAddrs[i].bytes.data(), transferOutputAddrs[i].bytes.size());
        }
    }
    
    {   
        auto op = operationTx.add_ops();
        op->set_asset_id(assetIDBytes.data(), assetIDBytes.size());
        {
            auto utxoID = op->add_utxo_ids();
            utxoID->set_tx_id(txIDBytes.data(), txIDBytes.size());
            utxoID->set_utxo_index(6);
        }
        {
            auto utxoID = op->add_utxo_ids();
            utxoID->set_tx_id(txIDBytes.data(), txIDBytes.size());
            utxoID->set_utxo_index(5);
        }
        auto transferOp = op->mutable_transfer_op(); 
        auto nftTransferOp = transferOp->mutable_nft_transfer_op();
        std::vector<uint32_t> addressIndices = {3, 4, 5};
        for (auto& addressIndex: addressIndices) {
            nftTransferOp->add_address_indices(addressIndex);
        }
        auto nftTransferAddrs = generateAddressesForAnySigner();
        for (int i = 0; i < nftTransferAddrs.size(); ++i) {
            nftTransferOp->add_addresses();
            nftTransferOp->set_addresses(i, nftTransferAddrs[i].bytes.data(), nftTransferAddrs[i].bytes.size());
        }
        auto payload = Data{0xfe, 0xde, 0xfe, 0xde};
        nftTransferOp->set_payload(payload.data(), payload.size());
        nftTransferOp->set_group_id(1);
        nftTransferOp->set_locktime(2);
        nftTransferOp->set_threshold(3);
    }
    Proto::SigningOutput output;
    ANY_SIGN(input, TWCoinTypeAvalancheXChain);

    ASSERT_EQ(hex(output.encoded()), "00000000000200003039d891ad56056d9c01f18f43f58b5c784ad07a4a49cf3d1f11623804b5cba2c6bf00000002dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000700000000000003e8000000000000000000000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29cdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db000000070000000000003039000000000000d43100000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29c00000002f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd15000000020000000300000007f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd1500000002000000030000000700000004deadbeef0000000200000009000000028d718dfbf41202d03e495cda3eeb2464dfdc0e7e1e73e3189e5b4752dae1a919360e43aeb3c31c1258c8cc80028bbcd83f15d00aa40dffbcbc9ad35e4f57ac8b008d718dfbf41202d03e495cda3eeb2464dfdc0e7e1e73e3189e5b4752dae1a919360e43aeb3c31c1258c8cc80028bbcd83f15d00aa40dffbcbc9ad35e4f57ac8b0000000009000000028d718dfbf41202d03e495cda3eeb2464dfdc0e7e1e73e3189e5b4752dae1a919360e43aeb3c31c1258c8cc80028bbcd83f15d00aa40dffbcbc9ad35e4f57ac8b008d718dfbf41202d03e495cda3eeb2464dfdc0e7e1e73e3189e5b4752dae1a919360e43aeb3c31c1258c8cc80028bbcd83f15d00aa40dffbcbc9ad35e4f57ac8b00");
}

TEST(TWAnySignerAvalanche, SignImportTransaction) {
    Proto::SigningInput input;
    setUpDefaultPrivateKeyData(input);
    auto &inputTx = *input.mutable_input_tx();
    auto &importTx = *inputTx.mutable_import_tx();
    auto &baseTx = *importTx.mutable_base_tx();
    setUpDefaultBaseTx(baseTx);
    
    auto blockchainID = CB58::avalanche.decode("2eNy1mUFdmaxXNj1eQHUe7Np4gju9sJsEtWQ4MX3ToiNKuADed");
    Data sourceChain(blockchainID.begin(), blockchainID.begin() + BLOCKCHAIN_ID_SIZE); // we just want the first 32 bytes, no checksum
    importTx.set_source_chain(sourceChain.data(), sourceChain.size());

    auto assetIDBytes = parse_hex("0xdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db"); 
    auto spendableAddresses = generateAddressesForAnySigner();
    {
        auto txIDBytes = parse_hex("0xf1e1d1c1b1a191817161514131211101f0e0d0c0b0a090807060504030201000");
        auto input = importTx.add_ins();
        input->set_tx_id(txIDBytes.data(), txIDBytes.size());
        input->set_utxo_index(5);
        input->set_asset_id(assetIDBytes.data(), assetIDBytes.size());
        for (int i = 0; i < spendableAddresses.size(); ++i) {
            input->add_spendable_addresses();
            input->set_spendable_addresses(i, spendableAddresses[i].bytes.data(), spendableAddresses[i].bytes.size());
        }
        auto secpInput = input->mutable_input()->mutable_secp_transfer_input();
        auto addressesIn = std::vector<uint32_t>{3, 7};
        for (auto& addressIndex : addressesIn) {
            secpInput->add_address_indices(addressIndex);
        }
        secpInput->set_amount(123456789);
    }
    {    
        auto txIDBytes = parse_hex("0xf1e1d1c1b1a191817161514131211101f0e0d0c0b0a090807060504030201000");
        auto input = importTx.add_ins();
        input->set_tx_id(txIDBytes.data(), txIDBytes.size());
        input->set_utxo_index(5);
        input->set_asset_id(assetIDBytes.data(), assetIDBytes.size());
        for (int i = 0; i < spendableAddresses.size(); ++i) {
            input->add_spendable_addresses();
            input->set_spendable_addresses(i, spendableAddresses[i].bytes.data(), spendableAddresses[i].bytes.size());
        }
        auto secpInput = input->mutable_input()->mutable_secp_transfer_input();
        auto addressesIn = std::vector<uint32_t>{3, 7};
        for (auto& addressIndex : addressesIn) {
            secpInput->add_address_indices(addressIndex);
        }
        secpInput->set_amount(123456789);
    }
    Proto::SigningOutput output;
    ANY_SIGN(input, TWCoinTypeAvalancheXChain);
    ASSERT_EQ(hex(output.encoded()), "00000000000300003039d891ad56056d9c01f18f43f58b5c784ad07a4a49cf3d1f11623804b5cba2c6bf00000002dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000700000000000003e8000000000000000000000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29cdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db000000070000000000003039000000000000d43100000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29c00000002f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd15000000020000000300000007f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd1500000002000000030000000700000004deadbeef000000020000000900000002f7393f970599a401f43e8bc3082968ae9b4bb01401d9401f83f523fb599c2a9670fa12e7634f6037e39ed3cf0e6e1cda8f89ac696af78245106a55a0fc0aa39a01f7393f970599a401f43e8bc3082968ae9b4bb01401d9401f83f523fb599c2a9670fa12e7634f6037e39ed3cf0e6e1cda8f89ac696af78245106a55a0fc0aa39a010000000900000002f7393f970599a401f43e8bc3082968ae9b4bb01401d9401f83f523fb599c2a9670fa12e7634f6037e39ed3cf0e6e1cda8f89ac696af78245106a55a0fc0aa39a01f7393f970599a401f43e8bc3082968ae9b4bb01401d9401f83f523fb599c2a9670fa12e7634f6037e39ed3cf0e6e1cda8f89ac696af78245106a55a0fc0aa39a01");
}

TEST(TWAnySignerAvalanche, SignExportTransaction) {
    Proto::SigningInput input;
    setUpDefaultPrivateKeyData(input);
    auto &inputTx = *input.mutable_input_tx();
    auto &exportTx = *inputTx.mutable_export_tx();
    auto &baseTx = *exportTx.mutable_base_tx();
    setUpDefaultBaseTx(baseTx);
    
    auto blockchainID = CB58::avalanche.decode("2eNy1mUFdmaxXNj1eQHUe7Np4gju9sJsEtWQ4MX3ToiNKuADed");
    Data destChain(blockchainID.begin(), blockchainID.begin() + BLOCKCHAIN_ID_SIZE); // we just want the first 32 bytes, no checksum
    exportTx.set_destination_chain(destChain.data(), destChain.size());

    auto assetIDBytes = parse_hex("0xdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db");     
    {
        auto output = exportTx.add_outs();
        output->set_asset_id(assetIDBytes.data(), assetIDBytes.size());
        auto addressesOut = generateAddressesForAnySigner();
        auto secpTransferOut = output->mutable_output()->mutable_secp_transfer_output();
        for (int i = 0; i < addressesOut.size(); ++i) {
            secpTransferOut->add_addresses();
            secpTransferOut->set_addresses(i, addressesOut[i].bytes.data(), addressesOut[i].bytes.size());
        }
        secpTransferOut->set_amount(12345);
        secpTransferOut->set_locktime(54321);
        secpTransferOut->set_threshold(1);
    }
    {
        auto output = exportTx.add_outs();
        output->set_asset_id(assetIDBytes.data(), assetIDBytes.size());
        auto addressesOut = generateAddressesForAnySigner();
        auto secpTransferOut = output->mutable_output()->mutable_secp_transfer_output();
        for (int i = 0; i < addressesOut.size(); ++i) {
            secpTransferOut->add_addresses();
            secpTransferOut->set_addresses(i, addressesOut[i].bytes.data(), addressesOut[i].bytes.size());
        }
        secpTransferOut->set_amount(3);
        secpTransferOut->set_locktime(2);
        secpTransferOut->set_threshold(1);
    }
    Proto::SigningOutput output;
    ANY_SIGN(input, TWCoinTypeAvalancheXChain);
    ASSERT_EQ(hex(output.encoded()), "00000000000400003039d891ad56056d9c01f18f43f58b5c784ad07a4a49cf3d1f11623804b5cba2c6bf00000002dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000700000000000003e8000000000000000000000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29cdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db000000070000000000003039000000000000d43100000001000000013cb7d3842e8cee6a0ebd09f1fe884f6861e1b29c00000002f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd15000000020000000300000007f1e1d1c1b1a191817161514131211101f0e0d0c0b0a09080706050403020100000000005dbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db0000000500000000075bcd1500000002000000030000000700000004deadbeef000000020000000900000002a964ef14456ddfa1b4f969c18ec1a139806da2a3869b4205f160a5ad998b075f692b4a052680587a677f1a609cec9e147f2053f676c8b989366f7f84fba17a4e00a964ef14456ddfa1b4f969c18ec1a139806da2a3869b4205f160a5ad998b075f692b4a052680587a677f1a609cec9e147f2053f676c8b989366f7f84fba17a4e000000000900000002a964ef14456ddfa1b4f969c18ec1a139806da2a3869b4205f160a5ad998b075f692b4a052680587a677f1a609cec9e147f2053f676c8b989366f7f84fba17a4e00a964ef14456ddfa1b4f969c18ec1a139806da2a3869b4205f160a5ad998b075f692b4a052680587a677f1a609cec9e147f2053f676c8b989366f7f84fba17a4e00");
}

std::vector<PublicKey> generateAddressesForAnySigner() {
    // build some arbitrary addresses for this test
    std::vector<PublicKey> arbitraryAddresses;
    {   
        auto privateKey = PrivateKey(parse_hex("0x98cb077f972feb0481f1d894f272c6a1e3c15e272a1658ff716444f465200070"));
        const auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeSECP256k1);
        arbitraryAddresses.push_back(publicKey);
    }
    {   
        auto privateKey = PrivateKey(parse_hex("0x88cb770f972feb0481f1d894f272c6a1e3c15e272a1658ff716444f465200070"));
        const auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeSECP256k1);
        arbitraryAddresses.push_back(publicKey);
    }
    {   
        auto privateKey = PrivateKey(parse_hex("0x78cb077f972fe00481f0d894f272c6a1e3c15e272a1658ff716444f465200070"));
        const auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeSECP256k1);
        arbitraryAddresses.push_back(publicKey);
    }
    return arbitraryAddresses;
}

const std::vector<uint8_t> getDefaultPrivateKeyBytes() {
    const auto privateKeyBytes = CB58::avalanche.decode("ewoqjP7PxY4yr3iLTpLisriqt94hdyDFNgchSxGGztUrTXtNN");
    const std::vector<uint8_t> privateKeyBytesNoChecksum(privateKeyBytes.begin(), privateKeyBytes.begin() + 32); // we just want the first 32 bytes
    return privateKeyBytesNoChecksum; 
}

void setUpDefaultPrivateKeyData(Proto::SigningInput &input) {
    auto privateKeyBytes = getDefaultPrivateKeyBytes();
    input.add_private_keys();
    input.set_private_keys(0, privateKeyBytes.data(), privateKeyBytes.size());
}

void setUpDefaultBaseTx(TW::Avalanche::Proto::BaseTx &baseTx) {
    const auto privateKey = PrivateKey(getDefaultPrivateKeyBytes()); 
    const auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeSECP256k1);
    // keep in mind the expected formats in Signer!
    // for example, addresses are expected as secp pubkey, not avalanche-formatted address

    
    auto blockchainID = CB58::avalanche.decode("2eNy1mUFdmaxXNj1eQHUe7Np4gju9sJsEtWQ4MX3ToiNKuADed");
    Data blockchainIDBytes(blockchainID.begin(), blockchainID.begin() + 32); // we just want the first 32 bytes, no checksum
    uint32_t netID = 12345;
    auto assetID = "0xdbcf890f77f49b96857648b72b77f9f82937f28a68704af05da0dc12ba53f2db";
    auto assetIDBytes = parse_hex(assetID);
    auto txIDBytes = parse_hex("0xf1e1d1c1b1a191817161514131211101f0e0d0c0b0a090807060504030201000"); 
    Data memo = {0xde, 0xad, 0xbe, 0xef};
    auto amount = 1000;   
    auto locktime = 0;
    auto threshold = 1;

    baseTx.set_network_id(netID);
    baseTx.set_blockchain_id(blockchainIDBytes.data(), blockchainIDBytes.size());
    baseTx.set_memo(memo.data(), memo.size());
    
    auto coreInputOne = new Proto::SECP256K1TransferInput();
    coreInputOne->set_amount(123456789);
    coreInputOne->add_address_indices(3);
    coreInputOne->add_address_indices(7);
    auto wrappedInputOne = new Proto::TransactionInput();
    wrappedInputOne->set_allocated_secp_transfer_input(coreInputOne);
    auto inputOne = baseTx.add_inputs();
    inputOne->set_utxo_index(5);
    inputOne->set_tx_id(txIDBytes.data(), txIDBytes.size());
    inputOne->set_asset_id(assetIDBytes.data(), assetIDBytes.size());
    for (auto i = 0; i < 8; ++i) {
        inputOne->add_spendable_addresses();
        inputOne->set_spendable_addresses(i, publicKey.bytes.data(), publicKey.bytes.size());
    }
    inputOne->set_allocated_input(wrappedInputOne);
    auto coreInputTwo = new Proto::SECP256K1TransferInput();
    coreInputTwo->set_amount(123456789);
    coreInputTwo->add_address_indices(3);
    coreInputTwo->add_address_indices(7);
    auto wrappedInputTwo = new Proto::TransactionInput();
    wrappedInputTwo->set_allocated_secp_transfer_input(coreInputTwo);
    auto inputTwo = baseTx.add_inputs();
    inputTwo->set_utxo_index(5);
    inputTwo->set_tx_id(txIDBytes.data(), txIDBytes.size());
    inputTwo->set_asset_id(assetIDBytes.data(), assetIDBytes.size());
    for (auto i = 0; i < 8; ++i) {
        inputTwo->add_spendable_addresses();
        inputTwo->set_spendable_addresses(i, publicKey.bytes.data(), publicKey.bytes.size());
    }
    inputTwo->set_allocated_input(wrappedInputTwo);

    auto coreOutputOne = new Proto::SECP256K1TransferOutput();
    coreOutputOne->set_amount(12345);
    coreOutputOne->set_locktime(54321);
    coreOutputOne->set_threshold(threshold);
    coreOutputOne->add_addresses();
    coreOutputOne->set_addresses(0, publicKey.bytes.data(), publicKey.bytes.size());
    auto wrappedOutputOne = new Proto::TransactionOutput();
    wrappedOutputOne->set_allocated_secp_transfer_output(coreOutputOne);
    auto outputOne = baseTx.add_outputs();
    outputOne->set_asset_id(assetIDBytes.data(), assetIDBytes.size());
    outputOne->set_allocated_output(wrappedOutputOne);

    auto coreOutputTwo = new Proto::SECP256K1TransferOutput();
    coreOutputTwo->set_amount(amount);
    coreOutputTwo->set_locktime(locktime);
    coreOutputTwo->set_threshold(threshold);
    coreOutputTwo->add_addresses();
    coreOutputTwo->set_addresses(0, publicKey.bytes.data(), publicKey.bytes.size());    
    auto wrappedOutputTwo = new Proto::TransactionOutput();
    wrappedOutputTwo->set_allocated_secp_transfer_output(coreOutputTwo);
    auto outputTwo = baseTx.add_outputs();
    outputTwo->set_asset_id(assetIDBytes.data(), assetIDBytes.size());
    outputTwo->set_allocated_output(wrappedOutputTwo);
}