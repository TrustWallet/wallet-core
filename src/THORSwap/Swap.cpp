// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Swap.h"

#include <TrustWalletCore/TWCoinType.h>
#include "Coin.h"
// BTC
#include "Bitcoin/SigHashType.h"
#include "../proto/Bitcoin.pb.h"
// ETH
#include "Ethereum/Address.h"
#include "Ethereum/ABI/Function.h"
#include "Ethereum/ABI/ParamBase.h"
#include "Ethereum/ABI/ParamAddress.h"
#include "uint256.h"
#include "../proto/Ethereum.pb.h"
// BNB
#include "Binance/Address.h"
#include "../proto/Binance.pb.h"

#include <cstdlib>

/*
 * References:
 *  https://gitlab.com/thorchain/asgardex-common/asgardex-util
 */

namespace TW::THORSwap {

TWCoinType chainCoinType(Chain chain) {
    switch (chain) {
        //case Chain::THOR: return TWCoinTypeBitcoin; // TODO
        case Chain::BTC: return TWCoinTypeBitcoin;
        case Chain::ETH: return TWCoinTypeEthereum;
        case Chain::BNB: return TWCoinTypeBinance;
        default: return TWCoinTypeBitcoin;
    }
}

std::string chainName(Chain chain) {
    switch (chain) {
        case Chain::THOR: return "THOR";
        case Chain::BTC: return "BTC";
        case Chain::ETH: return "ETH";
        case Chain::BNB: return "BNB";
        default: return "";
    }
}

std::string Swap::buildMemo(Chain toChain, const std::string& toSymbol, const std::string& toAddress, uint64_t limit) {
    std::string prefix = "SWAP";
    if (toChain == Chain::ETH) {
        prefix = "=";
    }
    return prefix + ":" + chainName(toChain) + "." + toSymbol + ":" + toAddress + ":" + std::to_string(limit);
}

bool validateAddress(Chain chain, const std::string& address) {
    if (chain == Chain::THOR) { return true;} // TODO remove once ThorChain is supported
    return TW::validateAddress(chainCoinType(chain), address);
}

std::pair<Data, std::string> Swap::build(
    Chain fromChain,
    Chain toChain,
    const std::string& fromAddress,
    const std::string& toSymbol,
    const std::string& toTokenId,
    const std::string& toAddress,
    const std::string& vaultAddress,
    const std::string& routerAddress,
    const std::string& fromAmount,
    const std::string& toAmountLimit
)  {
    if (!validateAddress(fromChain, fromAddress)) {
        return std::make_pair<Data, std::string>({}, "Invalid from address");
    }
    if (!validateAddress(toChain, toAddress)) {
        return std::make_pair<Data, std::string>({}, "Invalid to address");
    }

    uint64_t fromAmountNum = std::atoll(fromAmount.c_str());
    uint64_t toAmountLimitNum = std::atoll(toAmountLimit.c_str());
    auto memo = buildMemo(toChain, toSymbol, toAddress, toAmountLimitNum);

    switch (fromChain) {
        case Chain::BTC: {
            Data out;
            auto res = buildBitcoin(toChain, toSymbol, toTokenId, fromAddress, toAddress, vaultAddress, fromAmountNum, memo, out);
            return std::make_pair(std::move(out), std::move(res));
        }

        case Chain::ETH: {
            Data out;
            auto res = buildEthereum(toChain, toSymbol, toTokenId, fromAddress, toAddress, vaultAddress, routerAddress, fromAmountNum, memo, out);
            return std::make_pair(std::move(out), std::move(res));
        }

        case Chain::BNB: {
            Data out;
            auto res = buildBinance(toChain, toSymbol, toTokenId, fromAddress, toAddress, vaultAddress, fromAmountNum, memo, out);
            return std::make_pair(std::move(out), std::move(res));
        }

        default:
            return std::make_pair<Data, std::string>({}, "Invalid from chain: " + std::to_string(toChain));
    }
}

std::string Swap::buildBitcoin(Chain toChain, const std::string& toSymbol, const std::string& toTokenId, const std::string& fromAddress, const std::string& toAddress, const std::string& vaultAddress, uint64_t amount, const std::string& memo, Data& out) {
    auto input = Bitcoin::Proto::SigningInput();

    // Following fields need to be set after building, before sending
    input.set_hash_type(TWBitcoinSigHashTypeAll);
    input.set_byte_fee(1);
    input.set_use_max_amount(false);
    // private_key
    // utxo
    // scripts
    // ... end

    input.set_amount(amount);
    input.set_to_address(vaultAddress);
    input.set_change_address(fromAddress);
    input.set_coin_type(TWCoinTypeBitcoin);
    input.set_output_op_return(memo);

    auto serialized = input.SerializeAsString();
    out.insert(out.end(), serialized.begin(), serialized.end());
    return "";
}

Data ethAddressStringToData(const std::string& asString) {
    Data asData;
    asData.resize(20);
    if (asString.empty()) {
        return asData;
    }
    auto address = Ethereum::Address(asString);
    std::copy(address.bytes.begin(), address.bytes.end(), asData.data());
    return asData;
}

std::string Swap::buildEthereum(Chain toChain, const std::string& toSymbol, const std::string& toTokenId, const std::string& fromAddress, const std::string& toAddress, const std::string& vaultAddress, const std::string& routerAddress, uint64_t amount, const std::string& memo, Data& out) {
    auto input = Ethereum::Proto::SigningInput();

    // some sanity check / address conversion
    Data vaultAddressBin = ethAddressStringToData(vaultAddress);
    if (!Ethereum::Address::isValid(vaultAddress) || vaultAddressBin.size() != Ethereum::Address::size) {
        return "Invalid vault address: " + vaultAddress;
    }
    if (!Ethereum::Address::isValid(routerAddress)) {
        return "Invalid router address: " + routerAddress;
    }
    Data toAssetAddressBin = ethAddressStringToData(toTokenId);
    if (!Ethereum::Address::isValid(toTokenId) || toAssetAddressBin.size() != Ethereum::Address::size) {
        return "Invalid assetId: " + toTokenId;
    }

    // Following fields need to be set after building, before sending
    auto chainId = store(uint256_t(1));
    input.set_chain_id(chainId.data(), chainId.size());
    auto nonce = store(uint256_t(11));
    input.set_nonce(nonce.data(), nonce.size());
    auto gasPrice = store(uint256_t(10000000000));
    input.set_gas_price(gasPrice.data(), gasPrice.size());
    auto gasLimit = store(uint256_t(1000000));
    input.set_gas_limit(gasLimit.data(), gasLimit.size());
    input.set_private_key("");
    // ... end

    input.set_to_address(routerAddress);
    auto& transfer = *input.mutable_transaction()->mutable_contract_generic();
    auto func = Ethereum::ABI::Function("deposit", std::vector<std::shared_ptr<Ethereum::ABI::ParamBase>>{
        std::make_shared<Ethereum::ABI::ParamAddress>(vaultAddressBin),
        std::make_shared<Ethereum::ABI::ParamAddress>(toAssetAddressBin),
        std::make_shared<Ethereum::ABI::ParamUInt256>(uint256_t(amount)),
        std::make_shared<Ethereum::ABI::ParamString>(memo)
    });
    Data payload;
    func.encode(payload);
    transfer.set_data(payload.data(), payload.size());
    Data amountData = store(uint256_t(amount));
    transfer.set_amount(amountData.data(), amountData.size());

    auto serialized = input.SerializeAsString();
    out.insert(out.end(), serialized.begin(), serialized.end());
    return "";
}

std::string Swap::buildBinance(Chain toChain, const std::string& toSymbol, const std::string& toTokenId, const std::string& fromAddress, const std::string& toAddress, const std::string& vaultAddress, uint64_t amount, const std::string& memo, Data& out) {
    auto input = Binance::Proto::SigningInput();

    // Following fields need to be set after building, before sending
    input.set_chain_id("Binance-Chain-Nile");
    input.set_account_number(0);
    input.set_sequence(0);
    input.set_source(0);
    input.set_private_key("");
    // ... end

    input.set_memo(memo);

    auto& order = *input.mutable_send_order();

    auto token = Binance::Proto::SendOrder::Token();
    token.set_denom("BNB");
    token.set_amount(amount);
    {
        Binance::Address fromAddressBin;
        Binance::Address::decode(fromAddress, fromAddressBin);
        auto input = order.add_inputs();
        input->set_address(fromAddressBin.getKeyHash().data(), fromAddressBin.getKeyHash().size());
        *input->add_coins() = token;
    }
    {
        Binance::Address vaultAddressBin;
        Binance::Address::decode(vaultAddress, vaultAddressBin);
        auto output = order.add_outputs();
        output->set_address(vaultAddressBin.getKeyHash().data(), vaultAddressBin.getKeyHash().size());
        *output->add_coins() = token;
    }

    auto serialized = input.SerializeAsString();
    out.insert(out.end(), serialized.begin(), serialized.end());
    return "";
}

} // namespace TW
