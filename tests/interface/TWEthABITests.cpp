// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <TrustWalletCore/TWEthAbiFunction.h>
#include <TrustWalletCore/TWString.h>

#include "Ethereum/ABI.h"
#include "Data.h"
#include "HexCoding.h"
#include "uint256.h"

#include <gtest/gtest.h>
#include <string>

namespace TW::Ethereum {

TEST(TWABI, FuncCreateEmpty) {
    TWEthAbiFunction* func = TWEthAbiFunctionCreateWithString(TWStringCreateWithUTF8Bytes("baz"));
    EXPECT_TRUE(func != nullptr);

    TWString* type = TWEthAbiFunctionGetType(func);
    std::string type2 = TWStringUTF8Bytes(type);
    EXPECT_EQ("baz()", type2);

    // delete
    TWEthAbiFunctionDelete(func);
}

TEST(TWABI, FuncCreate1) {
    TWEthAbiFunction* func = TWEthAbiFunctionCreateWithString(TWStringCreateWithUTF8Bytes("baz"));
    EXPECT_TRUE(func != nullptr);

    int p1index = TWEthAbiFunctionAddInParamInt32(func, 69);
    EXPECT_EQ(0, p1index);
    int p2index = TWEthAbiFunctionAddOutParamInt32(func);
    EXPECT_EQ(0, p2index);
    // check back get value
    int p2val2 = TWEthAbiFunctionGetOutParamInt32(func, p2index);
    EXPECT_EQ(0, p2val2);

    TWString* type = TWEthAbiFunctionGetType(func);
    std::string type2 = TWStringUTF8Bytes(type);
    EXPECT_EQ("baz(int32)", type2);

    // delete
    TWEthAbiFunctionDelete(func);
}

TEST(TWABI, FuncCreate2) {
    TWEthAbiFunction* func = TWEthAbiFunctionCreateWithString(TWStringCreateWithUTF8Bytes("baz"));
    EXPECT_TRUE(func != nullptr);

    TWString* p1valStr = TWStringCreateWithUTF8Bytes("0045");
    TWData* p1val = TWDataCreateWithHexString(p1valStr);
    int p1index = TWEthAbiFunctionAddInParamUInt256(func, p1val);
    EXPECT_EQ(0, p1index);
    //TWDataDelete(p1val);
    TWStringDelete(p1valStr);

    int p2index = TWEthAbiFunctionAddOutParamUInt256(func);
    EXPECT_EQ(0, p2index);

    // check back get value
    TWData* p2val2 = TWEthAbiFunctionGetOutParamUInt256(func, p2index);
    Data p2val3 = data(TWDataBytes(p2val2), TWDataSize(p2val2));
    EXPECT_EQ("00", hex(p2val3));

    TWString* type = TWEthAbiFunctionGetType(func);
    EXPECT_EQ("baz(uint256)", std::string(TWStringUTF8Bytes(type)));

    // delete
    TWEthAbiFunctionDelete(func);
}

TEST(TWABI, EncodeFuncCase1) {
    TWEthAbiFunction* func = TWEthAbiFunctionCreateWithString(TWStringCreateWithUTF8Bytes("sam"));
    EXPECT_TRUE(func != nullptr);
    
    EXPECT_EQ(0, TWEthAbiFunctionAddInParamByteArray(func, TWDataCreateWithHexString(TWStringCreateWithUTF8Bytes("64617665"))));
    EXPECT_EQ(1, TWEthAbiFunctionAddInParamBool(func, true));
    int paramArrIdx = TWEthAbiFunctionAddInParamArray(func);
    EXPECT_EQ(2, paramArrIdx);
    EXPECT_EQ(0, TWEthAbiFunctionAddInArrayParamUInt256(func, paramArrIdx, TWDataCreateWithHexString(TWStringCreateWithUTF8Bytes("01"))));
    EXPECT_EQ(1, TWEthAbiFunctionAddInArrayParamUInt256(func, paramArrIdx, TWDataCreateWithHexString(TWStringCreateWithUTF8Bytes("02"))));
    EXPECT_EQ(2, TWEthAbiFunctionAddInArrayParamUInt256(func, paramArrIdx, TWDataCreateWithHexString(TWStringCreateWithUTF8Bytes("03"))));

    TWString* type = TWEthAbiFunctionGetType(func);
    EXPECT_EQ("sam(bytes,bool,uint256[])", std::string(TWStringUTF8Bytes(type)));

    TWData* encoded = TWEthAbiFunctionEncode(func);
    Data enc2 = data(TWDataBytes(encoded), TWDataSize(encoded));
    EXPECT_EQ("a5643bf2"
        "0000000000000000000000000000000000000000000000000000000000000060"
        "0000000000000000000000000000000000000000000000000000000000000001"
        "00000000000000000000000000000000000000000000000000000000000000a0"
        "0000000000000000000000000000000000000000000000000000000000000004"
        "6461766500000000000000000000000000000000000000000000000000000000"
        "0000000000000000000000000000000000000000000000000000000000000003"
        "0000000000000000000000000000000000000000000000000000000000000001"
        "0000000000000000000000000000000000000000000000000000000000000002"
        "0000000000000000000000000000000000000000000000000000000000000003",
        hex(enc2));

    // delete
    TWEthAbiFunctionDelete(func);
}

TEST(TWABI, EncodeFuncCase2) {
    TWEthAbiFunction* func = TWEthAbiFunctionCreateWithString(TWStringCreateWithUTF8Bytes("f"));
    EXPECT_TRUE(func != nullptr);
    
    EXPECT_EQ(0, TWEthAbiFunctionAddInParamUInt256(func, TWDataCreateWithHexString(TWStringCreateWithUTF8Bytes("0123"))));
    int paramArrIdx = TWEthAbiFunctionAddInParamArray(func);
    EXPECT_EQ(1, paramArrIdx);
    EXPECT_EQ(0, TWEthAbiFunctionAddInArrayParamUInt32(func, paramArrIdx, 0x456));
    EXPECT_EQ(1, TWEthAbiFunctionAddInArrayParamUInt32(func, paramArrIdx, 0x789));
    EXPECT_EQ(2, TWEthAbiFunctionAddInParamByteArrayFix(func, 10, TWDataCreateWithHexString(TWStringCreateWithUTF8Bytes("31323334353637383930"))));
    EXPECT_EQ(3, TWEthAbiFunctionAddInParamString(func, TWStringCreateWithUTF8Bytes("Hello, world!")));

    TWString* type = TWEthAbiFunctionGetType(func);
    EXPECT_EQ("f(uint256,uint32[],bytes10,string)", std::string(TWStringUTF8Bytes(type)));

    TWData* encoded = TWEthAbiFunctionEncode(func);
    Data enc2 = data(TWDataBytes(encoded), TWDataSize(encoded));
    EXPECT_EQ("47b941bf"
        "0000000000000000000000000000000000000000000000000000000000000123"
        "0000000000000000000000000000000000000000000000000000000000000080"
        "3132333435363738393000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000000000000000e0"
        "0000000000000000000000000000000000000000000000000000000000000002"
        "0000000000000000000000000000000000000000000000000000000000000456"
        "0000000000000000000000000000000000000000000000000000000000000789"
        "000000000000000000000000000000000000000000000000000000000000000d"
        "48656c6c6f2c20776f726c642100000000000000000000000000000000000000",
        hex(enc2));

    // delete
    TWEthAbiFunctionDelete(func);
}

} // namespace TW::Ethereum