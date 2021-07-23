// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Ethereum/ABI.h"
#include "HexCoding.h"
#include "../interface/TWTestUtilities.h"

#include <gtest/gtest.h>

using namespace TW;
using namespace TW::Ethereum::ABI;


///// Parameter types

TEST(EthereumAbi, ParamTypeNames) {
    EXPECT_EQ("uint8", ParamUInt8().getType());
    EXPECT_EQ("uint16", ParamUInt16().getType());
    EXPECT_EQ("uint32", ParamUInt32().getType());
    EXPECT_EQ("uint64", ParamUInt64().getType());
    EXPECT_EQ("uint256", ParamUInt256().getType());
    EXPECT_EQ("uint168", ParamUIntN(168).getType());
    EXPECT_EQ("int8", ParamInt8().getType());
    EXPECT_EQ("int16", ParamInt16().getType());
    EXPECT_EQ("int32", ParamInt32().getType());
    EXPECT_EQ("int64", ParamInt64().getType());
    EXPECT_EQ("int256", ParamInt256().getType());
    EXPECT_EQ("int168", ParamIntN(168).getType());
    EXPECT_EQ("bool", ParamBool().getType());
    EXPECT_EQ("string", ParamString().getType());
    EXPECT_EQ("address", ParamAddress().getType());
    EXPECT_EQ("bytes", ParamByteArray().getType());
    EXPECT_EQ("bytes168", ParamByteArrayFix(168).getType());
    // ParamArray: needs a child parameter to obtain type
    auto paramArray = ParamArray();
    EXPECT_EQ("empty[]", paramArray.getType());
    paramArray.addParam(std::make_shared<ParamBool>());
    EXPECT_EQ("bool[]", paramArray.getType());
}

TEST(EthereumAbi, ParamBool) {
    {
        auto param = ParamBool(false);
        EXPECT_FALSE(param.getVal());
        param.setVal(true);
        EXPECT_TRUE(param.getVal());

        EXPECT_EQ("bool", param.getType());
        EXPECT_FALSE(param.isDynamic());
        EXPECT_EQ(32, param.getSize());
    }
    {
        auto param = ParamBool(false);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("0000000000000000000000000000000000000000000000000000000000000000", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_FALSE(param.getVal());
    }
    {
        auto param = ParamBool(true);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("0000000000000000000000000000000000000000000000000000000000000001", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_TRUE(param.getVal());
    }
}

TEST(EthereumAbi, ParamUInt8) {
    {
        auto param = ParamUInt8(101);
        EXPECT_EQ(101, param.getVal());
        param.setVal(1);
        EXPECT_EQ(1, param.getVal());

        EXPECT_EQ("uint8", param.getType());
        EXPECT_FALSE(param.isDynamic());
        EXPECT_EQ(32, param.getSize());
    }
    {
        auto param = ParamUInt8(101);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("0000000000000000000000000000000000000000000000000000000000000065", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(101, param.getVal());
    }
    {
        auto param = ParamUInt8(1);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("0000000000000000000000000000000000000000000000000000000000000001", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(1, param.getVal());
    }
}

TEST(EthereumAbi, ParamUInt16) {
    {
        auto param = ParamUInt16(101);
        EXPECT_EQ(101, param.getVal());
        param.setVal(1234);
        EXPECT_EQ(1234, param.getVal());

        EXPECT_EQ("uint16", param.getType());
        EXPECT_FALSE(param.isDynamic());
        EXPECT_EQ(32, param.getSize());
    }
    {
        auto param = ParamUInt16(101);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("0000000000000000000000000000000000000000000000000000000000000065", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(101, param.getVal());
    }
    {
        auto param = ParamUInt16(1234);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("00000000000000000000000000000000000000000000000000000000000004d2", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(1234, param.getVal());
    }
}

TEST(EthereumAbi, ParamUInt32) {
    {
        auto param = ParamUInt32(101);
        EXPECT_EQ(101, param.getVal());
        param.setVal(1234);
        EXPECT_EQ(1234, param.getVal());

        EXPECT_EQ("uint32", param.getType());
        EXPECT_FALSE(param.isDynamic());
        EXPECT_EQ(32, param.getSize());
    }
    {
        auto param = ParamUInt32(101);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("0000000000000000000000000000000000000000000000000000000000000065", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(101, param.getVal());
    }
    {
        auto param = ParamUInt32(1234);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("00000000000000000000000000000000000000000000000000000000000004d2", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(1234, param.getVal());
    }
}

TEST(EthereumAbi, ParamUInt64) {
    {
        auto param = ParamUInt64(101);
        EXPECT_EQ(101, param.getVal());
        param.setVal(1234);
        EXPECT_EQ(1234, param.getVal());

        EXPECT_EQ("uint64", param.getType());
        EXPECT_FALSE(param.isDynamic());
        EXPECT_EQ(32, param.getSize());
    }
    {
        auto param = ParamUInt64(101);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("0000000000000000000000000000000000000000000000000000000000000065", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(101, param.getVal());
    }
    {
        auto param = ParamUInt64(1234);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("00000000000000000000000000000000000000000000000000000000000004d2", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(1234, param.getVal());
    }
}

TEST(EthereumAbi, ParamUInt256) {
    const uint256_t bigInt = uint256_t("0x1234567890123456789012345678901234567890");
    {
        auto param = ParamUInt256(uint256_t(101));
        EXPECT_EQ(uint256_t(101), param.getVal());
        param.setVal(uint256_t(1234));
        EXPECT_EQ(uint256_t(1234), param.getVal());

        EXPECT_EQ("uint256", param.getType());
        EXPECT_FALSE(param.isDynamic());
        EXPECT_EQ(32, param.getSize());
    }
    {
        auto param = ParamUInt256(uint256_t(101));
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("0000000000000000000000000000000000000000000000000000000000000065", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(uint256_t(101), param.getVal());
    }
    {
        auto param = ParamUInt256(uint256_t(1234));
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("00000000000000000000000000000000000000000000000000000000000004d2", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(uint256_t(1234), param.getVal());
    }
    {
        auto param = ParamUInt256(bigInt);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("0000000000000000000000001234567890123456789012345678901234567890", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(bigInt, param.getVal());
    }
    {
        auto param = ParamUInt256(uint256_t(-1));
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(uint256_t(-1), param.getVal());
    }
}

TEST(EthereumAbi, ParamUInt80) {
    {
        auto param = ParamUIntN(80, 0);
        EXPECT_EQ(0, param.getVal());
        param.setVal(100);
        EXPECT_EQ(100, param.getVal());

        EXPECT_EQ("uint80", param.getType());
        EXPECT_FALSE(param.isDynamic());
        EXPECT_EQ(32, param.getSize());

        // above number of bits, masked
        param.setVal(load(Data(parse_hex("1010101010101010101010101010101010101010101010101010101010101010"))));
        EXPECT_EQ(load(Data(parse_hex("00000010101010101010101010"))), param.getVal());
    }
    {
        auto param = ParamUIntN(80, 1);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("0000000000000000000000000000000000000000000000000000000000000001", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(1, param.getVal());
    }
    {
        auto param = ParamUIntN(80, 0x123);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("0000000000000000000000000000000000000000000000000000000000000123", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(0x123, param.getVal());
    }
}

TEST(EthereumAbi, ParamInt80) {
    // large negative, above number of bits, and its counterpart truncated to 80 bits
    int256_t largeNeg2 = ValueEncoder::int256FromUint256(load(Data(parse_hex("ffff101010101010101010101010101010101010101010101010101010101010"))));
    int256_t largeNeg1 = ValueEncoder::int256FromUint256(load(Data(parse_hex("ffffffffffffffffffffffffffffffffffffffffffff10101010101010101010"))));
    {
        auto param = ParamIntN(80, 0);
        EXPECT_EQ(0, param.getVal());
        param.setVal(int256_t(101));
        EXPECT_EQ(int256_t(101), param.getVal());

        EXPECT_EQ("int80", param.getType());
        EXPECT_FALSE(param.isDynamic());
        EXPECT_EQ(32, param.getSize());

        param.setVal(int256_t(-101));
        EXPECT_EQ(int256_t(-101), param.getVal());
        param.setVal(largeNeg1);
        EXPECT_EQ(largeNeg1, param.getVal());
        // large negative, above number of bits, masked
        param.setVal(largeNeg2);
        EXPECT_EQ(largeNeg1, param.getVal());
    }
    {
        auto param = ParamIntN(80, 1);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("0000000000000000000000000000000000000000000000000000000000000001", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(1, param.getVal());
    }
    {
        auto param = ParamIntN(80, int256_t(-1234));
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffb2e", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(int256_t(-1234), param.getVal());
    }
    {
        auto param = ParamIntN(80, largeNeg1);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("ffffffffffffffffffffffffffffffffffffffffffff10101010101010101010", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(int256_t(largeNeg1), param.getVal());
    }
    {
        auto param = ParamIntN(80, largeNeg2);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("ffffffffffffffffffffffffffffffffffffffffffff10101010101010101010", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(int256_t(largeNeg1), param.getVal());
    }
}

TEST(EthereumAbi, ParamString) {
    const std::string helloStr = "Hello World!    Hello World!    Hello World!";
    {
        auto param = ParamString(helloStr);
        EXPECT_EQ(helloStr, param.getVal());

        EXPECT_EQ("string", param.getType());
        EXPECT_TRUE(param.isDynamic());
        EXPECT_EQ(3 * 32, param.getSize());
    }
    {
        auto param = ParamString(helloStr);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ(
            "000000000000000000000000000000000000000000000000000000000000002c"
            "48656c6c6f20576f726c64212020202048656c6c6f20576f726c642120202020"
            "48656c6c6f20576f726c64210000000000000000000000000000000000000000",
            hex(encoded));
        EXPECT_EQ(3 * 32, encoded.size());
        EXPECT_EQ(3 * 32, param.getSize());
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(helloStr, param.getVal());
    }
}

TEST(EthereumAbi, ParamAddress) {
    std::string val1Str("f784682c82526e245f50975190ef0fff4e4fc077");
    Data val1(parse_hex(val1Str));
    {
        auto param = ParamAddress(val1);
        EXPECT_EQ(val1, param.getData());

        EXPECT_EQ("address", param.getType());
        EXPECT_FALSE(param.isDynamic());
        EXPECT_EQ(32, param.getSize());
    }
    {
        auto param = ParamAddress(val1);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("000000000000000000000000f784682c82526e245f50975190ef0fff4e4fc077", hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(val1, param.getData());
    }
    {
        auto param = ParamAddress(parse_hex("0000000000000000000000000000000000000012"));
        EXPECT_EQ("0000000000000000000000000000000000000012", hex(param.getData()));
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("0000000000000000000000000000000000000000000000000000000000000012", hex(encoded));
    }
    {
        auto param = ParamAddress(parse_hex("4300000000000000000000000000000000000000"));
        EXPECT_EQ("4300000000000000000000000000000000000000", hex(param.getData()));
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ("0000000000000000000000004300000000000000000000000000000000000000", hex(encoded));
    }
}

TEST(EthereumAbi, ParamByteArray) {
    Data data10 = parse_hex("31323334353637383930");
    {
        auto param = ParamByteArray(data10);
        EXPECT_EQ(data10, param.getVal());

        EXPECT_EQ("bytes", param.getType());
        EXPECT_TRUE(param.isDynamic());
        EXPECT_EQ(2 * 32, param.getSize());
    }
    {
        auto param = ParamByteArray(data10);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ(2 * 32, encoded.size());
        EXPECT_EQ(2 * 32, param.getSize());
        EXPECT_EQ(
            "000000000000000000000000000000000000000000000000000000000000000a"
            "3132333435363738393000000000000000000000000000000000000000000000", 
            hex(encoded));
        EXPECT_EQ(2 * 32, encoded.size());
        EXPECT_EQ(2 * 32, param.getSize());
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(data10, param.getVal());
    }
}

TEST(EthereumAbi, ParamByteArrayFix) {
    Data data10 = parse_hex("31323334353637383930");
    {
        auto param = ParamByteArrayFix(10, data10);
        EXPECT_EQ(data10, param.getVal());

        EXPECT_EQ("bytes10", param.getType());
        EXPECT_FALSE(param.isDynamic());
        EXPECT_EQ(32, param.getSize());
    }
    {
        auto param = ParamByteArrayFix(10, data10);
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ(32, encoded.size());
        EXPECT_EQ(32, param.getSize());
        EXPECT_EQ(
            "3132333435363738393000000000000000000000000000000000000000000000", 
            hex(encoded));
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(data10, param.getVal());
    }
}

TEST(EthereumAbi, ParamArrayByte) {
    {
        auto param = ParamArray();
        param.addParam(std::make_shared<ParamUInt8>(49));
        param.addParam(std::make_shared<ParamUInt8>(50));
        param.addParam(std::make_shared<ParamUInt8>(51));
        EXPECT_EQ(3, param.getVal().size());
        EXPECT_EQ(49, (std::dynamic_pointer_cast<ParamUInt8>(param.getVal()[0]))->getVal());
        EXPECT_EQ(51, (std::dynamic_pointer_cast<ParamUInt8>(param.getVal()[2]))->getVal());

        EXPECT_EQ("uint8[]", param.getType());
        EXPECT_TRUE(param.isDynamic());
        EXPECT_EQ((3 + 1) * 32, param.getSize());
        EXPECT_EQ(3, param.getCount());
    }
    {
        auto param = ParamArray();
        param.addParam(std::make_shared<ParamUInt8>(49));
        param.addParam(std::make_shared<ParamUInt8>(50));
        param.addParam(std::make_shared<ParamUInt8>(51));
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ(4 * 32, encoded.size());
        EXPECT_EQ(4 * 32, param.getSize());
        EXPECT_EQ(
            "0000000000000000000000000000000000000000000000000000000000000003"
            "0000000000000000000000000000000000000000000000000000000000000031"
            "0000000000000000000000000000000000000000000000000000000000000032"
            "0000000000000000000000000000000000000000000000000000000000000033",
            hex(encoded));
        EXPECT_EQ(4 * 32, encoded.size());
        EXPECT_EQ(4 * 32, param.getSize());
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(3, param.getVal().size());
        EXPECT_EQ(49, (std::dynamic_pointer_cast<ParamUInt8>(param.getVal()[0]))->getVal());
        EXPECT_EQ(51, (std::dynamic_pointer_cast<ParamUInt8>(param.getVal()[2]))->getVal());
    }
}

TEST(EthereumAbi, ParamArrayAddress) {
    {
        auto param = ParamArray();
        param.addParam(std::make_shared<ParamAddress>(Data(parse_hex("f784682c82526e245f50975190ef0fff4e4fc077"))));
        param.addParam(std::make_shared<ParamAddress>(Data(parse_hex("2e00cd222cb42b616d86d037cc494e8ab7f5c9a3"))));
        EXPECT_EQ(2, param.getVal().size());

        EXPECT_EQ("address[]", param.getType());
        EXPECT_TRUE(param.isDynamic());
        EXPECT_EQ((2 + 1) * 32, param.getSize());
        EXPECT_EQ(2, param.getCount());
    }
    {
        auto param = ParamArray();
        param.addParam(std::make_shared<ParamAddress>(Data(parse_hex("f784682c82526e245f50975190ef0fff4e4fc077"))));
        param.addParam(std::make_shared<ParamAddress>(Data(parse_hex("2e00cd222cb42b616d86d037cc494e8ab7f5c9a3"))));
        Data encoded;
        param.encode(encoded);
        EXPECT_EQ(3 * 32, encoded.size());
        EXPECT_EQ(3 * 32, param.getSize());
        EXPECT_EQ(
            "0000000000000000000000000000000000000000000000000000000000000002"
            "000000000000000000000000f784682c82526e245f50975190ef0fff4e4fc077"
            "0000000000000000000000002e00cd222cb42b616d86d037cc494e8ab7f5c9a3",
            hex(encoded));
        EXPECT_EQ(3 * 32, encoded.size());
        EXPECT_EQ(3 * 32, param.getSize());
        size_t offset = 0;
        EXPECT_TRUE(param.decode(encoded, offset));
        EXPECT_EQ(2, param.getCount());
        EXPECT_EQ(2, param.getVal().size());
        EXPECT_EQ(
            "2e00cd222cb42b616d86d037cc494e8ab7f5c9a3", 
            hex((std::dynamic_pointer_cast<ParamAddress>(param.getVal()[1]))->getData()));
    }
}

TEST(EthereumAbi, ParamArrayOfByteArray) {
    auto param = ParamArray();
    param.addParam(std::make_shared<ParamByteArray>(parse_hex("1011")));
    param.addParam(std::make_shared<ParamByteArray>(parse_hex("102222")));
    param.addParam(std::make_shared<ParamByteArray>(parse_hex("10333333")));
    EXPECT_EQ(3, param.getVal().size());

    EXPECT_EQ("bytes[]", param.getType());
    EXPECT_TRUE(param.isDynamic());
    EXPECT_EQ((1 + 3 + 3 * 2) * 32, param.getSize());
    EXPECT_EQ(3, param.getCount());
}

TEST(EthereumAbi, ParamArrayBytesContract) {
    auto param = ParamArray();
    param.addParam(std::make_shared<ParamByteArray>(parse_hex("0xd5fa2b00e71cd96d4ba1c4b512b0c5bee30d2b6becf61e574c32a17a67156fa9ed3c4c6f00000000000000000000000047331175b23c2f067204b506ca1501c26731c990")));
    param.addParam(std::make_shared<ParamByteArray>(parse_hex("0x304e6adee71cd96d4ba1c4b512b0c5bee30d2b6becf61e574c32a17a67156fa9ed3c4c6f00000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000000")));
    param.addParam(std::make_shared<ParamByteArray>(parse_hex("0x8b95dd71e71cd96d4ba1c4b512b0c5bee30d2b6becf61e574c32a17a67156fa9ed3c4c6f000000000000000000000000000000000000000000000000000000000000003c0000000000000000000000000000000000000000000000000000000000000060000000000000000000000000000000000000000000000000000000000000001447331175b23c2f067204b506ca1501c26731c990000000000000000000000000")));
    param.addParam(std::make_shared<ParamByteArray>(parse_hex("0x8b95dd71e71cd96d4ba1c4b512b0c5bee30d2b6becf61e574c32a17a67156fa9ed3c4c6f00000000000000000000000000000000000000000000000000000000000002ca00000000000000000000000000000000000000000000000000000000000000600000000000000000000000000000000000000000000000000000000000000014d30f834b53d8f7e851e87b90ffa65757a35b8505000000000000000000000000")));
    EXPECT_EQ(4, param.getCount());
    EXPECT_EQ(4, param.getVal().size());

    EXPECT_EQ("bytes[]", param.getType());        
    EXPECT_TRUE(param.isDynamic());

    Data encoded;
    param.encode(encoded);
    EXPECT_EQ(896, encoded.size());

    EXPECT_EQ(896, param.getSize());
}

///// Direct encode & decode

TEST(EthereumAbi, EncodeVectorByte10) {
    auto p = ParamByteArrayFix(10, std::vector<byte>{0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30});
    EXPECT_EQ("bytes10", p.getType());
    Data encoded;
    p.encode(encoded);
    EXPECT_EQ("3132333435363738393000000000000000000000000000000000000000000000", hex(encoded));
}

TEST(EthereumAbi, EncodeVectorByte) {
    auto p = ParamByteArray(std::vector<byte>{0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30});
    EXPECT_EQ("bytes", p.getType());
    Data encoded;
    p.encode(encoded);
    EXPECT_EQ(
        "000000000000000000000000000000000000000000000000000000000000000a"
        "3132333435363738393000000000000000000000000000000000000000000000", hex(encoded));
}

TEST(EthereumAbi, EncodeArrayByte) {
    auto p = ParamArray(std::vector<std::shared_ptr<ParamBase>>{
        std::make_shared<ParamByteArray>(parse_hex("1011")),
        std::make_shared<ParamByteArray>(parse_hex("102222"))
    });
    EXPECT_EQ("bytes[]", p.getType());
    Data encoded;
    p.encode(encoded);
    EXPECT_EQ(
        "0000000000000000000000000000000000000000000000000000000000000002"
        "0000000000000000000000000000000000000000000000000000000000000040"
        "0000000000000000000000000000000000000000000000000000000000000080"
        "0000000000000000000000000000000000000000000000000000000000000002"
        "1011000000000000000000000000000000000000000000000000000000000000"
        "0000000000000000000000000000000000000000000000000000000000000003"
        "1022220000000000000000000000000000000000000000000000000000000000",
        hex(encoded)
    );
    EXPECT_EQ((1 + 2 + 2 * 2) * 32, encoded.size());
    EXPECT_EQ((1 + 2 + 2 * 2) * 32, p.getSize());
}

TEST(EthereumAbi, DecodeUInt) {
    Data encoded = parse_hex("000000000000000000000000000000000000000000000000000000000000002a");
    size_t offset = 0;
    uint256_t decoded;
    bool res = ParamNumberType<uint256_t>::decodeNumber(encoded, decoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(uint256_t(42), decoded);
    EXPECT_EQ(32, offset);
}

TEST(EthereumAbi, DecodeUInt8) {
    Data encoded = parse_hex("0000000000000000000000000000000000000000000000000000000000000018");
    size_t offset = 0;
    uint8_t decoded;
    bool res = ParamNumberType<uint8_t>::decodeNumber(encoded, decoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(24, decoded);
    EXPECT_EQ(32, offset);
}

TEST(EthereumAbi, DecodeUInt8WithOffset) {
    Data encoded = parse_hex("abcdef0000000000000000000000000000000000000000000000000000000000000018");
    size_t offset = 3;
    uint8_t decoded;
    bool res = ParamNumberType<uint8_t>::decodeNumber(encoded, decoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(24, decoded);
    EXPECT_EQ(3 + 32, offset);
}

TEST(EthereumAbi, DecodeUIntWithOffset) {
    Data encoded = parse_hex("abcdef000000000000000000000000000000000000000000000000000000000000002a");
    size_t offset = 3;
    uint256_t decoded;
    bool res = decode(encoded, decoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(uint256_t(42), decoded);
    EXPECT_EQ(3 + 32, offset);
}

TEST(EthereumAbi, DecodeUIntErrorTooShort) {
    Data encoded = parse_hex("000000000000000000000000000000000000000000000000002a");
    size_t offset = 0;
    uint256_t decoded;
    bool res = decode(encoded, decoded, offset);
    EXPECT_FALSE(res);
    EXPECT_EQ(uint256_t(0), decoded);
    EXPECT_EQ(0, offset);
}

TEST(EthereumAbi, DecodeArrayUint) {
    Data encoded;
    append(encoded, parse_hex("000000000000000000000000000000000000000000000000000000000000000a"));
    append(encoded, parse_hex("3132333435363738393000000000000000000000000000000000000000000000"));
    size_t offset = 0;
    std::vector<byte> decoded;
    bool res = ParamByteArray::decodeBytes(encoded, decoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(10, decoded.size());
    if (decoded.size() >= 2) {
        EXPECT_EQ(49u, decoded[0]);
        EXPECT_EQ(50u, decoded[1]);
    }
    EXPECT_EQ(32 + 32, offset);
}

TEST(EthereumAbi, DecodeArrayTooShort) {
    Data encoded;
    append(encoded, parse_hex("000000000000000000000000000000000000000000000000000000000000000a"));
    append(encoded, parse_hex("313233343536373839"));
    size_t offset = 0;
    std::vector<byte> decoded;
    bool res = ParamByteArray::decodeBytes(encoded, decoded, offset);
    EXPECT_FALSE(res);
}

TEST(EthereumAbi, DecodeArrayInvalidLen) {
    Data encoded = parse_hex("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef");
    size_t offset = 0;
    std::vector<byte> decoded;
    bool res = ParamByteArray::decodeBytes(encoded, decoded, offset);
    EXPECT_FALSE(res);
}

TEST(EthereumAbi, DecodeByteArray) {
    Data encoded;
    append(encoded, parse_hex("000000000000000000000000000000000000000000000000000000000000000a"));
    append(encoded, parse_hex("3132333435363738393000000000000000000000000000000000000000000000"));
    size_t offset = 0;
    Data decoded;
    bool res = ParamByteArray::decodeBytes(encoded, decoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ("31323334353637383930", hex(decoded));
    EXPECT_EQ(32 + 32, offset);
}

TEST(EthereumAbi, DecodeByteArray10) {
    Data encoded = parse_hex("3132333435363738393000000000000000000000000000000000000000000000");
    size_t offset = 0;
    Data decoded;
    bool res = ParamByteArrayFix::decodeBytesFix(encoded, 10, decoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(10, decoded.size());
    EXPECT_EQ(49u, decoded[0]);
    EXPECT_EQ(50u, decoded[1]);
    EXPECT_EQ(32, offset);
}

TEST(EthereumAbi, DecodeArrayOfByteArray) {
    Data encoded = parse_hex(
        "0000000000000000000000000000000000000000000000000000000000000002"
        "0000000000000000000000000000000000000000000000000000000000000040"
        "0000000000000000000000000000000000000000000000000000000000000080"
        "0000000000000000000000000000000000000000000000000000000000000002"
        "1011000000000000000000000000000000000000000000000000000000000000"
        "0000000000000000000000000000000000000000000000000000000000000003"
        "1022200000000000000000000000000000000000000000000000000000000000"
    );
    size_t offset = 0;
    Data decoded;
    auto param = ParamArray();
    param.addParam(std::make_shared<ParamByteArray>(Data()));
    param.addParam(std::make_shared<ParamByteArray>(Data()));
    bool res = param.decode(encoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(2, param.getCount());
    EXPECT_EQ(7 * 32, offset);
    EXPECT_EQ(2, param.getVal().size());
}

///// Parameters encode & decode

TEST(EthereumAbi, EncodeParamsSimple) {
    auto p = Parameters(std::vector<std::shared_ptr<ParamBase>>{ 
        std::make_shared<ParamUInt256>(16u),
        std::make_shared<ParamUInt256>(17u),
        std::make_shared<ParamBool>(true) });
    EXPECT_EQ("(uint256,uint256,bool)", p.getType());
    Data encoded;
    p.encode(encoded);

    EXPECT_EQ(3 * 32, encoded.size());
    EXPECT_EQ(3 * 32, p.getSize());
    EXPECT_EQ(
        "0000000000000000000000000000000000000000000000000000000000000010"
        "0000000000000000000000000000000000000000000000000000000000000011"
        "0000000000000000000000000000000000000000000000000000000000000001",
        hex(encoded));
}

TEST(EthereumAbi, EncodeParamsMixed) {
    auto p = Parameters(std::vector<std::shared_ptr<ParamBase>>{
        std::make_shared<ParamUInt256>(69u), 
        std::make_shared<ParamArray>(std::vector<std::shared_ptr<ParamBase>>{
            std::make_shared<ParamUInt256>(1),
            std::make_shared<ParamUInt256>(2),
            std::make_shared<ParamUInt256>(3)
        }),
        std::make_shared<ParamBool>(true),
        std::make_shared<ParamString>("Hello"),
        std::make_shared<ParamByteArray>(Data{0x64, 0x61, 0x76, 0x65})
    });
    EXPECT_EQ("(uint256,uint256[],bool,string,bytes)", p.getType());
    Data encoded;
    p.encode(encoded);

    EXPECT_EQ(13 * 32, encoded.size());
    EXPECT_EQ(13 * 32, p.getSize());
    EXPECT_EQ(
        "0000000000000000000000000000000000000000000000000000000000000045"
        "00000000000000000000000000000000000000000000000000000000000000a0"
        "0000000000000000000000000000000000000000000000000000000000000001"
        "0000000000000000000000000000000000000000000000000000000000000120"
        "0000000000000000000000000000000000000000000000000000000000000160"
        "0000000000000000000000000000000000000000000000000000000000000003"
        "0000000000000000000000000000000000000000000000000000000000000001"
        "0000000000000000000000000000000000000000000000000000000000000002"
        "0000000000000000000000000000000000000000000000000000000000000003"
        "0000000000000000000000000000000000000000000000000000000000000005"
        "48656c6c6f000000000000000000000000000000000000000000000000000000"
        "0000000000000000000000000000000000000000000000000000000000000004"
        "6461766500000000000000000000000000000000000000000000000000000000",
        hex(encoded));
    /*
     * explained:
     * uint256 69u
     * idx of dynamic vector, 5*32
     * true
     * index of dynamic string, 9*32
     * index of dynamic data, 11*32
     * vector size 3
     * vector val1
     * vector val2
     * vector val3
     * string size 5
     * string
     * data size 4
     * data
     */
}

TEST(EthereumAbi, DecodeParamsSimple) {
    Data encoded;
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000010"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000011"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000001"));
    auto p = Parameters(std::vector<std::shared_ptr<ParamBase>>{
        std::make_shared<ParamUInt256>(0),
        std::make_shared<ParamUInt256>(0),
        std::make_shared<ParamBool>(false)
    });
    EXPECT_EQ("(uint256,uint256,bool)", p.getType());
    size_t offset = 0;
    bool res = p.decode(encoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(uint256_t(16u), (std::dynamic_pointer_cast<ParamUInt256>(p.getParam(0)))->getVal());
    EXPECT_EQ(uint256_t(17u), (std::dynamic_pointer_cast<ParamUInt256>(p.getParam(1)))->getVal());
    EXPECT_EQ(true, (std::dynamic_pointer_cast<ParamBool>(p.getParam(2)))->getVal());
    EXPECT_EQ(3 * 32, offset);
}

TEST(EthereumAbi, DecodeParamsMixed) {
    Data encoded;
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000045"));
    append(encoded, parse_hex("00000000000000000000000000000000000000000000000000000000000000a0"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000001"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000120"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000160"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000003"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000001"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000002"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000003"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000005"));
    append(encoded, parse_hex("48656c6c6f000000000000000000000000000000000000000000000000000000"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000004"));
    append(encoded, parse_hex("6461766500000000000000000000000000000000000000000000000000000000"));
    auto p = Parameters(std::vector<std::shared_ptr<ParamBase>>{
        std::make_shared<ParamUInt256>(), 
        std::make_shared<ParamArray>(std::vector<std::shared_ptr<ParamBase>>{
            std::make_shared<ParamUInt256>(),
            std::make_shared<ParamUInt256>(),
            std::make_shared<ParamUInt256>()
        }),
        std::make_shared<ParamBool>(),
        std::make_shared<ParamString>(),
        std::make_shared<ParamByteArray>()
    });
    EXPECT_EQ("(uint256,uint256[],bool,string,bytes)", p.getType());
    size_t offset = 0;
    bool res = p.decode(encoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(uint256_t(69u), (std::dynamic_pointer_cast<ParamUInt256>(p.getParam(0)))->getVal());
    EXPECT_EQ(3, (std::dynamic_pointer_cast<ParamArray>(p.getParam(1)))->getCount());
    EXPECT_EQ(1, (std::dynamic_pointer_cast<ParamUInt256>((std::dynamic_pointer_cast<ParamArray>(p.getParam(1)))->getParam(0)))->getVal());
    EXPECT_EQ(3, (std::dynamic_pointer_cast<ParamUInt256>((std::dynamic_pointer_cast<ParamArray>(p.getParam(1)))->getParam(2)))->getVal());
    EXPECT_EQ(true, (std::dynamic_pointer_cast<ParamBool>(p.getParam(2)))->getVal());
    EXPECT_EQ("Hello", (std::dynamic_pointer_cast<ParamString>(p.getParam(3)))->getVal());
    EXPECT_EQ(13 * 32, offset);
}

///// Function encode & decode

TEST(EthereumAbi, EncodeSignature) {
    auto func = Function("baz", std::vector<std::shared_ptr<ParamBase>>{
        std::make_shared<ParamUInt256>(69u),
        std::make_shared<ParamBool>(true)
    });
    EXPECT_EQ("baz(uint256,bool)", func.getType());
    Data encoded;
    func.encode(encoded);

    EXPECT_EQ(encoded.size(), 32 * 2 + 4);
    EXPECT_EQ(hex(encoded.begin(), encoded.begin() + 4), "72ed38b6");
    EXPECT_EQ(hex(encoded.begin() +   4, encoded.begin() + 36 ), "0000000000000000000000000000000000000000000000000000000000000045");
    EXPECT_EQ(hex(encoded.begin() +  36, encoded.begin() + 68 ), "0000000000000000000000000000000000000000000000000000000000000001");
}

TEST(EthereumAbi, EncodeFunctionWithDynamicArgumentsCase1) {
    auto func = Function("sam", std::vector<std::shared_ptr<ParamBase>>{
        std::make_shared<ParamByteArray>(Data{0x64, 0x61, 0x76, 0x65}),
        std::make_shared<ParamBool>(true),
        std::make_shared<ParamArray>(std::vector<std::shared_ptr<ParamBase>>{
            std::make_shared<ParamUInt256>(1),
            std::make_shared<ParamUInt256>(2),
            std::make_shared<ParamUInt256>(3)
        })
    });
    EXPECT_EQ("sam(bytes,bool,uint256[])", func.getType());
    Data encoded;
    func.encode(encoded);

    EXPECT_EQ(encoded.size(), 32 * 9 + 4);
    EXPECT_EQ(hex(encoded.begin() +   0, encoded.begin() + 4  ), "a5643bf2");
    EXPECT_EQ(hex(encoded.begin() +   4, encoded.begin() + 36 ), "0000000000000000000000000000000000000000000000000000000000000060");
    EXPECT_EQ(hex(encoded.begin() +  36, encoded.begin() + 68 ), "0000000000000000000000000000000000000000000000000000000000000001");
    EXPECT_EQ(hex(encoded.begin() +  68, encoded.begin() + 100), "00000000000000000000000000000000000000000000000000000000000000a0");
    EXPECT_EQ(hex(encoded.begin() + 100, encoded.begin() + 132), "0000000000000000000000000000000000000000000000000000000000000004");
    EXPECT_EQ(hex(encoded.begin() + 132, encoded.begin() + 164), "6461766500000000000000000000000000000000000000000000000000000000");
    EXPECT_EQ(hex(encoded.begin() + 164, encoded.begin() + 196), "0000000000000000000000000000000000000000000000000000000000000003");
    EXPECT_EQ(hex(encoded.begin() + 196, encoded.begin() + 228), "0000000000000000000000000000000000000000000000000000000000000001");
    EXPECT_EQ(hex(encoded.begin() + 228, encoded.begin() + 260), "0000000000000000000000000000000000000000000000000000000000000002");
    EXPECT_EQ(hex(encoded.begin() + 260, encoded.begin() + 292), "0000000000000000000000000000000000000000000000000000000000000003");
}

TEST(EthereumAbi, EncodeFunctionWithDynamicArgumentsCase2) {
    auto func = Function("f", std::vector<std::shared_ptr<ParamBase>>{
        std::make_shared<ParamUInt256>(0x123),
        std::make_shared<ParamArray>(std::vector<std::shared_ptr<ParamBase>>{
            std::make_shared<ParamUInt32>(0x456),
            std::make_shared<ParamUInt32>(0x789)
        }),
        std::make_shared<ParamByteArrayFix>(10, std::vector<byte>{0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30}),
        std::make_shared<ParamString>("Hello, world!")
    });
    EXPECT_EQ("f(uint256,uint32[],bytes10,string)", func.getType());
    Data encoded;
    func.encode(encoded);

    EXPECT_EQ(encoded.size(), 32 * 9 + 4);
    EXPECT_EQ(hex(encoded.begin() +   0, encoded.begin() + 4  ), "47b941bf");
    EXPECT_EQ(hex(encoded.begin() +   4, encoded.begin() + 36 ), "0000000000000000000000000000000000000000000000000000000000000123");
    EXPECT_EQ(hex(encoded.begin() +  36, encoded.begin() + 68 ), "0000000000000000000000000000000000000000000000000000000000000080");
    EXPECT_EQ(hex(encoded.begin() +  68, encoded.begin() + 100), "3132333435363738393000000000000000000000000000000000000000000000");
    EXPECT_EQ(hex(encoded.begin() + 100, encoded.begin() + 132), "00000000000000000000000000000000000000000000000000000000000000e0");
    EXPECT_EQ(hex(encoded.begin() + 132, encoded.begin() + 164), "0000000000000000000000000000000000000000000000000000000000000002");
    EXPECT_EQ(hex(encoded.begin() + 164, encoded.begin() + 196), "0000000000000000000000000000000000000000000000000000000000000456");
    EXPECT_EQ(hex(encoded.begin() + 196, encoded.begin() + 228), "0000000000000000000000000000000000000000000000000000000000000789");
    EXPECT_EQ(hex(encoded.begin() + 228, encoded.begin() + 260), "000000000000000000000000000000000000000000000000000000000000000d");
    EXPECT_EQ(hex(encoded.begin() + 260, encoded.begin() + 292), "48656c6c6f2c20776f726c642100000000000000000000000000000000000000");
}

TEST(EthereumAbi, DecodeFunctionOutputCase1) {
    Data encoded;
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000045"));

    auto func = Function("readout", std::vector<std::shared_ptr<ParamBase>>{
        std::make_shared<ParamAddress>(parse_hex("f784682c82526e245f50975190ef0fff4e4fc077")),
        std::make_shared<ParamUInt64>(1000)
    });
    func.addOutParam(std::make_shared<ParamUInt64>());
    EXPECT_EQ("readout(address,uint64)", func.getType());

    // original output value
    std::shared_ptr<ParamBase> param;
    EXPECT_TRUE(func.getOutParam(0, param));
    EXPECT_EQ(0, (std::dynamic_pointer_cast<ParamUInt64>(param))->getVal());

    size_t offset = 0;
    bool res = func.decodeOutput(encoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(32, offset);

    // new output value
    EXPECT_EQ(0x45, (std::dynamic_pointer_cast<ParamUInt64>(param))->getVal());
}

TEST(EthereumAbi, DecodeFunctionOutputCase2) {
    auto func = Function("getAmountsOut", std::vector<std::shared_ptr<ParamBase>>{
        std::make_shared<ParamUInt256>(100),
        std::make_shared<ParamArray>(std::make_shared<ParamAddress>(parse_hex("000000000000000000000000f784682c82526e245f50975190ef0fff4e4fc077")))
    });
    func.addOutParam(std::make_shared<ParamArray>(std::vector<std::shared_ptr<ParamBase>>{
        std::make_shared<ParamUInt256>(66),
        std::make_shared<ParamUInt256>(67)
    }));
    EXPECT_EQ("getAmountsOut(uint256,address[])", func.getType());

    Data encoded;
    append(encoded, parse_hex(
        "0000000000000000000000000000000000000000000000000000000000000020"
        "0000000000000000000000000000000000000000000000000000000000000002"
        "0000000000000000000000000000000000000000000000000000000000000004"
        "0000000000000000000000000000000000000000000000000000000000000005"
    ));
    size_t offset = 0;
    bool res = func.decodeOutput(encoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(128, offset);

    // new output values
    std::shared_ptr<ParamBase> param;
    EXPECT_TRUE(func.getOutParam(0, param));
    EXPECT_EQ(2, (std::dynamic_pointer_cast<ParamArray>(param))->getCount());
    EXPECT_EQ(4, (std::dynamic_pointer_cast<ParamUInt256>((std::dynamic_pointer_cast<ParamArray>(param))->getParam(0)))->getVal());
    EXPECT_EQ(5, (std::dynamic_pointer_cast<ParamUInt256>((std::dynamic_pointer_cast<ParamArray>(param))->getParam(1)))->getVal());
}

TEST(EthereumAbi, DecodeInputSignature) {
    Data encoded;
    append(encoded, parse_hex("72ed38b6"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000045"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000001"));
    auto func = Function("baz", std::vector<std::shared_ptr<ParamBase>>{
        std::make_shared<ParamUInt256>(), std::make_shared<ParamBool>()
    });
    EXPECT_EQ("baz(uint256,bool)", func.getType());
    size_t offset = 0;
    bool res = func.decodeInput(encoded, offset);
    EXPECT_TRUE(res);
    std::shared_ptr<ParamBase> param;
    EXPECT_TRUE(func.getInParam(0, param));
    EXPECT_EQ(69u, (std::dynamic_pointer_cast<ParamUInt256>(param))->getVal());
    EXPECT_TRUE(func.getInParam(1, param));
    EXPECT_EQ(true, (std::dynamic_pointer_cast<ParamBool>(param))->getVal());
    EXPECT_EQ(4 + 2 * 32, offset);  
}

TEST(EthereumAbi, DecodeFunctionInputWithDynamicArgumentsCase1) {
    Data encoded;
    append(encoded, parse_hex("a5643bf2"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000060"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000001"));
    append(encoded, parse_hex("00000000000000000000000000000000000000000000000000000000000000a0"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000004"));
    append(encoded, parse_hex("6461766500000000000000000000000000000000000000000000000000000000"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000003"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000001"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000002"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000003"));

    auto func = Function("sam", std::vector<std::shared_ptr<ParamBase>>{
        std::make_shared<ParamByteArray>(Data{0x64, 0x61, 0x76, 0x65}),
        std::make_shared<ParamBool>(true),
        std::make_shared<ParamArray>(std::vector<std::shared_ptr<ParamBase>>{
            std::make_shared<ParamUInt256>(1),
            std::make_shared<ParamUInt256>(2),
            std::make_shared<ParamUInt256>(3)
        })
    });
    EXPECT_EQ("sam(bytes,bool,uint256[])", func.getType());

    size_t offset = 0;
    bool res = func.decodeInput(encoded, offset);
    EXPECT_TRUE(res);
    std::shared_ptr<ParamBase> param;
    EXPECT_TRUE(func.getInParam(0, param));
    EXPECT_EQ(4, (std::dynamic_pointer_cast<ParamByteArray>(param))->getCount());
    EXPECT_EQ(0x64, (std::dynamic_pointer_cast<ParamByteArray>(param))->getVal()[0]);
    EXPECT_EQ(0x65, (std::dynamic_pointer_cast<ParamByteArray>(param))->getVal()[3]);
    EXPECT_TRUE(func.getInParam(1, param));
    EXPECT_EQ(true, (std::dynamic_pointer_cast<ParamBool>(param))->getVal());
    EXPECT_TRUE(func.getInParam(2, param));
    EXPECT_EQ(3, (std::dynamic_pointer_cast<ParamArray>(param))->getCount());
    EXPECT_EQ(uint256_t(1), (std::dynamic_pointer_cast<ParamUInt256>((std::dynamic_pointer_cast<ParamArray>(param))->getVal()[0]))->getVal());
    EXPECT_EQ(uint256_t(3), (std::dynamic_pointer_cast<ParamUInt256>((std::dynamic_pointer_cast<ParamArray>(param))->getVal()[2]))->getVal());
    EXPECT_EQ(4 + 9 * 32, offset);
}

TEST(EthereumAbi, DecodeFunctionInputWithDynamicArgumentsCase2) {
    Data encoded;
    append(encoded, parse_hex("47b941bf"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000123"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000080"));
    append(encoded, parse_hex("3132333435363738393000000000000000000000000000000000000000000000"));
    append(encoded, parse_hex("00000000000000000000000000000000000000000000000000000000000000e0"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000002"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000456"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000789"));
    append(encoded, parse_hex("000000000000000000000000000000000000000000000000000000000000000d"));
    append(encoded, parse_hex("48656c6c6f2c20776f726c642100000000000000000000000000000000000000"));

    auto func = Function("f", std::vector<std::shared_ptr<ParamBase>>{
        std::make_shared<ParamUInt256>(0x123),
        std::make_shared<ParamArray>(std::vector<std::shared_ptr<ParamBase>>{
            std::make_shared<ParamUInt32>(0x456),
            std::make_shared<ParamUInt32>(0x789)
        }),
        std::make_shared<ParamByteArrayFix>(10, std::vector<byte>{0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30}),
        std::make_shared<ParamString>("Hello, world!")
    });
    EXPECT_EQ("f(uint256,uint32[],bytes10,string)", func.getType());

    size_t offset = 0;
    bool res = func.decodeInput(encoded, offset);
    EXPECT_TRUE(res);
    std::shared_ptr<ParamBase> param;
    EXPECT_TRUE(func.getInParam(0, param));
    EXPECT_EQ(uint256_t(0x123), (std::dynamic_pointer_cast<ParamUInt256>(param))->getVal());
    EXPECT_TRUE(func.getInParam(1, param));
    EXPECT_EQ(2, (std::dynamic_pointer_cast<ParamArray>(param))->getCount());
    EXPECT_EQ(0x456, (std::dynamic_pointer_cast<ParamUInt32>((std::dynamic_pointer_cast<ParamArray>(param))->getVal()[0]))->getVal());
    EXPECT_EQ(0x789, (std::dynamic_pointer_cast<ParamUInt32>((std::dynamic_pointer_cast<ParamArray>(param))->getVal()[1]))->getVal());
    EXPECT_TRUE(func.getInParam(2, param));
    EXPECT_EQ(10, (std::dynamic_pointer_cast<ParamByteArrayFix>(param))->getCount());
    EXPECT_EQ("31323334353637383930", hex((std::dynamic_pointer_cast<ParamByteArrayFix>(param))->getVal()));
    EXPECT_TRUE(func.getInParam(3, param));
    EXPECT_EQ(std::string("Hello, world!"), (std::dynamic_pointer_cast<ParamString>(param))->getVal());
    EXPECT_EQ(4 + 9 * 32, offset);
}

TEST(EthereumAbi, DecodeFunctionContractMulticall) {
    Data encoded = parse_hex(
        "0xac9650d800000000000000000000000000000000000000000000000000000000000000200000000000000000"
        "000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000"
        "000000000000000000008000000000000000000000000000000000000000000000000000000000000001000000"
        "0000000000000000000000000000000000000000000000000000000001a0000000000000000000000000000000"
        "000000000000000000000000000000028000000000000000000000000000000000000000000000000000000000"
        "00000044d5fa2b00e71cd96d4ba1c4b512b0c5bee30d2b6becf61e574c32a17a67156fa9ed3c4c6f0000000000"
        "0000000000000047331175b23c2f067204b506ca1501c26731c990000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000064304e6a"
        "dee71cd96d4ba1c4b512b0c5bee30d2b6becf61e574c32a17a67156fa9ed3c4c6f000000000000000000000000"
        "000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000a48b95dd71e71cd96d4ba1c4b512b0c5bee30d2b6becf61e"
        "574c32a17a67156fa9ed3c4c6f000000000000000000000000000000000000000000000000000000000000003c"
        "000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000"
        "0000000000000000000000000000000000001447331175b23c2f067204b506ca1501c26731c990000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "0000000000000000000000000000000000000000a48b95dd71e71cd96d4ba1c4b512b0c5bee30d2b6becf61e57"
        "4c32a17a67156fa9ed3c4c6f00000000000000000000000000000000000000000000000000000000000002ca00"
        "000000000000000000000000000000000000000000000000000000000000600000000000000000000000000000"
        "000000000000000000000000000000000014d30f834b53d8f7e851e87b90ffa65757a35b850500000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000");
    ASSERT_EQ(4 + 928, encoded.size());

    auto func = Function("multicall", std::vector<std::shared_ptr<ParamBase>>{
        std::make_shared<ParamArray>(std::vector<std::shared_ptr<ParamBase>>{
            std::make_shared<ParamByteArray>(Data()),
            std::make_shared<ParamByteArray>(Data()),
            std::make_shared<ParamByteArray>(Data()),
            std::make_shared<ParamByteArray>(Data())
        }),
    });
    EXPECT_EQ("multicall(bytes[])", func.getType());

    size_t offset = 0;
    bool res = func.decodeInput(encoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(4 + 29 * 32, offset);
}

TEST(EthereumAbi, ParamFactoryMake) {
    {
        // test for UInt256: ParamUInt256 and ParamUIntN(256), both have type "uint256", factory produces the more specific ParamUInt256
        // there was confusion about this
        std::shared_ptr<ParamBase> p = ParamFactory::make("uint256");
        EXPECT_EQ("uint256", p->getType());
        EXPECT_TRUE(nullptr != std::dynamic_pointer_cast<ParamUInt256>(p).get());
        EXPECT_EQ(nullptr, std::dynamic_pointer_cast<ParamUIntN>(p).get());
    }
    {
        // int32 is ParamInt32, not ParamIntN
        std::shared_ptr<ParamBase> p = ParamFactory::make("int32");
        EXPECT_EQ("int32", p->getType());
        EXPECT_TRUE(nullptr != std::dynamic_pointer_cast<ParamInt32>(p).get());
        EXPECT_EQ(nullptr, std::dynamic_pointer_cast<ParamIntN>(p).get());
    }
    {
        // int168 is ParamIntN
        std::shared_ptr<ParamBase> p = ParamFactory::make("int168");
        EXPECT_EQ("int168", p->getType());
        EXPECT_TRUE(nullptr != std::dynamic_pointer_cast<ParamIntN>(p).get());
    }
    {
        // uint is uint256
        std::shared_ptr<ParamBase> p = ParamFactory::make("uint");
        EXPECT_EQ("uint256", p->getType());
        EXPECT_TRUE(nullptr != std::dynamic_pointer_cast<ParamUInt256>(p).get());
    }
    {
        // int is int256
        std::shared_ptr<ParamBase> p = ParamFactory::make("int");
        EXPECT_EQ("int256", p->getType());
        EXPECT_TRUE(nullptr != std::dynamic_pointer_cast<ParamInt256>(p).get());
    }
    {
        std::shared_ptr<ParamBase> p = ParamFactory::make("uint8[]");
        EXPECT_EQ("uint8[]", p->getType());
        EXPECT_TRUE(nullptr != std::dynamic_pointer_cast<ParamArray>(p).get());
        auto elemParam = std::dynamic_pointer_cast<ParamArray>(p)->getParam(0);
        EXPECT_TRUE(nullptr != elemParam.get());
    }
    {
        std::shared_ptr<ParamBase> p = ParamFactory::make("address[]");
        EXPECT_EQ("address[]", p->getType());
        EXPECT_TRUE(nullptr != std::dynamic_pointer_cast<ParamArray>(p).get());
        auto elemParam = std::dynamic_pointer_cast<ParamArray>(p)->getParam(0);
        EXPECT_TRUE(nullptr != elemParam.get());
    }
    {
        std::shared_ptr<ParamBase> p = ParamFactory::make("bytes[]");
        EXPECT_EQ("bytes[]", p->getType());
        EXPECT_TRUE(nullptr != std::dynamic_pointer_cast<ParamArray>(p).get());
        auto elemParam = std::dynamic_pointer_cast<ParamArray>(p)->getParam(0);
        EXPECT_TRUE(nullptr != elemParam.get());
    }
}

TEST(EthereumAbi, ParamFactoryMakeException) {
    EXPECT_EXCEPTION(ParamFactory::make("uint93"), "invalid bit size");
}

TEST(EthereumAbi, ParamFactoryGetArrayValue) {
    {
        auto pArray = std::make_shared<ParamArray>(std::make_shared<ParamUInt8>());
        const auto vals = ParamFactory::getArrayValue(pArray, pArray->getType());
        ASSERT_EQ(vals.size(), 1);
        EXPECT_EQ(vals[0], "0");
    }
    {   // wrong type, not array
        auto pArray = std::make_shared<ParamArray>(std::make_shared<ParamUInt8>());
        const auto vals = ParamFactory::getArrayValue(pArray, "bool");
        EXPECT_EQ(vals.size(), 0);
    }
    {   // wrong param, not array
        auto pArray = std::make_shared<ParamUInt8>();
        const auto vals = ParamFactory::getArrayValue(pArray, "uint8[]");
        EXPECT_EQ(vals.size(), 0);
    }
}

TEST(EthereumAbi, ParamFactorySetGetValue) {
    {
        auto p = std::make_shared<ParamUInt8>();
        EXPECT_EQ("0", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("13"));
        EXPECT_EQ("13", ParamFactory::getValue(p, p->getType()));
        EXPECT_FALSE(p->setValueJson("a5"));
        EXPECT_FALSE(p->setValueJson("0xa5"));
        EXPECT_FALSE(p->setValueJson("0x00"));
    }
    {
        auto p = std::make_shared<ParamUInt16>();
        EXPECT_EQ("0", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("1234"));
        EXPECT_EQ("1234", ParamFactory::getValue(p, p->getType()));
        EXPECT_FALSE(p->setValueJson("a5"));
        EXPECT_FALSE(p->setValueJson("0xa5"));
        EXPECT_FALSE(p->setValueJson("0x00"));
    }
    {
        auto p = std::make_shared<ParamUInt32>();
        EXPECT_EQ("0", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("1234567"));
        EXPECT_EQ("1234567", ParamFactory::getValue(p, p->getType()));
        EXPECT_FALSE(p->setValueJson("a5"));
        EXPECT_FALSE(p->setValueJson("0xa5"));
        EXPECT_FALSE(p->setValueJson("0x00"));
    }
    {
        auto p = std::make_shared<ParamUInt64>();
        EXPECT_EQ("0", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("1234567"));
        EXPECT_EQ("1234567", ParamFactory::getValue(p, p->getType()));
        EXPECT_FALSE(p->setValueJson("a5"));
        EXPECT_FALSE(p->setValueJson("0xa5"));
        EXPECT_FALSE(p->setValueJson("0x00"));
    }
    {
        auto p = std::make_shared<ParamUIntN>(128);
        EXPECT_EQ("0", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("1234567890123456789"));
        EXPECT_EQ("1234567890123456789", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("0xa5"));
        EXPECT_EQ("165", ParamFactory::getValue(p, p->getType()));
        EXPECT_FALSE(p->setValueJson("a5"));
    }
    {
        auto p = std::make_shared<ParamUIntN>(168);
        EXPECT_EQ("0", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("1234567890123456789"));
        EXPECT_EQ("1234567890123456789", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("0xa5"));
        EXPECT_EQ("165", ParamFactory::getValue(p, p->getType()));
        EXPECT_FALSE(p->setValueJson("a5"));
    }
    {
        auto p = std::make_shared<ParamUInt256>();
        EXPECT_EQ("0", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("1234567890123456789"));
        EXPECT_EQ("1234567890123456789", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("0xa5"));
        EXPECT_EQ("165", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("0x00"));
        EXPECT_FALSE(p->setValueJson("a5"));
        EXPECT_FALSE(p->setValueJson("0x"));
    }
    {
        auto p = std::make_shared<ParamInt8>();
        EXPECT_EQ("0", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("13"));
        EXPECT_EQ("13", ParamFactory::getValue(p, p->getType()));
        EXPECT_FALSE(p->setValueJson("a5"));
        EXPECT_FALSE(p->setValueJson("0xa5"));
        EXPECT_FALSE(p->setValueJson("0x00"));
    }
    {
        auto p = std::make_shared<ParamInt16>();
        EXPECT_EQ("0", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("1234"));
        EXPECT_EQ("1234", ParamFactory::getValue(p, p->getType()));
        EXPECT_FALSE(p->setValueJson("a5"));
        EXPECT_FALSE(p->setValueJson("0xa5"));
        EXPECT_FALSE(p->setValueJson("0x00"));
    }
    {
        auto p = std::make_shared<ParamInt32>();
        EXPECT_EQ("0", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("1234567"));
        EXPECT_EQ("1234567", ParamFactory::getValue(p, p->getType()));
        EXPECT_FALSE(p->setValueJson("a5"));
        EXPECT_FALSE(p->setValueJson("0xa5"));
        EXPECT_FALSE(p->setValueJson("0x00"));
    }
    {
        auto p = std::make_shared<ParamInt64>();
        EXPECT_EQ("0", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("1234567"));
        EXPECT_EQ("1234567", ParamFactory::getValue(p, p->getType()));
        EXPECT_FALSE(p->setValueJson("a5"));
        EXPECT_FALSE(p->setValueJson("0xa5"));
        EXPECT_FALSE(p->setValueJson("0x00"));
    }
    {
        auto p = std::make_shared<ParamIntN>(128);
        EXPECT_EQ("0", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("1234567890123456789"));
        EXPECT_EQ("1234567890123456789", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("0xa5"));
        EXPECT_EQ("165", ParamFactory::getValue(p, p->getType()));
        EXPECT_FALSE(p->setValueJson("a5"));
    }
    {
        auto p = std::make_shared<ParamIntN>(168);
        EXPECT_EQ("0", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("1234567890123456789"));
        EXPECT_EQ("1234567890123456789", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("0xa5"));
        EXPECT_EQ("165", ParamFactory::getValue(p, p->getType()));
        EXPECT_FALSE(p->setValueJson("a5"));
    }
    {
        auto p = std::make_shared<ParamInt256>();
        EXPECT_EQ("0", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("1234567890123456789"));
        EXPECT_EQ("1234567890123456789", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("0xa5"));
        EXPECT_EQ("165", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("0x00"));
        EXPECT_FALSE(p->setValueJson("a5"));
        EXPECT_FALSE(p->setValueJson("0x"));
    }
    {
        auto p = std::make_shared<ParamBool>();
        EXPECT_EQ("false", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("true"));
        EXPECT_EQ("true", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("false"));
        EXPECT_TRUE(p->setValueJson("1"));
        EXPECT_TRUE(p->setValueJson("0"));
        EXPECT_FALSE(p->setValueJson("a5"));
        EXPECT_FALSE(p->setValueJson("0xa5"));
        EXPECT_FALSE(p->setValueJson("0x00"));
    }
    {
        auto p = std::make_shared<ParamString>();
        EXPECT_EQ("", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("ABCdefGHI"));
        EXPECT_EQ("ABCdefGHI", ParamFactory::getValue(p, p->getType()));
        EXPECT_EQ(9, p->getCount());
    }
    {
        auto p = std::make_shared<ParamByteArray>();
        EXPECT_EQ("0x", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("0123456789"));
        EXPECT_EQ("0x0123456789", ParamFactory::getValue(p, p->getType()));
    }
    {
        auto p = std::make_shared<ParamByteArrayFix>(36);
        EXPECT_EQ("0x000000000000000000000000000000000000000000000000000000000000000000000000", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("0x000000000000000000000000000000000000000000000000000000000000000123456789"));
        EXPECT_EQ("0x000000000000000000000000000000000000000000000000000000000000000123456789", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("0x0123456789")); // will be padded
        EXPECT_EQ("0x012345678900000000000000000000000000000000000000000000000000000000000000", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("0xabcdef0000000000000000000000000000000000000000000000000000000000000123456789")); // will be cropped
        EXPECT_EQ("0xabcdef000000000000000000000000000000000000000000000000000000000000012345", ParamFactory::getValue(p, p->getType()));
    }
    {
        auto p = std::make_shared<ParamAddress>();
        EXPECT_EQ("0x0000000000000000000000000000000000000000", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("0x0000000000000000000000000000000123456789"));
        EXPECT_EQ("0x0000000000000000000000000000000123456789", ParamFactory::getValue(p, p->getType()));
    }
    {
        auto p = std::make_shared<ParamArray>(std::make_shared<ParamUInt8>());
        EXPECT_EQ("[0]", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("[13,14,15]"));
        EXPECT_EQ("[13,14,15]", ParamFactory::getValue(p, p->getType()));
        EXPECT_FALSE(p->setValueJson("13"));
    }
    {
        auto p = std::make_shared<ParamArray>(std::make_shared<ParamAddress>());
        EXPECT_EQ("[\"0x0000000000000000000000000000000000000000\"]", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("[\"0x0000000000000000000000000000000123456789\"]"));
        EXPECT_EQ("[\"0x0000000000000000000000000000000123456789\"]", ParamFactory::getValue(p, p->getType()));
        EXPECT_FALSE(p->setValueJson("0x0000000000000000000000000000000123456789"));
    }
    {
        auto p = std::make_shared<ParamArray>(std::make_shared<ParamBool>());
        EXPECT_EQ("[false]", ParamFactory::getValue(p, p->getType()));
        EXPECT_TRUE(p->setValueJson("[true,false,true]"));
        EXPECT_EQ("[true,false,true]", ParamFactory::getValue(p, p->getType()));
        EXPECT_FALSE(p->setValueJson("true"));
    }
}

TEST(EthereumAbi, ParamFactoryGetValue) {
    const std::vector<std::string> types = {
        "uint8", "uint16", "uint32", "uint64", "uint128", "uint168", "uint256",
        "int8", "int16", "int32", "int64", "int128", "int168", "int256",
        "bool", "string", "bytes", "bytes168", "address",
        "uint8[]", "address[]", "bool[]", "bytes[]",
    };
    for (auto t: types) {
        std::shared_ptr<ParamBase> p = ParamFactory::make(t);
        EXPECT_EQ(t, p->getType());

        std::string expected = "";
        // for numerical values, value is "0"
        if (t == "uint8[]" || t == "int8[]") {
            expected = "[0]";
        } else if (t == "bool") {
            expected = "false";
        } else if (t == "address[]") {
            expected = "[\"0x0000000000000000000000000000000000000000\"]";
        } else if (t == "address") {
            expected = "0x0000000000000000000000000000000000000000";
        } else if (t == "bool[]") {
            expected = "[false]";
        } else if (t == "bytes[]") {
            expected = "[\"0x\"]";
        } else if (t == "bytes") {
            expected = "0x";
        } else if (t == "bytes168") {
            expected = "0x000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
        } else if (t.substr(0, 3) == "int" || t.substr(0, 4) == "uint") {
            expected = "0";
        }
        EXPECT_EQ(expected, ParamFactory::getValue(p, t));
    }
}

TEST(EthereumAbi, MaskForBits) {
    EXPECT_EQ(0x000000ffffff, ParamUIntN::maskForBits(24));
    EXPECT_EQ(0x00ffffffffff, ParamUIntN::maskForBits(40));
}

TEST(EthereumAbi, ParamSetMethods) {
    {
        auto p = ParamSet(std::vector<std::shared_ptr<ParamBase>>{
            std::make_shared<ParamUInt256>(16u),
            std::make_shared<ParamBool>(true) });
        EXPECT_EQ(p.getCount(), 2);
        EXPECT_EQ(p.addParam(std::shared_ptr<ParamString>(nullptr)), -1);

        std::shared_ptr<ParamBase> getparam;
        EXPECT_TRUE(p.getParam(1, getparam));
        EXPECT_EQ(getparam->getType(), "bool");
        EXPECT_FALSE(p.getParam(2, getparam));

        EXPECT_EQ(p.getParamUnsafe(0)->getType(), "uint256");
        EXPECT_EQ(p.getParamUnsafe(1)->getType(), "bool");
        EXPECT_EQ(p.getParamUnsafe(2)->getType(), "uint256");
        EXPECT_EQ(p.getParamUnsafe(99)->getType(), "uint256");
    }
    {
        auto pEmpty = ParamSet(std::vector<std::shared_ptr<ParamBase>>{});
        EXPECT_EQ(pEmpty.getParamUnsafe(0).get(), nullptr);
    }
}

TEST(EthereumAbi, ParametersMethods) {
    auto p = Parameters(std::vector<std::shared_ptr<ParamBase>>{
        std::make_shared<ParamUInt256>(16u),
        std::make_shared<ParamBool>(true) });
    EXPECT_TRUE(p.isDynamic());
    EXPECT_EQ(p.getCount(), 2);
    EXPECT_FALSE(p.setValueJson("value"));
    EXPECT_EQ(hex(p.hashStruct()), "755311b9e2cee471a91b161ccc5deed933d844b5af2b885543cc3c04eb640983");
}
