// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Ont.h"
#include "ParamsBuilder.h"

#include <unordered_map>

#include <TrezorCrypto/rand.h>

using namespace TW;
using namespace TW::Ontology;

Transaction Ont::balanceOf(const std::string &address) {
    auto builder = ParamsBuilder();
    auto invokeCode = ParamsBuilder::buildNativeInvokeCode(contractAddress(), version, "balanceOf", Address(address).data);
    auto tx = Transaction((uint8_t) 0, txType, random32(), (uint64_t) 0, (uint64_t) 0, (std::string) "", invokeCode);
    return tx;
}

Transaction Ont::transfer(const Account &from, std::string &to, uint64_t amount, const Account &payer, uint64_t gasPrice, uint64_t gasLimit) {
    auto toAddress = Address(to);
    std::unordered_map<std::string, boost::any> transferParam{{"from", from.getAddress().data}, {"to", toAddress.data}, {"amount", amount}};
    std::vector<boost::any> args{transferParam};
    auto invokeCode = ParamsBuilder::buildNativeInvokeCode(ontContract, 0x00, "transfer", args);
    auto tx = Transaction(version, txType, random32(), gasPrice, gasLimit, payer.getAddress().string(), invokeCode);
    return tx;
}
