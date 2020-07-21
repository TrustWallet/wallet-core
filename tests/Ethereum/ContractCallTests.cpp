// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Ethereum/ContractCall.h"
#include "HexCoding.h"

#include <fstream>
#include <gtest/gtest.h>

using namespace TW;
using namespace TW::Ethereum;

extern std::string TESTS_ROOT;

static nlohmann::json load_json(std::string path) {
    std::ifstream stream(path);
    nlohmann::json json;
    stream >> json;
    return json;
}

TEST(ContractCall, Approval) {
    auto path = TESTS_ROOT + "/Ethereum/Data/erc20.json";
    auto abi = load_json(path);
    auto call = parse_hex("095ea7b30000000000000000000000005aaeb6053f3e94c9b9a09f33669435e7ef1beaed"
                          "0000000000000000000000000000000000000000000000000000000000000001");
    auto decoded = decodeCall(call, abi);

    auto expected =
        R"|({"function":"approve(address,uint256)","inputs":[{"name":"_spender","type":"address","value":"0x5aaeb6053f3e94c9b9a09f33669435e7ef1beaed"},{"name":"_value","type":"uint256","value":"1"}]})|";

    EXPECT_EQ(decoded.value(), expected);
}

TEST(ContractCall, UniswapSwapTokens) {
    auto path = TESTS_ROOT + "/Ethereum/Data/uniswap_router_v2.json";
    auto abi = load_json(path);
    // https://etherscan.io/tx/0x57a2414f3cd9ca373b7e663ae67ecf933e40cb77a6e4ed28e4e28b5aa0d8ec63
    auto call = parse_hex(
        "0x38ed17390000000000000000000000000000000000000000000000000de0b6b3a76400000000000000000000"
        "00000000000000000000000000000000229f7e501ad62bdb000000000000000000000000000000000000000000"
        "00000000000000000000a00000000000000000000000007d8bf18c7ce84b3e175b339c4ca93aed1dd166f10000"
        "00000000000000000000000000000000000000000000000000005f0ed070000000000000000000000000000000"
        "00000000000000000000000000000000040000000000000000000000006b175474e89094c44da98b954eedeac4"
        "95271d0f0000000000000000000000009f8f72aa9304c8b593d555f12ef6589cc3a579a2000000000000000000"
        "000000c02aaa39b223fe8d0a0e5c4f27ead9083c756cc2000000000000000000000000e41d2489571d32218924"
        "6dafa5ebde1f4699f498");
    auto decoded = decodeCall(call, abi);
    auto expected =
        R"|({"function":"swapExactTokensForTokens(uint256,uint256,address[],address,uint256)","inputs":[{"name":"amountIn","type":"uint256","value":"1000000000000000000"},{"name":"amountOutMin","type":"uint256","value":"2494851601099271131"},{"name":"path","type":"address[]","value":["0x6b175474e89094c44da98b954eedeac495271d0f","0x9f8f72aa9304c8b593d555f12ef6589cc3a579a2","0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2","0xe41d2489571d322189246dafa5ebde1f4699f498"]},{"name":"to","type":"address","value":"0x7d8bf18c7ce84b3e175b339c4ca93aed1dd166f1"},{"name":"deadline","type":"uint256","value":"1594806384"}]})|";

    EXPECT_EQ(decoded.value(), expected);
}

TEST(ContractCall, KyberTrade) {
    auto path = TESTS_ROOT + "/Ethereum/Data/kyber_proxy.json";
    auto abi = load_json(path);

    // https://etherscan.io/tx/0x51ffab782b9a27d754389505d5a50db525c04c68142ce20512d579f10f9e13e4
    auto call = parse_hex(
        "ae591d54000000000000000000000000eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee000000000000000000"
        "000000000000000000000000000004a97d605a3b980000000000000000000000000000dac17f958d2ee523a220"
        "6206994597c13d831ec70000000000000000000000007755297c6a26d495739206181fe81646dbd0bf39ffffff"
        "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff00000000000000000000000000000000"
        "000000000000000ce32ff7d63c35d189000000000000000000000000440bbd6a888a36de6e2f6a25f65bc4e168"
        "74faa9000000000000000000000000000000000000000000000000000000000000000800000000000000000000"
        "000000000000000000000000000000000000000001200000000000000000000000000000000000000000000000"
        "000000000000000000");
    auto decoded = decodeCall(call, abi);

    auto expected =
        R"|({"function":"tradeWithHintAndFee(address,uint256,address,address,uint256,uint256,address,uint256,bytes)","inputs":[{"name":"src","type":"address","value":"0xeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"},{"name":"srcAmount","type":"uint256","value":"86000000000000000000"},{"name":"dest","type":"address","value":"0xdac17f958d2ee523a2206206994597c13d831ec7"},{"name":"destAddress","type":"address","value":"0x7755297c6a26d495739206181fe81646dbd0bf39"},{"name":"maxDestAmount","type":"uint256","value":"115792089237316195423570985008687907853269984665640564039457584007913129639935"},{"name":"minConversionRate","type":"uint256","value":"237731504554534883721"},{"name":"platformWallet","type":"address","value":"0x440bbd6a888a36de6e2f6a25f65bc4e16874faa9"},{"name":"platformFeeBps","type":"uint256","value":"8"},{"name":"hint","type":"bytes","value":"0x"}]})|";

    EXPECT_EQ(decoded.value(), expected);
}

TEST(ContractCall, ApprovalForAll) {
    auto path = TESTS_ROOT + "/Ethereum/Data/erc721.json";
    auto abi = load_json(path);

    // https://etherscan.io/tx/0xc2744000a107aee4761cf8a638657f91c3003a54e2f1818c37d781be7e48187a
    auto call = parse_hex("0xa22cb46500000000000000000000000088341d1a8f672d2780c8dc725902aae72f143b"
                          "0c0000000000000000000000000000000000000000000000000000000000000001");
    auto decoded = decodeCall(call, abi);

    auto expected =
        R"|({"function":"setApprovalForAll(address,bool)","inputs":[{"name":"to","type":"address","value":"0x88341d1a8f672d2780c8dc725902aae72f143b0c"},{"name":"approved","type":"bool","value":true}]})|";

    EXPECT_EQ(decoded.value(), expected);
}

TEST(ContractCall, SetString) {
    auto call = parse_hex("c47f00270000000000000000000000000000000000000000000000000000000000000020"
                          "000000000000000000000000000000000000000000000000000000000000000864656164"
                          "62656566000000000000000000000000000000000000000000000000");
    auto abi = nlohmann::json::parse(
        R"|({"c47f0027":{"constant":false,"inputs":[{"name":"name","type":"string"}],"name":"setName","outputs":[],"payable":false,"stateMutability":"nonpayable","type":"function"}})|");
    auto decoded = decodeCall(call, abi);
    auto expected =
        R"|({"function":"setName(string)","inputs":[{"name":"name","type":"string","value":"deadbeef"}]})|";

    EXPECT_EQ(decoded.value(), expected);
}

TEST(ContractCall, Invalid) {
    EXPECT_FALSE(decodeCall(Data(), "").has_value());
    EXPECT_FALSE(decodeCall(parse_hex("0xa22cb465"), "").has_value());
}
