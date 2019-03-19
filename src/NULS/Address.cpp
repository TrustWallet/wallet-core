// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.
#include "Address.h"

#include <TrezorCrypto/base58.h>
#include <TrezorCrypto/ecdsa.h>
#include <HexCoding.h>

using namespace TW::NULS;

bool Address::isValid(const std::string &string) {
  if (string.empty()) {
    return false;
  }

  size_t capacity = 24;
  uint8_t result[capacity];
  if (!b58tobin(result, &capacity, string.data())) {
    return false;
  }

  if (capacity != Address::addressSize) {
    return false;
  }

  uint8_t address[24] = {0};
  memcpy(address, result, 24);

  // Check Xor

  uint8_t checkSum = 0x00;
  for (int i = 0; i < 23; ++i) {
    checkSum ^= address[i];
  }

  return address[23] == checkSum;
}

Address::Address(const std::string &string) {
  if (!isValid(string)) {
    return;
  }

  size_t capacity = Address::addressSize;
  uint8_t result[capacity];
  bool success = b58tobin(result, &capacity, string.data());
  if (success) {
    std::copy(result, result + Address::addressSize, bytes.begin());
  }
}

Address::Address(const TW::PublicKey &publicKey) {
  // Main-Net chainID
  bytes[0] = 0x04;
  bytes[1] = 0x23;
  // Address Type
  bytes[2] = 0x01;

  ecdsa_get_pubkeyhash(publicKey.bytes.data(), HASHER_SHA2_RIPEMD, bytes.begin() + 3);

  // Calc chechsum
  uint8_t checkSum = 0x00;
  for (int i = 0; i < 23; ++i) {
    checkSum ^= bytes[i];
  }
  bytes[23] = checkSum;
}

Address::Address(const std::vector<uint8_t> &data) {
  if (data.size() != Address::addressSize) {
    throw std::invalid_argument("Invalid address data");
  }

  std::copy(data.begin(), data.end(), bytes.begin());
}

std::string Address::string() const {
  size_t size = 0;
  b58enc(nullptr, &size, bytes.data(), Address::addressSize);

  std::string str(size, ' ');
  size_t res = size;
  bool success = b58enc(&str[0], &res, bytes.data(), Address::addressSize);
  if (success) {
    str.resize(size - 1);
    return str;
  }
  return std::string();
}

uint16_t Address::chainID() const {
  return *(uint16_t *) (&bytes);
}

uint8_t Address::type() const {
  return bytes[2];
}

bool Address::isValid() const {
  if (this->chainID() != 8964) {
    return false;
  }
  if (this->type() != 0x01 && this->type() != 0x02) {
    return false;
  }

  uint8_t checkSum = 0x00;
  for (int i = 0; i < 23; ++i) {
    checkSum ^= bytes[i];
  }

  return bytes[23] == checkSum;
}

TW::PrivateKey Address::importHexPrivateKey(std::string hexPrivateKey) {
  Data privKey = parse_hex(hexPrivateKey);
  Data data = Data();
  switch (privKey.size()) {
  case 31: {
    data.push_back(static_cast<uint8_t>(0x00));
    std::copy(privKey.begin(), privKey.end(), std::back_inserter(data));
  }
    break;
  case 32: {
    std::copy(privKey.begin(), privKey.end(), std::back_inserter(data));
  }
    break;
  case 33: {
    std::copy(privKey.begin() + 1, privKey.end(), std::back_inserter(data));
  }
    break;
  default: {
    throw std::invalid_argument("Invalid private key");
  }
  }

  auto key = PrivateKey(data);
  return key;
}
