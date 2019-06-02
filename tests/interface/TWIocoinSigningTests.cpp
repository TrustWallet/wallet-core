// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Iocoin/OutPoint.h"
#include "Bitcoin/Script.h"
#include "Iocoin/Transaction.h"
#include "Iocoin/TransactionBuilder.h"
#include "Iocoin/TransactionSigner.h"
#include "Hash.h"
#include "HexCoding.h"
#include "PrivateKey.h"
#include "proto/Iocoin.pb.h"
#include "proto/Bitcoin.pb.h" //XXXX
#include "TWTestUtilities.h"
#include "Iocoin/Address.h"
#include <TrustWalletCore/TWBitcoinScript.h>
#include <TrustWalletCore/TWBitcoinTransactionSigner.h>
#include <TrustWalletCore/TWHash.h>
#include <TrustWalletCore/TWPrivateKey.h>

#include <gtest/gtest.h>

using namespace TW;
using namespace TW::Iocoin;

TEST(IocoinSigner, KeyTest) {
  auto  privateKey = PrivateKey(parse_hex("6a8e59ea2d81bf4b83db2783643a709d2eb68871e1da7f5637ce3f68a1bc9918"));
	const auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeSECP256k1);

    const auto address = TW::Iocoin::Address(publicKey, 0x67);
    ASSERT_EQ(address.string(), "iWDSWNtGwcGAsaQPdJtu5STX8wVz5THTd2"); 
}

TEST(IocoinSigning, SignP2PKH_1) {
	Bitcoin::Proto::SigningInput input;
  input.set_hash_type(TWSignatureHashTypeAll);
 input.set_amount(100000000);
 input.set_byte_fee(100);
 input.set_to_address("iZPDYnh31Qt9syqga7hYjxy5G6q4WHWBTK");
input.set_change_address("in2MDub2nA6oT5evFXhEqk7dJdr1GzLmt1");
 //input.set_time(1557670493);
  auto  utxoKey = parse_hex("6a8e59ea2d81bf4b83db2783643a709d2eb68871e1da7f5637ce3f68a1bc9918");

  input.add_private_key(utxoKey.data(), utxoKey.size());
  auto utxo = input.add_utxo();
	  auto utxoScript = Bitcoin::Script(parse_hex("76a91425510b7b765287b83c2ae025ad89b6580ae5af6888ac")); 

  utxo->set_script(utxoScript.bytes.data(), utxoScript.bytes.size());
  utxo->set_amount(200000000);
 //swap "5da0a1824bb0a9f7d2c859d0b271f3824fe207ec54f839625a506f71b6a7ae6e"
 auto hash = parse_hex("e6ea7a6b17f605a526938f45ce702ef4283f172b0d958c2d7f9a0bb4281a0ad5");
    utxo->mutable_out_point()->set_hash(hash.data(), hash.size());
	    utxo->mutable_out_point()->set_index(0);
  auto result = TW::Iocoin::TransactionSigner<Iocoin::Transaction>(std::move(input), 1557670493).sign();
      ASSERT_TRUE(result) << result.error();
    Data serialized;
    auto signedTx = result.payload();
    signedTx.encode(serialized);
 ASSERT_EQ(hex(serialized), "010000005d2ad85c01e6ea7a6b17f605a526938f45ce702ef4283f172b0d958c2d7f9a0bb4281a0ad5000000006a47304402204ecd97fcbe5535cbff45782f7a6751569dd94f8baa246e768f461cd4f04ab0370220489ae246667462de3980716965eb90108ab7b59242eb0c0d4f94c696f18cabaf0121030321d3b8aef78d1b325741b42218ba4c68085896d996e040d0b7054cad995422ffffffff0200e1f505000000001976a9144812c3c0ca31ee1b4d09904ed3548976ab47ba7b88ac605af405000000001976a914d2ba122b36087eed245087ff5a378bbca0b6e8b388ac00000000");
}

TEST(IocoinSigning, SignP2PKH_2) {
	Bitcoin::Proto::SigningInput input;
  input.set_hash_type(TWSignatureHashTypeAll);
 input.set_amount(50000000);
 input.set_byte_fee(100);
 input.set_to_address("iaRR5dtiFb2TLrYk6ok6bUsULx8jze2DFA");
input.set_change_address("imoDDjYUVZa5fJadaLjsENNS6nunGTo5AD");
 //XXXX input.set_time(1558212449);
 auto  utxoKey = parse_hex("36c39b26f6c5481d99056482d3a3698d41ea1dca342ae486e004d5f890f0c101");
  auto  privateKey = PrivateKey(utxoKey);
 const auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeSECP256k1);

 const auto address = TW::Iocoin::Address(publicKey, 0x67);
 ASSERT_EQ(address.string(), "iWwzWG5BqGfNHWmBodsGtKzXjFNNUmQ7TN"); 

  input.add_private_key(utxoKey.data(), utxoKey.size());
  auto utxo = input.add_utxo();
  auto utxoScript = Bitcoin::Script(parse_hex("76a9142d5d39ced4aced8463998458f31aead5c62d461388ac")); 

  utxo->set_script(utxoScript.bytes.data(), utxoScript.bytes.size());
  utxo->set_amount(100000000);
 //swap "f683048b015ca1dbf8a61f778e16067483f47d7183c849c40a4b04c26179a0e3"
 auto hash = parse_hex("e3a07961c2044b0ac449c883717df4837406168e771fa6f8dba15c018b0483f6");
    utxo->mutable_out_point()->set_hash(hash.data(), hash.size());
	    utxo->mutable_out_point()->set_index(0);
  auto result = TW::Iocoin::TransactionSigner<Iocoin::Transaction>(std::move(input),1558212449).sign();
      ASSERT_TRUE(result) << result.error();
    Data serialized;
    auto signedTx = result.payload();
    signedTx.encode(serialized);
 ASSERT_EQ(hex(serialized), "01000000616fe05c01e3a07961c2044b0ac449c883717df4837406168e771fa6f8dba15c018b0483f6000000006b483045022100e4dc0073faf0735441fc85e1bac91fbbbda98622db0af927e5481360bd81a7ef02205f8917109a7c2c2539e52a0c8f7dbd2e5c7d1118ba551971013b86fe2c2ebe770121034d58f0a26ca65949a188b3d8d262f915c0e71ea31b14f9dd878557527aee1cbdffffffff0280f0fa02000000001976a914537559437955b94d415fc40728e359ae5349ac7988ace069f902000000001976a914d03dfba0277dbf161ffb13e30bb0e647855cf7d388ac00000000");
}
TEST(IocoinSigning, SignP2PKH_3) {
	Bitcoin::Proto::SigningInput input;
  input.set_hash_type(TWSignatureHashTypeAll);
 input.set_amount(10000000);
 input.set_byte_fee(100);
 input.set_to_address("iYhZVFoFtp1XVL7TPSYNeeJi7x9WJsHxYz");
input.set_change_address("ieAeT6f4PnBeCYCZeQrhsGEph5wGpg15Tk");
 //XXXX input.set_time(1558307035);
 auto  utxoKey = parse_hex("5f65dac0c634e78fe0d84e96278449385b47d44a431a8bf96f054f0b553d085e");
  auto  privateKey = PrivateKey(utxoKey);
 const auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeSECP256k1);

 const auto address = TW::Iocoin::Address(publicKey, 0x67);
 ASSERT_EQ(address.string(), "iX52LUtQiFMTW5nqEPg9wNCxS5X6yQtgu8"); 

  input.add_private_key(utxoKey.data(), utxoKey.size());
  auto utxo = input.add_utxo();
  auto utxoScript = Bitcoin::Script(parse_hex("76a9142eb1aa93ba9de5765ae903737fb5a27018bb6c2c88ac")); 

  utxo->set_script(utxoScript.bytes.data(), utxoScript.bytes.size());
  utxo->set_amount(50000000);
 //swap "c37ecdcb8405662cc2d6b92da0148652d7eaa8a7be38953e808cfbfbbc5cfd17"
 auto hash = parse_hex("17fd5cbcfbfb8c803e9538bea7a8ead7528614a02db9d6c22c660584cbcd7ec3");
    utxo->mutable_out_point()->set_hash(hash.data(), hash.size());
	    utxo->mutable_out_point()->set_index(0);
  auto ts = TW::Iocoin::TransactionSigner<Iocoin::Transaction>(std::move(input),1558307035);
  auto result = ts.sign();
      ASSERT_TRUE(result) << result.error();
    Data serialized;
    auto signedTx = result.payload();
    signedTx.encode(serialized);
 ASSERT_EQ(hex(serialized), "01000000dbe0e15c0117fd5cbcfbfb8c803e9538bea7a8ead7528614a02db9d6c22c660584cbcd7ec3000000006a4730440220762bc79f1bd59ef41f8241e05a0ea8b00e3664cdfe96d8da264e43b01539ca1702207fb6193778990a758108b1014e4ab6b7cd25187f514a7fcb562bb7aefdfff8480121031aab68ed3a99ebfd205f3b49cc5898d947eda5d62b67a57f43ef43972d123370ffffffff0280969800000000001976a9144092c53fd38ae2bea725f7dab2a3cc946ae44b6a88ac60d36002000000001976a9147c8ac5ebc8dd00bc2d75201d80d9b0482b84592088ac00000000");
}

TEST(IocoinSigning, SignP2PKH_4) {
	Bitcoin::Proto::SigningInput input;
  input.set_hash_type(TWSignatureHashTypeAll);
 input.set_amount(10000000);
 input.set_byte_fee(100);
 input.set_to_address("ik4PotegQBUL574VtWvJgX2RoYcwGKVrQH");
input.set_change_address("ibPvdeVwmpeyVLsk4p4n5ngCPdzF24S9kg");
 //XXXX input.set_time(1558620026);

 auto  utxoKey1 = parse_hex("18702dcddff023581b9ef2b84864af50d3974066dfc56c21fa6dee637245698b");
  auto  privateKey1 = PrivateKey(utxoKey1);
 const auto publicKey1 = privateKey1.getPublicKey(TWPublicKeyTypeSECP256k1);
 const auto address1 = TW::Iocoin::Address(publicKey1, 0x67);
 ASSERT_EQ(address1.string(), "ieAeT6f4PnBeCYCZeQrhsGEph5wGpg15Tk"); 
 input.add_private_key(utxoKey1.data(), utxoKey1.size());
  auto utxo1 = input.add_utxo();
  auto utxoScript1 = Bitcoin::Script(parse_hex("76a9147c8ac5ebc8dd00bc2d75201d80d9b0482b84592088ac")); 
  utxo1->set_script(utxoScript1.bytes.data(), utxoScript1.bytes.size());
  utxo1->set_amount(39900000);
 //swap "44feeefe3bc3435c1a843f9e7466048e806bfa7906c501890e7d8faa90713d94"
 auto hash1 = parse_hex("943d7190aa8f7d0e8901c50679fa6b808e0466749e3f841a5c43c33bfeeefe44");
   utxo1->mutable_out_point()->set_hash(hash1.data(), hash1.size());
  utxo1->mutable_out_point()->set_index(1);

 auto  utxoKey2 = parse_hex("0b97a9a5f658c52bb0897e1b9edb5f8d5cc61fdabb38fdf628113707bf69714c");
  auto  privateKey2 = PrivateKey(utxoKey2);
 const auto publicKey2 = privateKey2.getPublicKey(TWPublicKeyTypeSECP256k1);
 const auto address2 = TW::Iocoin::Address(publicKey2, 0x67);
ASSERT_EQ(address2.string(), "iaRR5dtiFb2TLrYk6ok6bUsULx8jze2DFA"); 
  input.add_private_key(utxoKey2.data(), utxoKey2.size());
  auto utxo2 = input.add_utxo();
  auto utxoScript2 = Bitcoin::Script(parse_hex("76a914537559437955b94d415fc40728e359ae5349ac7988ac")); 
  utxo2->set_script(utxoScript2.bytes.data(), utxoScript2.bytes.size());
  utxo2->set_amount(50000000);
  //swap "b3681ff1c4f2d7218d7b65f7f95e60398fcba4e88ad60acf0ef56fbabd91e859"
  auto hash2 = parse_hex("59e891bdba6ff50ecf0ad68ae8a4cb8f39605ef9f7657b8d21d7f2c4f11f68b3"); 
  utxo2->mutable_out_point()->set_hash(hash2.data(), hash2.size());
  utxo2->mutable_out_point()->set_index(0);

 auto  utxoKey3 = parse_hex("e648eb192038fff1dc5371606eb7d117fe4c3455c039d16e331fe8d8cf51d6de");
  auto  privateKey3 = PrivateKey(utxoKey3);
 const auto publicKey3 = privateKey3.getPublicKey(TWPublicKeyTypeSECP256k1);
 const auto address3 = TW::Iocoin::Address(publicKey3, 0x67);
 ASSERT_EQ(address3.string(), "iW9UPDmw2u25TCzGpvYsp1otiGV9ZLhNNw"); 
  input.add_private_key(utxoKey3.data(), utxoKey3.size());
  auto utxo3 = input.add_utxo();
  auto utxoScript3 = Bitcoin::Script(parse_hex("76a9142490f2904c41e154f2dddabc0eec3e01210ea52688ac")); 
  utxo3->set_script(utxoScript3.bytes.data(), utxoScript3.bytes.size());
  utxo3->set_amount(249700000);
  //swap "c37ecdcb8405662cc2d6b92da0148652d7eaa8a7be38953e808cfbfbbc5cfd17"
  auto hash3 = parse_hex("17fd5cbcfbfb8c803e9538bea7a8ead7528614a02db9d6c22c660584cbcd7ec3"); 
  utxo3->mutable_out_point()->set_hash(hash3.data(), hash3.size());

  auto ts = TW::Iocoin::TransactionSigner<Iocoin::Transaction>(std::move(input),1558620026);
  auto result = ts.sign();
      ASSERT_TRUE(result) << result.error();
    Data serialized;
    auto signedTx = result.payload();
    signedTx.encode(serialized);

 ASSERT_EQ(hex(serialized), "010000007aa7e65c01943d7190aa8f7d0e8901c50679fa6b808e0466749e3f841a5c43c33bfeeefe44010000006b483045022100ad7a2ada3d4a3739c7acea6b14e4a37099a11c4a71ad1e0094954d685412d31002205ea5b07f2f4518bb0935b07bef9b6b60face22b6c3209b796dfa3fbd4dfb5d6f01210269126db996189cce60c7a6544397cc0705929e157f93d148a16b095322bb6a42ffffffff0280969800000000001976a914bd2ccf5905ed7b50ad94f28a75e49578b518f30a88ac40b6c601000000001976a9145e254fad043556b147672c36150d3a19049e36e688ac00000000");
}

TEST(IocoinSigning, SignP2PKH_5) {
	Bitcoin::Proto::SigningInput input;
  input.set_hash_type(TWSignatureHashTypeAll);
  input.set_amount(25000000);
  input.set_byte_fee(100);
  input.set_to_address("ir7z3i3XfyDbzDNwgFg939eGiXvrXwoz6R");
  input.set_change_address("if7Mj9RNtctB4PtvingfqT7EyhagyrGxFp");
  //XXXX input.set_time(1558693088);

  auto utxoKey1 = parse_hex("cc4f404144cfb7512a2570648a5a1418a52760c7a18bdbe17a155b1fecf838e9");
  auto  privateKey1 = PrivateKey(utxoKey1);
  const auto publicKey1 = privateKey1.getPublicKey(TWPublicKeyTypeSECP256k1);
  const auto address1 = TW::Iocoin::Address(publicKey1, 0x67);
  ASSERT_EQ(address1.string(), "icktHUr4KZLNrhBHmpvHazmLKpFuSjyEXD"); 

  input.add_private_key(utxoKey1.data(), utxoKey1.size());
  auto utxo1 = input.add_utxo();
  auto utxoScript1 = Bitcoin::Script(parse_hex("76a9146d14366388191ed0adec8df2fbcd56f986746d7888ac")); 
  utxo1->set_script(utxoScript1.bytes.data(), utxoScript1.bytes.size());
  utxo1->set_amount(10000000);
  //swap "87f6f07025cf658e6166135f9f47c545a9ab7f7102232c97f9746a82bab697c1"
  auto hash1 = parse_hex("c197b6ba826a74f9972c2302717faba945c5479f5f1366618e65cf2570f0f687");
  utxo1->mutable_out_point()->set_hash(hash1.data(), hash1.size());
  utxo1->mutable_out_point()->set_index(1);

  auto utxoKey2 = parse_hex("0108ab5536142a64f67682311afd3add5ccf064c1607e84f905b3300cc2f1769");
  auto  privateKey2 = PrivateKey(utxoKey2);
  const auto publicKey2 = privateKey2.getPublicKey(TWPublicKeyTypeSECP256k1);
  const auto address2 = TW::Iocoin::Address(publicKey2, 0x67);
  ASSERT_EQ(address2.string(), "ihYvDKcnroAzREPb92A7k4zcYghxgUq2mb"); 

  input.add_private_key(utxoKey2.data(), utxoKey2.size());
  auto utxo2 = input.add_utxo();
  auto utxoScript2 = Bitcoin::Script(parse_hex("76a914a1a975a122f67eca9d559edaf00689c8c408953988ac")); 
  utxo2->set_script(utxoScript2.bytes.data(), utxoScript2.bytes.size());
  utxo2->set_amount(10000000);
  //swap "a605eb93c2f7075b208ffee2f7bb715552efd4ef4755d8b0271ad5c0688a1baf"
  auto hash2 = parse_hex("af1b8a68c0d51a27b0d85547efd4ef525571bbf7e2fe8f205b07f7c293eb05a6"); 
  utxo2->mutable_out_point()->set_hash(hash2.data(), hash2.size());
  utxo2->mutable_out_point()->set_index(1);

  auto utxoKey3 = parse_hex("76133ba9bf2847140560ee516d0ddc8909aec8098a609feb9ce807bb8e5d53ab");
  auto  privateKey3 = PrivateKey(utxoKey3);
  const auto publicKey3 = privateKey3.getPublicKey(TWPublicKeyTypeSECP256k1);
  const auto address3 = TW::Iocoin::Address(publicKey3, 0x67);
  ASSERT_EQ(address3.string(), "idGY3z15RCN2fvEPkGDbEBduXpbJrJkCau"); 

  input.add_private_key(utxoKey3.data(), utxoKey3.size());
  auto utxo3 = input.add_utxo();
  auto utxoScript3 = Bitcoin::Script(parse_hex("76a91472afcd48cdc1cfcb34ace545182a35cf916664f788ac")); 
  utxo3->set_script(utxoScript3.bytes.data(), utxoScript3.bytes.size());
  utxo3->set_amount(10000000);
  //swap "abff4c09c0e0aba09e52255d66916483c5917bb92451a5d645651a0e34459758"
  auto hash3 = parse_hex("589745340e1a6545d6a55124b97b91c5836491665d25529ea0abe0c0094cffab"); 
  utxo3->mutable_out_point()->set_hash(hash3.data(), hash3.size());
  utxo3->mutable_out_point()->set_index(1);

  auto ts = TW::Iocoin::TransactionSigner<Iocoin::Transaction>(std::move(input), 1558693088);
  auto result = ts.sign();
  ASSERT_TRUE(result) << result.error();
  Data serialized;
  auto signedTx = result.payload();
  signedTx.encode(serialized);

  ASSERT_EQ(hex(serialized), "01000000e0c4e75c03c197b6ba826a74f9972c2302717faba945c5479f5f1366618e65cf2570f0f687010000006a47304402201c00c1fa1c2a428759a4348264581e415ecb419c314c9b335dde4811a336ba8a022051e1d175ac5b0eea7a99bf5dbafdc6bd5b57898e4554bbb1b25427fca49b480501210380f29ee6d85fd34f83c3808c537b24379260434f9235113cff28ca6c7063ca0effffffffaf1b8a68c0d51a27b0d85547efd4ef525571bbf7e2fe8f205b07f7c293eb05a6010000006a4730440220237eef66950bbcc75eae396490c27d5c293abcb0604c1bf2e4ff2b3bfc8cbcce02201fdd428601cf4955fd72df0c87172293c9cd568135ed9e77ad92bb34247a7c80012103579929d95d77d3ecbdd7868f3228bf464fe48de1e6a1df3445af360564cc1801ffffffff589745340e1a6545d6a55124b97b91c5836491665d25529ea0abe0c0094cffab010000006a47304402205b0af77cdea7c3dea8cdbbb9f187a08f4fdc61a23a446ed66ce5e91bfdd63234022009ec02ee975656e7026b6ac9e0640749b5baba4d86080fbf62e59802675d52df012102aa1373393c10dfd53e348d07698584ab75c3326be33ca1675328cca1bd524ed6ffffffff0240787d01000000001976a914ffab64a6f1551b738f6824421cdb7f59d3b6027088aca0c44a00000000001976a91486e3b0ec539000d443a673106a5144fc14cea13f88ac00000000");
}
