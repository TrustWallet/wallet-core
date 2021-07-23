// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Ethereum/ABI.h"
#include "Ethereum/Address.h"
#include "Ethereum/Signer.h"
#include <HexCoding.h>
#include <PrivateKey.h>
#include "../interface/TWTestUtilities.h"

#include <fstream>
#include <gtest/gtest.h>

using namespace TW::Ethereum::ABI;
using namespace TW::Ethereum;
using namespace TW;

extern std::string TESTS_ROOT;

std::string load_file(const std::string path) {
    std::ifstream stream(path);
    std::string content((std::istreambuf_iterator<char>(stream)), (std::istreambuf_iterator<char>()));
    return content;
}

// https://github.com/MetaMask/eth-sig-util/blob/main/test/index.ts

ParamStruct msgPersonCow2("Person", std::vector<std::shared_ptr<ParamNamed>>{
    std::make_shared<ParamNamed>("name", std::make_shared<ParamString>("Cow")),
    std::make_shared<ParamNamed>("wallets", std::make_shared<ParamArray>(std::vector<std::shared_ptr<ParamBase>>{
        std::make_shared<ParamAddress>(parse_hex("CD2a3d9F938E13CD947Ec05AbC7FE734Df8DD826")),
        std::make_shared<ParamAddress>(parse_hex("DeaDbeefdEAdbeefdEadbEEFdeadbeEFdEaDbeeF"))
    }))
});
ParamStruct msgPersonBob3("Person", std::vector<std::shared_ptr<ParamNamed>>{
    std::make_shared<ParamNamed>("name", std::make_shared<ParamString>("Bob")),
    std::make_shared<ParamNamed>("wallets", std::make_shared<ParamArray>(std::vector<std::shared_ptr<ParamBase>>{
        std::make_shared<ParamAddress>(parse_hex("bBbBBBBbbBBBbbbBbbBbbbbBBbBbbbbBbBbbBBbB")),
        std::make_shared<ParamAddress>(parse_hex("B0BdaBea57B0BDABeA57b0bdABEA57b0BDabEa57")),
        std::make_shared<ParamAddress>(parse_hex("B0B0b0b0b0b0B000000000000000000000000000"))
    }))
});
ParamStruct msgGroup("Group", std::vector<std::shared_ptr<ParamNamed>>{
    std::make_shared<ParamNamed>("name", std::make_shared<ParamString>("")),
    std::make_shared<ParamNamed>("members", std::make_shared<ParamArray>(std::vector<std::shared_ptr<ParamBase>>{
        std::make_shared<ParamStruct>(msgPersonCow2)
    }))
});
ParamStruct msgMailCow1Bob1("Mail", std::vector<std::shared_ptr<ParamNamed>>{
    std::make_shared<ParamNamed>("from", std::make_shared<ParamStruct>("Person", std::vector<std::shared_ptr<ParamNamed>>{
        std::make_shared<ParamNamed>("name", std::make_shared<ParamString>("Cow")),
        std::make_shared<ParamNamed>("wallet", std::make_shared<ParamAddress>(parse_hex("CD2a3d9F938E13CD947Ec05AbC7FE734Df8DD826")))
    })),
    std::make_shared<ParamNamed>("to", std::make_shared<ParamStruct>("Person", std::vector<std::shared_ptr<ParamNamed>>{
        std::make_shared<ParamNamed>("name", std::make_shared<ParamString>("Bob")),
        std::make_shared<ParamNamed>("wallet", std::make_shared<ParamAddress>(parse_hex("bBbBBBBbbBBBbbbBbbBbbbbBBbBbbbbBbBbbBBbB")))
    })),
    std::make_shared<ParamNamed>("contents", std::make_shared<ParamString>("Hello, Bob!"))
});
ParamStruct msgMailCow2Bob3("Mail", std::vector<std::shared_ptr<ParamNamed>>{
    std::make_shared<ParamNamed>("from", std::make_shared<ParamStruct>(msgPersonCow2)),
    std::make_shared<ParamNamed>("to", std::make_shared<ParamArray>(std::make_shared<ParamStruct>(msgPersonBob3))),
    std::make_shared<ParamNamed>("contents", std::make_shared<ParamString>("Hello, Bob!"))
});
ParamStruct msgEIP712Domain("EIP712Domain", std::vector<std::shared_ptr<ParamNamed>>{
    std::make_shared<ParamNamed>("name", std::make_shared<ParamString>("Ether Mail")),
    std::make_shared<ParamNamed>("version", std::make_shared<ParamString>("1")),
    std::make_shared<ParamNamed>("chainId", std::make_shared<ParamUInt256>(1)),
    std::make_shared<ParamNamed>("verifyingContract", std::make_shared<ParamAddress>(parse_hex("CcCCccccCCCCcCCCCCCcCcCccCcCCCcCcccccccC")))
});

PrivateKey privateKeyCow = PrivateKey(Hash::keccak256(TW::data("cow")));
PrivateKey privateKeyDragon = PrivateKey(Hash::keccak256(TW::data("dragon")));
PrivateKey privateKeyOilTimes12 = PrivateKey(parse_hex("b0f20d59451a2fac1be6d458e036adfa5d83ebd4c21f9a76de3c4a3a65671eba")); // 0x60c2A43Cc69658eC4b02a65A07623D7192166F4e

// See 'signedTypeData' in https://github.com/MetaMask/eth-sig-util/blob/main/test/index.ts
TEST(EthereumAbiStruct, encodeTypes) {
    EXPECT_EQ(msgMailCow1Bob1.encodeType(), "Mail(Person from,Person to,string contents)Person(string name,address wallet)");

    EXPECT_EQ(hex(msgMailCow1Bob1.hashType()), "a0cedeb2dc280ba39b857546d74f5549c3a1d7bdc2dd96bf881f76108e23dac2");

    EXPECT_EQ(hex(msgMailCow1Bob1.encodeHashes()), "a0cedeb2dc280ba39b857546d74f5549c3a1d7bdc2dd96bf881f76108e23dac2fc71e5fa27ff56c350aa531bc129ebdf613b772b6604664f5d8dbe21b85eb0c8cd54f074a4af31b4411ff6a60c9719dbd559c221c8ac3492d9d872b041d703d1b5aadf3154a261abdd9086fc627b61efca26ae5702701d05cd2305f7c52a2fc8");

    EXPECT_EQ(hex(msgMailCow1Bob1.hashStruct()), "c52c0ee5d84264471806290a3f2c4cecfc5490626bf912d01f240d7a274b371e");

    EXPECT_EQ(hex(msgEIP712Domain.hashStruct()), "f2cee375fa42b42143804025fc449deafd50cc031ca257e0b194a650a912090f");

    Address address = Address(privateKeyCow.getPublicKey(TWPublicKeyTypeSECP256k1Extended));
    EXPECT_EQ(address.string(), "0xCD2a3d9F938E13CD947Ec05AbC7FE734Df8DD826");
}

TEST(EthereumAbiStruct, encodeTypes_Json) {
    auto hash = ParamStruct::hashStructJson(
        R"({
            "types": {
                "EIP712Domain": [
                    {"name": "name", "type": "string"},
                    {"name": "version", "type": "string"},
                    {"name": "chainId", "type": "uint256"},
                    {"name": "verifyingContract", "type": "address"}
                ],
                "Person": [
                    {"name": "name", "type": "string"},
                    {"name": "wallet", "type": "address"}
                ],
                "Mail": [
                    {"name": "from", "type": "Person"},
                    {"name": "to", "type": "Person"},
                    {"name": "contents", "type": "string"}
                ]
            },
            "primaryType": "Mail",
            "domain": {
                "name": "Ether Mail",
                "version": "1",
                "chainId": "0x01",
                "verifyingContract": "0xCcCCccccCCCCcCCCCCCcCcCccCcCCCcCcccccccC"
            },
            "message": {
                "from": {
                    "name": "Cow",
                    "wallet": "CD2a3d9F938E13CD947Ec05AbC7FE734Df8DD826"
                },
                "to": {
                    "name": "Bob",
                    "wallet": "bBbBBBBbbBBBbbbBbbBbbbbBBbBbbbbBbBbbBBbB"
                },
                "contents": "Hello, Bob!"
            }
        })");
    ASSERT_EQ(hex(hash), "be609aee343fb3c4b28e1df9e632fca64fcfaede20f02e86244efddf30957bd2");

    // sign the hash
    const auto rsv = Signer::sign(privateKeyCow, 0, hash);
    EXPECT_EQ(hex(store(rsv.r)), "4355c47d63924e8a72e509b65029052eb6c299d53a04e167c5775fd466751c9d");
    EXPECT_EQ(hex(store(rsv.s)), "07299936d304c153f6443dfa05f40ff007d72911b6f72307f996231605b91562");
    EXPECT_EQ(hex(store(rsv.v)), "1c");
}

// See 'signedTypeData with V3 string' in https://github.com/MetaMask/eth-sig-util/blob/main/test/index.ts
TEST(EthereumAbiStruct, encodeTypes_v3) {
    EXPECT_EQ(msgMailCow1Bob1.encodeType(), "Mail(Person from,Person to,string contents)Person(string name,address wallet)");

    EXPECT_EQ(hex(msgMailCow1Bob1.hashType()), "a0cedeb2dc280ba39b857546d74f5549c3a1d7bdc2dd96bf881f76108e23dac2");

    EXPECT_EQ(hex(msgMailCow1Bob1.encodeHashes()), "a0cedeb2dc280ba39b857546d74f5549c3a1d7bdc2dd96bf881f76108e23dac2fc71e5fa27ff56c350aa531bc129ebdf613b772b6604664f5d8dbe21b85eb0c8cd54f074a4af31b4411ff6a60c9719dbd559c221c8ac3492d9d872b041d703d1b5aadf3154a261abdd9086fc627b61efca26ae5702701d05cd2305f7c52a2fc8");

    EXPECT_EQ(hex(msgMailCow1Bob1.hashStruct()), "c52c0ee5d84264471806290a3f2c4cecfc5490626bf912d01f240d7a274b371e");

    EXPECT_EQ(hex(msgEIP712Domain.hashStruct()), "f2cee375fa42b42143804025fc449deafd50cc031ca257e0b194a650a912090f");

    Address address = Address(privateKeyCow.getPublicKey(TWPublicKeyTypeSECP256k1Extended));
    EXPECT_EQ(address.string(), "0xCD2a3d9F938E13CD947Ec05AbC7FE734Df8DD826");
}

TEST(EthereumAbiStruct, encodeTypes_v3_Json) {
    auto hash = ParamStruct::hashStructJson(
        R"({
            "types": {
                "EIP712Domain": [
                    {"name": "name", "type": "string"},
                    {"name": "version", "type": "string"},
                    {"name": "chainId", "type": "uint256"},
                    {"name": "verifyingContract", "type": "address"}
                ],
                "Person": [
                    {"name": "name", "type": "string"},
                    {"name": "wallet", "type": "address"}
                ],
                "Mail": [
                    {"name": "from", "type": "Person"},
                    {"name": "to", "type": "Person"},
                    {"name": "contents", "type": "string"}
                ]
            },
            "primaryType": "Mail",
            "domain": {
                "name": "Ether Mail",
                "version": "1",
                "chainId": 1,
                "verifyingContract": "0xCcCCccccCCCCcCCCCCCcCcCccCcCCCcCcccccccC"
            },
            "message": {
                "from": {
                    "name": "Cow",
                    "wallet": "CD2a3d9F938E13CD947Ec05AbC7FE734Df8DD826"
                },
                "to": {
                    "name": "Bob",
                    "wallet": "bBbBBBBbbBBBbbbBbbBbbbbBBbBbbbbBbBbbBBbB"
                },
                "contents": "Hello, Bob!"
            }
        })");
    ASSERT_EQ(hex(hash), "be609aee343fb3c4b28e1df9e632fca64fcfaede20f02e86244efddf30957bd2");

    // sign the hash
    const auto rsv = Signer::sign(privateKeyCow, 0, hash);
    EXPECT_EQ(hex(store(rsv.r)), "4355c47d63924e8a72e509b65029052eb6c299d53a04e167c5775fd466751c9d");
    EXPECT_EQ(hex(store(rsv.s)), "07299936d304c153f6443dfa05f40ff007d72911b6f72307f996231605b91562");
    EXPECT_EQ(hex(store(rsv.v)), "1c");
}

// See 'signedTypeData_v4' in https://github.com/MetaMask/eth-sig-util/blob/main/test/index.ts
TEST(EthereumAbiStruct, encodeTypes_v4) {
    EXPECT_EQ(msgGroup.encodeType(), "Group(string name,Person[] members)Person(string name,address[] wallets)");

    EXPECT_EQ(msgPersonCow2.encodeType(), "Person(string name,address[] wallets)");

    EXPECT_EQ(hex(msgPersonCow2.hashType()), "fabfe1ed996349fc6027709802be19d047da1aa5d6894ff5f6486d92db2e6860");

    EXPECT_EQ(hex(msgPersonCow2.encodeHashes()), 
        "fabfe1ed996349fc6027709802be19d047da1aa5d6894ff5f6486d92db2e6860"
        "8c1d2bd5348394761719da11ec67eedae9502d137e8940fee8ecd6f641ee1648"
        "8a8bfe642b9fc19c25ada5dadfd37487461dc81dd4b0778f262c163ed81b5e2a");

    EXPECT_EQ(hex(msgPersonCow2.hashStruct()), "9b4846dd48b866f0ac54d61b9b21a9e746f921cefa4ee94c4c0a1c49c774f67f");

    EXPECT_EQ(hex(msgPersonBob3.encodeHashes()), 
        "fabfe1ed996349fc6027709802be19d047da1aa5d6894ff5f6486d92db2e6860"
        "28cac318a86c8a0a6a9156c2dba2c8c2363677ba0514ef616592d81557e679b6"
        "d2734f4c86cc3bd9cabf04c3097589d3165d95e4648fc72d943ed161f651ec6d");

    EXPECT_EQ(hex(msgPersonBob3.hashStruct()), "efa62530c7ae3a290f8a13a5fc20450bdb3a6af19d9d9d2542b5a94e631a9168");

    EXPECT_EQ(msgMailCow2Bob3.encodeType(), "Mail(Person from,Person[] to,string contents)Person(string name,address[] wallets)");

    EXPECT_EQ(hex(msgMailCow2Bob3.hashType()), "4bd8a9a2b93427bb184aca81e24beb30ffa3c747e2a33d4225ec08bf12e2e753");

    EXPECT_EQ(hex(msgMailCow2Bob3.encodeHashes()), 
        "4bd8a9a2b93427bb184aca81e24beb30ffa3c747e2a33d4225ec08bf12e2e753"
        "9b4846dd48b866f0ac54d61b9b21a9e746f921cefa4ee94c4c0a1c49c774f67f"
        "ca322beec85be24e374d18d582a6f2997f75c54e7993ab5bc07404ce176ca7cd"
        "b5aadf3154a261abdd9086fc627b61efca26ae5702701d05cd2305f7c52a2fc8");

    EXPECT_EQ(hex(msgMailCow2Bob3.hashStruct()), "eb4221181ff3f1a83ea7313993ca9218496e424604ba9492bb4052c03d5c3df8");

    EXPECT_EQ(hex(msgEIP712Domain.hashStruct()), "f2cee375fa42b42143804025fc449deafd50cc031ca257e0b194a650a912090f");

    Address address = Address(privateKeyCow.getPublicKey(TWPublicKeyTypeSECP256k1Extended));
    EXPECT_EQ(address.string(), "0xCD2a3d9F938E13CD947Ec05AbC7FE734Df8DD826");
}

TEST(EthereumAbiStruct, encodeTypes_v4_Json) {
    auto hash = ParamStruct::hashStructJson(
        R"({
            "types": {
                "EIP712Domain": [
                    {"name": "name", "type": "string"},
                    {"name": "version", "type": "string"},
                    {"name": "chainId", "type": "uint256"},
                    {"name": "verifyingContract", "type": "address"}
                ],
                "Person": [
                    {"name": "name", "type": "string"},
                    {"name": "wallets", "type": "address[]"}
                ],
                "Mail": [
                    {"name": "from", "type": "Person"},
                    {"name": "to", "type": "Person[]"},
                    {"name": "contents", "type": "string"}
                ]
            },
            "primaryType": "Mail",
            "domain": {
                "name": "Ether Mail",
                "version": "1",
                "chainId": 1,
                "verifyingContract": "0xCcCCccccCCCCcCCCCCCcCcCccCcCCCcCcccccccC"
            },
            "message": {
                "from": {
                    "name": "Cow",
                    "wallets": [
                        "CD2a3d9F938E13CD947Ec05AbC7FE734Df8DD826",
                        "DeaDbeefdEAdbeefdEadbEEFdeadbeEFdEaDbeeF"
                    ]
                },
                "to": [
                    {
                        "name": "Bob",
                        "wallets": [
                            "bBbBBBBbbBBBbbbBbbBbbbbBBbBbbbbBbBbbBBbB",
                            "B0BdaBea57B0BDABeA57b0bdABEA57b0BDabEa57",
                            "B0B0b0b0b0b0B000000000000000000000000000"
                        ]
                    }
                ],
                "contents": "Hello, Bob!"
            }
        })");
    ASSERT_EQ(hex(hash), "a85c2e2b118698e88db68a8105b794a8cc7cec074e89ef991cb4f5f533819cc2");

    // sign the hash
    const auto rsv = Signer::sign(privateKeyCow, 0, hash);
    EXPECT_EQ(hex(store(rsv.r)), "65cbd956f2fae28a601bebc9b906cea0191744bd4c4247bcd27cd08f8eb6b71c");
    EXPECT_EQ(hex(store(rsv.s)), "78efdf7a31dc9abee78f492292721f362d296cf86b4538e07b51303b67f74906");
    EXPECT_EQ(hex(store(rsv.v)), "1b");
}

// See 'signedTypeData_v4 with recursive types' in https://github.com/MetaMask/eth-sig-util/blob/main/test/index.ts
TEST(EthereumAbiStruct, encodeTypes_v4Rec) {
    ParamStruct msgPersonRecursiveMother("Person", std::vector<std::shared_ptr<ParamNamed>>{
        std::make_shared<ParamNamed>("name", std::make_shared<ParamString>("Lyanna")),
        std::make_shared<ParamNamed>("mother", std::make_shared<ParamStruct>("Person", std::vector<std::shared_ptr<ParamNamed>>{})),
        std::make_shared<ParamNamed>("father", std::make_shared<ParamStruct>("Person", std::vector<std::shared_ptr<ParamNamed>>{
            std::make_shared<ParamNamed>("name", std::make_shared<ParamString>("Rickard")),
            std::make_shared<ParamNamed>("mother", std::make_shared<ParamStruct>("Person", std::vector<std::shared_ptr<ParamNamed>>{})),
            std::make_shared<ParamNamed>("father", std::make_shared<ParamStruct>("Person", std::vector<std::shared_ptr<ParamNamed>>{}))
        }))
    });
    ParamStruct msgPersonRecursiveFather("Person", std::vector<std::shared_ptr<ParamNamed>>{
        std::make_shared<ParamNamed>("name", std::make_shared<ParamString>("Rhaegar")),
        std::make_shared<ParamNamed>("mother", std::make_shared<ParamStruct>("Person", std::vector<std::shared_ptr<ParamNamed>>{})),
        std::make_shared<ParamNamed>("father", std::make_shared<ParamStruct>("Person", std::vector<std::shared_ptr<ParamNamed>>{
            std::make_shared<ParamNamed>("name", std::make_shared<ParamString>("Aeris II")),
            std::make_shared<ParamNamed>("mother", std::make_shared<ParamStruct>("Person", std::vector<std::shared_ptr<ParamNamed>>{})),
            std::make_shared<ParamNamed>("father", std::make_shared<ParamStruct>("Person", std::vector<std::shared_ptr<ParamNamed>>{}))
        }))
    });
    ParamStruct msgPersonRecursive("Person", std::vector<std::shared_ptr<ParamNamed>>{
        std::make_shared<ParamNamed>("name", std::make_shared<ParamString>("Jon")),
        std::make_shared<ParamNamed>("mother", std::make_shared<ParamStruct>(msgPersonRecursiveMother)),
        std::make_shared<ParamNamed>("father", std::make_shared<ParamStruct>(msgPersonRecursiveFather))
    });

    EXPECT_EQ(msgPersonRecursive.encodeType(), "Person(string name,Person mother,Person father)");

    EXPECT_EQ(hex(msgPersonRecursive.hashType()), "7c5c8e90cb92c8da53b893b24962513be98afcf1b57b00327ae4cc14e3a64116");

    EXPECT_EQ(hex(msgPersonRecursiveMother.encodeHashes()), 
        "7c5c8e90cb92c8da53b893b24962513be98afcf1b57b00327ae4cc14e3a64116"
        "afe4142a2b3e7b0503b44951e6030e0e2c5000ef83c61857e2e6003e7aef8570"
        "0000000000000000000000000000000000000000000000000000000000000000"
        "88f14be0dd46a8ec608ccbff6d3923a8b4e95cdfc9648f0db6d92a99a264cb36");

    EXPECT_EQ(hex(msgPersonRecursiveMother.hashStruct()), "9ebcfbf94f349de50bcb1e3aa4f1eb38824457c99914fefda27dcf9f99f6178b");

    EXPECT_EQ(hex(msgPersonRecursiveFather.encodeHashes()), 
        "7c5c8e90cb92c8da53b893b24962513be98afcf1b57b00327ae4cc14e3a64116"
        "b2a7c7faba769181e578a391a6a6811a3e84080c6a3770a0bf8a856dfa79d333"
        "0000000000000000000000000000000000000000000000000000000000000000"
        "02cc7460f2c9ff107904cff671ec6fee57ba3dd7decf999fe9fe056f3fd4d56e");

    EXPECT_EQ(hex(msgPersonRecursiveFather.hashStruct()), "b852e5abfeff916a30cb940c4e24c43cfb5aeb0fa8318bdb10dd2ed15c8c70d8");

    EXPECT_EQ(hex(msgPersonRecursive.encodeHashes()), 
        "7c5c8e90cb92c8da53b893b24962513be98afcf1b57b00327ae4cc14e3a64116"
        "e8d55aa98b6b411f04dbcf9b23f29247bb0e335a6bc5368220032fdcb9e5927f"
        "9ebcfbf94f349de50bcb1e3aa4f1eb38824457c99914fefda27dcf9f99f6178b"
        "b852e5abfeff916a30cb940c4e24c43cfb5aeb0fa8318bdb10dd2ed15c8c70d8");

    EXPECT_EQ(hex(msgPersonRecursive.hashStruct()), "fdc7b6d35bbd81f7fa78708604f57569a10edff2ca329c8011373f0667821a45");

    ParamStruct msgEIP712Domain("EIP712Domain", std::vector<std::shared_ptr<ParamNamed>>{
        std::make_shared<ParamNamed>("name", std::make_shared<ParamString>("Family Tree")),
        std::make_shared<ParamNamed>("version", std::make_shared<ParamString>("1")),
        std::make_shared<ParamNamed>("chainId", std::make_shared<ParamUInt256>(1)),
        std::make_shared<ParamNamed>("verifyingContract", std::make_shared<ParamAddress>(parse_hex("CcCCccccCCCCcCCCCCCcCcCccCcCCCcCcccccccC")))
    });

    EXPECT_EQ(hex(msgEIP712Domain.hashStruct()), "facb2c1888f63a780c84c216bd9a81b516fc501a19bae1fc81d82df590bbdc60");

    Address address = Address(privateKeyDragon.getPublicKey(TWPublicKeyTypeSECP256k1Extended));
    EXPECT_EQ(address.string(), "0x065a687103C9F6467380beE800ecD70B17f6b72F");
}

TEST(EthereumAbiStruct, encodeTypes_v4Rec_Json) {
    auto hash = ParamStruct::hashStructJson(
        R"({
            "types": {
                "EIP712Domain": [
                    {"name": "name", "type": "string"},
                    {"name": "version", "type": "string"},
                    {"name": "chainId", "type": "uint256"},
                    {"name": "verifyingContract", "type": "address"}
                ],
                "Person": [
                    {"name": "name", "type": "string"},
                    {"name": "mother", "type": "Person"},
                    {"name": "father", "type": "Person"}
                ]
            },
            "primaryType": "Person",
            "domain": {
                "name": "Family Tree",
                "version": "1",
                "chainId": 1,
                "verifyingContract": "0xCcCCccccCCCCcCCCCCCcCcCccCcCCCcCcccccccC"
            },
            "message": {
                "name": "Jon",
                "mother": {
                    "name": "Lyanna",
                    "father": {
                        "name": "Rickard"
                    }
                },
                "father": {
                    "name": "Rhaegar",
                    "father": {
                        "name": "Aeris II"
                    }
                }
            }
        })");
    ASSERT_EQ(hex(hash), "807773b9faa9879d4971b43856c4d60c2da15c6f8c062bd9d33afefb756de19c");

    // sign the hash
    const auto rsv = Signer::sign(privateKeyDragon, 0, hash);
    EXPECT_EQ(hex(store(rsv.r)), "f2ec61e636ff7bb3ac8bc2a4cc2c8b8f635dd1b2ec8094c963128b358e79c85c");
    EXPECT_EQ(hex(store(rsv.s)), "5ca6dd637ed7e80f0436fe8fce39c0e5f2082c9517fe677cc2917dcd6c84ba88");
    EXPECT_EQ(hex(store(rsv.v)), "1c");
}

// See 'signedTypeData' in https://github.com/MetaMask/eth-sig-util/blob/main/test/index.ts
TEST(EthereumAbiStruct, encodeTypeCow1) {
    ParamStruct msgPersonCow1("Person", std::vector<std::shared_ptr<ParamNamed>>{
        std::make_shared<ParamNamed>("name", std::make_shared<ParamString>("Cow")),
        std::make_shared<ParamNamed>("wallet", std::make_shared<ParamAddress>(parse_hex("CD2a3d9F938E13CD947Ec05AbC7FE734Df8DD826")))
    });

    EXPECT_EQ(msgPersonCow1.encodeType(), "Person(string name,address wallet)");

    EXPECT_EQ(hex(msgPersonCow1.hashType()), "b9d8c78acf9b987311de6c7b45bb6a9c8e1bf361fa7fd3467a2163f994c79500");

    EXPECT_EQ(hex(msgPersonCow1.encodeHashes()), "b9d8c78acf9b987311de6c7b45bb6a9c8e1bf361fa7fd3467a2163f994c795008c1d2bd5348394761719da11ec67eedae9502d137e8940fee8ecd6f641ee1648000000000000000000000000cd2a3d9f938e13cd947ec05abc7fe734df8dd826");

    EXPECT_EQ(hex(msgPersonCow1.hashStruct()), "fc71e5fa27ff56c350aa531bc129ebdf613b772b6604664f5d8dbe21b85eb0c8");
}

TEST(EthereumAbiStruct, hashStructJson) {
    {
        auto hash = ParamStruct::hashStructJson(
            R"({
                "types": {
                    "EIP712Domain": [
                        {"name": "name", "type": "string"},
                        {"name": "version", "type": "string"},
                        {"name": "chainId", "type": "uint256"},
                        {"name": "verifyingContract", "type": "address"}
                    ],
                    "Person": [
                        {"name": "name", "type": "string"},
                        {"name": "wallet", "type": "address"}
                    ]
                },
                "primaryType": "Person",
                "domain": {
                    "name": "Ether Person",
                    "version": "1",
                    "chainId": 1,
                    "verifyingContract": "0xCcCCccccCCCCcCCCCCCcCcCccCcCCCcCcccccccC"
                },
                "message": {
                    "name": "Cow",
                    "wallet": "CD2a3d9F938E13CD947Ec05AbC7FE734Df8DD826"
                }
            })");
        ASSERT_EQ(hex(hash), "0b4bb85394b9ebb1c2425e283c9e734a9a7a832622e97c998f77e1c7a3f01a20");
    }
    {   // edge cases
        EXPECT_EXCEPTION(ParamStruct::hashStructJson("NOT_A_JSON"), "Could not parse Json");
        EXPECT_EXCEPTION(ParamStruct::hashStructJson("+/{\\"), "Could not parse Json");
        EXPECT_EXCEPTION(ParamStruct::hashStructJson(""), "Could not parse Json");
        EXPECT_EXCEPTION(ParamStruct::hashStructJson("0"), "Expecting Json object");
        EXPECT_EXCEPTION(ParamStruct::hashStructJson("[]"), "Expecting Json object");
        EXPECT_EXCEPTION(ParamStruct::hashStructJson(R"({})"), "Top-level string field 'primaryType' missing");
        EXPECT_EXCEPTION(ParamStruct::hashStructJson(R"({"domain": {}, "message": {}, "types": {}})"), "Top-level string field 'primaryType' missing");
        EXPECT_EXCEPTION(ParamStruct::hashStructJson(R"({"primaryType": [], "domain": {}, "message": {}, "types": {}})"), "Top-level string field 'primaryType' missing");
        EXPECT_EXCEPTION(ParamStruct::hashStructJson(R"({"primaryType": "v1", "message": {}, "types": {}})"), "Top-level object field 'domain' missing");
        EXPECT_EXCEPTION(ParamStruct::hashStructJson(R"({"primaryType": "v1", "domain": "vDomain", "message": {}, "types": {}})"), "Top-level object field 'domain' missing");
        EXPECT_EXCEPTION(ParamStruct::hashStructJson(R"({"primaryType": "v1", "domain": {}, "types": {}})"), "Top-level object field 'message' missing");
        EXPECT_EXCEPTION(ParamStruct::hashStructJson(R"({"primaryType": "v1", "domain": {}, "message": "v2", "types": {}})"), "Top-level object field 'message' missing");
        EXPECT_EXCEPTION(ParamStruct::hashStructJson(R"({"primaryType": "v1", "domain": {}, "message": {}})"), "Top-level object field 'types' missing");
        EXPECT_EXCEPTION(ParamStruct::hashStructJson(R"({"primaryType": "v1", "domain": {}, "message": {}, "types": "vTypes"})"), "Top-level object field 'types' missing");
        EXPECT_EXCEPTION(ParamStruct::hashStructJson(R"({"primaryType": "v1", "domain": {}, "message": {}, "types": {}})"), "Type not found, EIP712Domain");
        EXPECT_EXCEPTION(ParamStruct::hashStructJson(R"({"primaryType": "v1", "domain": {}, "message": {}, "types": {"EIP712Domain": []}})"), "No valid params found");
        EXPECT_EXCEPTION(ParamStruct::hashStructJson(R"({"primaryType": "v1", "domain": {}, "message": {}, "types": {"EIP712Domain": [{"name": "param", "type": "type"}]}})"), "Unknown type type");
        EXPECT_EXCEPTION(ParamStruct::hashStructJson(R"({"primaryType": "v1", "domain": {"param": "val"}, "message": {}, "types": {"EIP712Domain": [{"name": "param", "type": "string"}]}})"), "Type not found, v1");
    }
}

TEST(EthereumAbiStruct, hashStruct_walletConnect) {
    // https://github.com/WalletConnect/walletconnect-example-dapp/blob/master/src/helpers/eip712.ts
    auto path = TESTS_ROOT + "/Ethereum/Data/eip712_walletconnect.json";
    auto typeData = load_file(path);
    auto hash = ParamStruct::hashStructJson(typeData);
    EXPECT_EQ(hex(hash), "abc79f527273b9e7bca1b3f1ac6ad1a8431fa6dc34ece900deabcd6969856b5e");

    // sign the hash
    const auto rsv = Signer::sign(privateKeyOilTimes12, 0, hash);
    EXPECT_EQ(hex(store(rsv.r)), "e9c1ce1307593c378c7e38e8aa00dfb42b5a1ce543b59a138a12f29bd7fea75c");
    EXPECT_EQ(hex(store(rsv.s)), "3fe71ef91c37abea29fe14b5f0de805f924af19d71bcef09e74aef2f0ccdf52a");
    EXPECT_EQ(hex(store(rsv.v)), "1c");
}

TEST(EthereumAbiStruct, hashStruct_cryptofights) {
    auto path = TESTS_ROOT + "/Ethereum/Data/eip712_cryptofights.json";
    auto typeData = load_file(path);
    auto hash = ParamStruct::hashStructJson(typeData);
    EXPECT_EQ(hex(hash), "db12328a6d193965801548e1174936c3aa7adbe1b54b3535a3c905bd4966467c");

    // sign the hash
    const auto rsv = Signer::sign(privateKeyOilTimes12, 0, hash);
    EXPECT_EQ(hex(store(rsv.r)), "9e26bdf0d113a72805acb1c2c8b0734d264290fd1cfbdf5e6502ae65a2f2bd83");
    EXPECT_EQ(hex(store(rsv.s)), "11512c15ad0833fd457ae5dd59c3bcb3d03f35b3d33c1c5a575852163db42369");
    EXPECT_EQ(hex(store(rsv.v)), "1b");
}

TEST(EthereumAbiStruct, hashStruct_snapshot) {
    auto path = TESTS_ROOT + "/Ethereum/Data/eip712_snapshot_v4.json";
    auto typeData = load_file(path);
    auto hash = ParamStruct::hashStructJson(typeData);
    EXPECT_EQ(hex(hash), "f558d08ad4a7651dbc9ec028cfcb4a8e6878a249073ef4fa694f85ee95f61c0f");

    // sign the hash
    const auto rsv = Signer::sign(privateKeyOilTimes12, 0, hash);
    EXPECT_EQ(hex(store(rsv.r)), "9da563ffcafe9fa8809540ebcc4bcf8bbc26874e192f430432e06547593e8681");
    EXPECT_EQ(hex(store(rsv.s)), "164808603aca259775bdf511124b58651f1b3ce9ccbcd5a8d63df02e2359bb8b");
    EXPECT_EQ(hex(store(rsv.v)), "1b");
}

TEST(EthereumAbiStruct, ParamFactoryMakeNamed) {
    std::shared_ptr<ParamNamed> p = ParamFactory::makeNamed("firstparam", "uint256");
    EXPECT_EQ(p->getName(), "firstparam");
    ASSERT_NE(p->getParam().get(), nullptr);
    EXPECT_EQ(p->getParam()->getType(), "uint256");
}

TEST(EthereumAbiStruct, ParamStructMakeStruct) {
    {
        std::shared_ptr<ParamStruct> s = ParamStruct::makeStruct("Person",
            R"(
                {"name": "Cow", "wallet": "CD2a3d9F938E13CD947Ec05AbC7FE734Df8DD826"}
            )",
            R"({
                "Person": [{"name": "name", "type": "string"}, {"name": "wallet", "type": "address"}]
            })");
        ASSERT_NE(s.get(), nullptr);
        EXPECT_EQ(s->getType(), "Person");
        ASSERT_EQ(s->getCount(), 2);
        EXPECT_EQ(s->encodeType(), "Person(string name,address wallet)");
        EXPECT_EQ(hex(s->hashStruct()), "fc71e5fa27ff56c350aa531bc129ebdf613b772b6604664f5d8dbe21b85eb0c8");
    }
    {
        std::shared_ptr<ParamStruct> s = ParamStruct::makeStruct("Person",
            R"(
                {"name": "Cow", "wallets": ["CD2a3d9F938E13CD947Ec05AbC7FE734Df8DD826", "DeaDbeefdEAdbeefdEadbEEFdeadbeEFdEaDbeeF"]}
            )",
            R"({
                "Person": [{"name": "name", "type": "string"}, {"name": "wallets", "type": "address[]"}]
            })");
        ASSERT_NE(s.get(), nullptr);
        EXPECT_EQ(s->getType(), "Person");
        ASSERT_EQ(s->getCount(), 2);
        EXPECT_EQ(s->encodeType(), "Person(string name,address[] wallets)");
        EXPECT_EQ(hex(s->hashStruct()), "9b4846dd48b866f0ac54d61b9b21a9e746f921cefa4ee94c4c0a1c49c774f67f");
    }
    {
        std::shared_ptr<ParamStruct> s = ParamStruct::makeStruct("Mail",
            R"({"from": {"name": "Cow", "wallet": "CD2a3d9F938E13CD947Ec05AbC7FE734Df8DD826"}, "to": {"name": "Bob", "wallet": "bBbBBBBbbBBBbbbBbbBbbbbBBbBbbbbBbBbbBBbB"}, "contents": "Hello, Bob!"})",
            R"({"Person": [{"name": "name", "type": "string"}, {"name": "wallet", "type": "address"}],"Mail": [{"name": "from", "type": "Person"},{"name": "to", "type": "Person"},{"name": "contents", "type": "string"}]})");
        ASSERT_NE(s.get(), nullptr);
        EXPECT_EQ(s->getType(), "Mail");
        ASSERT_EQ(s->getCount(), 3);
        EXPECT_EQ(s->encodeType(), "Mail(Person from,Person to,string contents)Person(string name,address wallet)");
        EXPECT_EQ(hex(s->hashStruct()), "c52c0ee5d84264471806290a3f2c4cecfc5490626bf912d01f240d7a274b371e");
    }

    {   // extra param
        std::shared_ptr<ParamStruct> s = ParamStruct::makeStruct("Person",
            R"({"extra_param": "extra_value", "name": "Cow", "wallet": "CD2a3d9F938E13CD947Ec05AbC7FE734Df8DD826"})",
            R"({"Person": [{"name": "name", "type": "string"}, {"name": "wallet", "type": "address"}]})");
        ASSERT_NE(s.get(), nullptr);
        EXPECT_EQ(s->encodeType(), "Person(string name,address wallet)");
        EXPECT_EQ(hex(s->hashStruct()), "fc71e5fa27ff56c350aa531bc129ebdf613b772b6604664f5d8dbe21b85eb0c8");
    }
    {   // empty array
        std::shared_ptr<ParamStruct> s = ParamStruct::makeStruct("Person",
            R"({"extra_param": "extra_value", "name": "Cow", "wallets": []})",
            R"({"Person": [{"name": "name", "type": "string"}, {"name": "wallets", "type": "address[]"}]})");
        ASSERT_NE(s.get(), nullptr);
        EXPECT_EQ(s->encodeType(), "Person(string name,address[] wallets)");
    }
    {   // missing param
        std::shared_ptr<ParamStruct> s = ParamStruct::makeStruct("Person",
            R"({"wallet": "CD2a3d9F938E13CD947Ec05AbC7FE734Df8DD826"})",
            R"({"Person": [{"name": "name", "type": "string"}, {"name": "wallet", "type": "address"}]})");
        ASSERT_NE(s.get(), nullptr);
        EXPECT_EQ(s->encodeType(), "Person(string name,address wallet)");
    }

    {
        EXPECT_EXCEPTION(ParamStruct::makeStruct("Person",
            "NOT_A_JSON+/{\\",
            R"({"Person": [{"name": "name", "type": "string"}, {"name": "wallet", "type": "address"}]})"),
            "Could not parse value Json");
    }
    {
        EXPECT_EXCEPTION(ParamStruct::makeStruct("Person",
            "0",
            R"({"Person": [{"name": "name", "type": "string"}, {"name": "wallet", "type": "address"}]})"),
            "Expecting object");
    }
    {
        EXPECT_EXCEPTION(ParamStruct::makeStruct("Person",
            // params mixed up
            R"({"wallets": "Cow", "name": ["CD2a3d9F938E13CD947Ec05AbC7FE734Df8DD826", "DeaDbeefdEAdbeefdEadbEEFdeadbeEFdEaDbeeF"]})",
            R"({"Person": [{"name": "name", "type": "string"}, {"name": "wallets", "type": "address[]"}]})"),
            "Could not set type for param wallets");
    }
    {
        EXPECT_EXCEPTION(ParamStruct::makeStruct("Person",
            R"({"name": "Cow", "wallets": ["CD2a3d9F938E13CD947Ec05AbC7FE734Df8DD826", "DeaDbeefdEAdbeefdEadbEEFdeadbeEFdEaDbeeF"]})",
            R"({"Person": [{"name": "name", "type": "string"}, {"name": "wallets", "type": "missingtype[]"}]})"),
            "Unknown struct array type missingtype");
    }
    {
        EXPECT_EXCEPTION(ParamStruct::makeStruct("Person",
            R"({"name": "Cow", "wallets": "NOT_AN_ARRAY"})",
            R"({"Person": [{"name": "name", "type": "string"}, {"name": "wallets", "type": "address[]"}]})"),
            "Could not set type for param wallets");
    }
    {
        EXPECT_EXCEPTION(ParamStruct::makeStruct("Mail",
            R"({"from": {"name": "Cow", "wallet": "CD2a3d9F938E13CD947Ec05AbC7FE734Df8DD826"}, "to": {"name": "Bob", "wallet": "bBbBBBBbbBBBbbbBbbBbbbbBBbBbbbbBbBbbBBbB"}, "contents": "Hello, Bob!"})",
            R"({"Mail": [{"name": "from", "type": "Person"},{"name": "to", "type": "Person"},{"name": "contents", "type": "string"}]})"),
            "Unknown type Person");
    }
}

TEST(EthereumAbiStruct, ParamFactoryMakeTypes) {
    {
        std::vector<std::shared_ptr<ParamStruct>> tt = ParamStruct::makeTypes(R"({"Person": [{"name": "name", "type": "string"}, {"name": "wallet", "type": "address"}]})");
        ASSERT_EQ(tt.size(), 1);
        EXPECT_EQ(tt[0]->encodeType(), "Person(string name,address wallet)");
    }
    {
        std::vector<std::shared_ptr<ParamStruct>> tt = ParamStruct::makeTypes(
            R"({
                "Person": [{"name": "name", "type": "string"}, {"name": "wallet", "type": "address"}],
                "Mail": [{"name": "from", "type": "Person"}, {"name": "to", "type": "Person"}, {"name": "contents", "type": "string"}]
            })");
        ASSERT_EQ(tt.size(), 2);
        EXPECT_EQ(tt[0]->encodeType(), "Mail(Person from,Person to,string contents)Person(string name,address wallet)");
        EXPECT_EQ(tt[1]->encodeType(), "Person(string name,address wallet)");
    }
    {   // edge cases
        EXPECT_EXCEPTION(ParamStruct::makeTypes("NOT_A_JSON"), "Could not parse types Json");
        EXPECT_EXCEPTION(ParamStruct::makeTypes("+/{\\"), "Could not parse types Json");
        EXPECT_EXCEPTION(ParamStruct::makeTypes(""), "Could not parse types Json");
        EXPECT_EXCEPTION(ParamStruct::makeTypes("0"), "Expecting object");
        EXPECT_EXCEPTION(ParamStruct::makeTypes("[]"), "Expecting object");
        EXPECT_EXCEPTION(ParamStruct::makeTypes("[{}]"), "Expecting object");
        EXPECT_EQ(ParamStruct::makeTypes("{}").size(), 0);
        EXPECT_EXCEPTION(ParamStruct::makeTypes("{\"a\": 0}"), "Expecting array");
        EXPECT_EXCEPTION(ParamStruct::makeTypes(R"({"name": 0})"), "Expecting array");
        // order does not matter
        EXPECT_EQ(ParamStruct::makeTypes(R"({"Mail": [{"name": "from", "type": "Person"}, {"name": "to", "type": "Person"}, {"name": "contents", "type": "string"}], "Person": [{"name": "name", "type": "string"}, {"name": "wallet", "type": "address"}]})").size(), 2);
    }
}

TEST(EthereumAbiStruct, ParamFactoryMakeType) {
    {
        std::shared_ptr<ParamStruct> t = ParamStruct::makeType("Person", R"([{"name": "name", "type": "string"}, {"name": "wallet", "type": "address"}])");
        EXPECT_NE(t.get(), nullptr);
        EXPECT_EQ(t->getType(), "Person");
        ASSERT_EQ(t->getCount(), 2);
        ASSERT_EQ(t->encodeType(), "Person(string name,address wallet)");
    }
    {   // edge cases
        EXPECT_EXCEPTION(ParamStruct::makeType("", ""), "Missing type name");
        EXPECT_EXCEPTION(ParamStruct::makeType("Person", "NOT_A_JSON"), "Could not parse type Json");
        EXPECT_EXCEPTION(ParamStruct::makeType("Person", "+/{\\"), "Could not parse type Json");
        EXPECT_EXCEPTION(ParamStruct::makeType("Person", ""), "Could not parse type Json");
        EXPECT_EXCEPTION(ParamStruct::makeType("Person", "0"), "Expecting array");
        EXPECT_EXCEPTION(ParamStruct::makeType("Person", "{}"), "Expecting array");
        EXPECT_EXCEPTION(ParamStruct::makeType("Person", "[]"), "No valid params found");
        EXPECT_EXCEPTION(ParamStruct::makeType("Person", R"([{"dummy": 0}])"), "Could not process Json: ");
        EXPECT_EXCEPTION(ParamStruct::makeType("Person", R"([{"name": "val"}])"), "Could not process Json: ");
        EXPECT_EXCEPTION(ParamStruct::makeType("Person", R"([{"type": "val"}])"), "Could not process Json: ");
        EXPECT_EXCEPTION(ParamStruct::makeType("Person", R"([{"name": "", "type": "type"}])"), "Expecting 'name' and 'type', in Person");
        EXPECT_EXCEPTION(ParamStruct::makeType("Person", R"([{"name": "name", "type": ""}])"), "Expecting 'name' and 'type', in Person");
        EXPECT_EXCEPTION(ParamStruct::makeType("Person", R"([{"name": "name", "type": "UNKNOWN_TYPE"}, {"name": "wallet", "type": "address"}])"), "Unknown type UNKNOWN_TYPE");
        EXPECT_EQ(ParamStruct::makeType("Person", R"([{"name": "name", "type": "UNKNOWN_TYPE"}, {"name": "wallet", "type": "address"}])", {}, true)->encodeType(), "Person(UNKNOWN_TYPE name,address wallet)UNKNOWN_TYPE()");
    }
}

TEST(EthereumAbiStruct, ParamNamedMethods) {
    const auto ps = std::make_shared<ParamString>("Hello");
    auto pn = std::make_shared<ParamNamed>("name", ps);

    EXPECT_EQ(pn->getSize(), ps->getSize());
    EXPECT_EQ(pn->isDynamic(), ps->isDynamic());
    Data encoded;
    pn->encode(encoded);
    EXPECT_EQ(hex(encoded), "000000000000000000000000000000000000000000000000000000000000000548656c6c6f000000000000000000000000000000000000000000000000000000");
    size_t offset = 0;
    EXPECT_EQ(pn->decode(encoded, offset), true);
    EXPECT_EQ(offset, 64);
    pn->setValueJson("World");
    EXPECT_EQ(ps->getVal(), "World");
}

TEST(EthereumAbiStruct, ParamSetNamed) {
    const auto pn1 = std::make_shared<ParamNamed>("param1", std::make_shared<ParamString>("Hello"));
    const auto pn2 = std::make_shared<ParamNamed>("param2", std::make_shared<ParamString>("World"));
    auto ps = std::make_shared<ParamSetNamed>(std::vector<std::shared_ptr<ParamNamed>>{pn1, pn2});
    EXPECT_EQ(ps->getCount(), 2);
    EXPECT_EQ(ps->addParam(std::shared_ptr<ParamNamed>(nullptr)), -1);
    EXPECT_EQ(ps->findParamByName("NO_SUCH_PARAM"), nullptr);
    auto pf1 = ps->findParamByName("param2");
    ASSERT_NE(pf1.get(), nullptr);
    EXPECT_EQ(pf1->getName(), "param2");
}

TEST(EthereumAbiStruct, ParamStructMethods) {
    const auto pn1 = std::make_shared<ParamNamed>("param1", std::make_shared<ParamString>("Hello"));
    const auto pn2 = std::make_shared<ParamNamed>("param2", std::make_shared<ParamString>("World"));
    auto ps = std::make_shared<ParamStruct>("struct", std::vector<std::shared_ptr<ParamNamed>>{pn1, pn2});

    EXPECT_EQ(ps->getSize(), 2);
    EXPECT_EQ(ps->isDynamic(), true);
    Data encoded;
    ps->encode(encoded);
    EXPECT_EQ(hex(encoded), "");
    size_t offset = 0;
    EXPECT_EQ(ps->decode(encoded, offset), true);
    EXPECT_EQ(offset, 0);
    EXPECT_FALSE(ps->setValueJson("dummy"));
    EXPECT_EQ(ps->findParamByName("param2")->getName(), "param2");
}

TEST(EthereumAbiStruct, ParamHashStruct) {
    {
        auto p = std::make_shared<ParamUInt8>();
        EXPECT_TRUE(p->setValueJson("13"));
        EXPECT_EQ(hex(p->hashStruct()), "000000000000000000000000000000000000000000000000000000000000000d");
    }
    {
        auto p = std::make_shared<ParamUInt16>();
        EXPECT_TRUE(p->setValueJson("1234"));
        EXPECT_EQ(hex(p->hashStruct()), "00000000000000000000000000000000000000000000000000000000000004d2");
    }
    {
        auto p = std::make_shared<ParamUInt32>();
        EXPECT_TRUE(p->setValueJson("1234567"));
        EXPECT_EQ(hex(p->hashStruct()), "000000000000000000000000000000000000000000000000000000000012d687");
    }
    {
        auto p = std::make_shared<ParamUInt64>();
        EXPECT_TRUE(p->setValueJson("1234567"));
        EXPECT_EQ(hex(p->hashStruct()), "000000000000000000000000000000000000000000000000000000000012d687");
    }
    {
        auto p = std::make_shared<ParamUIntN>(128);
        EXPECT_TRUE(p->setValueJson("1234567890123456789"));
        EXPECT_EQ(hex(p->hashStruct()), "000000000000000000000000000000000000000000000000112210f47de98115");
    }
    {
        auto p = std::make_shared<ParamUIntN>(168);
        EXPECT_TRUE(p->setValueJson("1234567890123456789"));
        EXPECT_EQ(hex(p->hashStruct()), "000000000000000000000000000000000000000000000000112210f47de98115");
    }
    {
        auto p = std::make_shared<ParamUInt256>();
        EXPECT_TRUE(p->setValueJson("1234567890123456789"));
        EXPECT_EQ(hex(p->hashStruct()), "000000000000000000000000000000000000000000000000112210f47de98115");
    }
    {
        auto p = std::make_shared<ParamInt8>();
        EXPECT_TRUE(p->setValueJson("13"));
        EXPECT_EQ(hex(p->hashStruct()), "000000000000000000000000000000000000000000000000000000000000000d");
    }
    {
        auto p = std::make_shared<ParamInt16>();
        EXPECT_TRUE(p->setValueJson("1234"));
        EXPECT_EQ(hex(p->hashStruct()), "00000000000000000000000000000000000000000000000000000000000004d2");
    }
    {
        auto p = std::make_shared<ParamInt32>();
        EXPECT_TRUE(p->setValueJson("1234567"));
        EXPECT_EQ(hex(p->hashStruct()), "000000000000000000000000000000000000000000000000000000000012d687");
    }
    {
        auto p = std::make_shared<ParamInt64>();
        EXPECT_TRUE(p->setValueJson("1234567"));
        EXPECT_EQ(hex(p->hashStruct()), "000000000000000000000000000000000000000000000000000000000012d687");
    }
    {
        auto p = std::make_shared<ParamIntN>(128);
        EXPECT_TRUE(p->setValueJson("1234567890123456789"));
        EXPECT_EQ(hex(p->hashStruct()), "000000000000000000000000000000000000000000000000112210f47de98115");
    }
    {
        auto p = std::make_shared<ParamIntN>(168);
        EXPECT_TRUE(p->setValueJson("1234567890123456789"));
        EXPECT_EQ(hex(p->hashStruct()), "000000000000000000000000000000000000000000000000112210f47de98115");
    }
    {
        auto p = std::make_shared<ParamInt256>();
        EXPECT_TRUE(p->setValueJson("1234567890123456789"));
        EXPECT_EQ(hex(p->hashStruct()), "000000000000000000000000000000000000000000000000112210f47de98115");
    }
    {
        auto p = std::make_shared<ParamBool>();
        EXPECT_TRUE(p->setValueJson("true"));
        EXPECT_EQ(hex(p->hashStruct()), "0000000000000000000000000000000000000000000000000000000000000001");
    }
    {
        auto p = std::make_shared<ParamString>();
        EXPECT_TRUE(p->setValueJson("ABCdefGHI"));
        EXPECT_EQ(hex(p->hashStruct()), "3a2aa9c027187dbf5a2f0c980281da43e810ecbe4d32e0b5c22211882c691889");
    }
    {
        auto p = std::make_shared<ParamByteArray>();
        EXPECT_TRUE(p->setValueJson("0123456789"));
        EXPECT_EQ(hex(p->hashStruct()), "0123456789000000000000000000000000000000000000000000000000000000");
        EXPECT_TRUE(p->setValueJson("0xa9059cbb0000000000000000000000002e0d94754b348d208d64d52d78bcd443afa9fa520000000000000000000000000000000000000000000000000000000000000007"));
        EXPECT_EQ(hex(p->hashStruct()), "a9485354dd9d340e02789cfc540c6c4a2ff5511beb414b64634a5e11c6a7168c");
        EXPECT_TRUE(p->setValueJson("0x0000000000000000000000000000000000000000000000000000000123456789"));
        EXPECT_EQ(hex(p->hashStruct()), "0000000000000000000000000000000000000000000000000000000123456789");
    }
    {
        auto p = std::make_shared<ParamByteArrayFix>(36);
        EXPECT_TRUE(p->setValueJson("0x000000000000000000000000000000000000000000000000000000000000000123456789"));
        EXPECT_EQ(hex(p->hashStruct()), "3deb4663f580c622d668f2121c29c3f4dacf06e40a3a76d1dea25e90bcd63b5d");
    }
    {
        auto p = std::make_shared<ParamByteArrayFix>(20);
        EXPECT_TRUE(p->setValueJson("0x0000000000000000000000000000000123456789"));
        EXPECT_EQ(hex(p->hashStruct()), "0000000000000000000000000000000123456789000000000000000000000000");
    }
    {
        auto p = std::make_shared<ParamByteArrayFix>(32);
        EXPECT_TRUE(p->setValueJson("0x0000000000000000000000000000000000000000000000000000000123456789"));
        EXPECT_EQ(hex(p->hashStruct()), "0000000000000000000000000000000000000000000000000000000123456789");
    }
    {
        auto p = std::make_shared<ParamAddress>();
        EXPECT_TRUE(p->setValueJson("0x0000000000000000000000000000000123456789"));
        EXPECT_EQ(hex(p->hashStruct()), "0000000000000000000000000000000000000000000000000000000123456789");
    }
    {
        auto p = std::make_shared<ParamArray>(std::make_shared<ParamUInt8>());
        EXPECT_TRUE(p->setValueJson("[13,14,15]"));
        EXPECT_EQ(hex(p->hashStruct()), "71494e9b6acbff3356f1292cc149101310110b6b13f835ae4665e4b00892fa83");
    }
    {
        auto p = std::make_shared<ParamArray>(std::make_shared<ParamAddress>());
        EXPECT_TRUE(p->setValueJson("[\"0x0000000000000000000000000000000123456789\"]"));
        EXPECT_EQ(hex(p->hashStruct()), "c8243991757dc8723e4976248127e573da4a2cbfad54b776d5a7c8d92b6e2a6b");
    }
    {
        auto p = std::make_shared<ParamArray>(std::make_shared<ParamBool>());
        EXPECT_TRUE(p->setValueJson("[true,false,true]"));
        EXPECT_EQ(hex(p->hashStruct()), "5c6090c0461491a2941743bda5c3658bf1ea53bbd3edcde54e16205e18b45792");
    }
}
