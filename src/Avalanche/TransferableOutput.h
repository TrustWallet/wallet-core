// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../Data.h"
#include "Address.h"
#include <vector>

namespace TW::Avalanche {

enum TransactionOutputTypeID {
  SECPTransferOut = 7,
  SECPMint = 6,
  NFTTransferOut = 11,
  NFTMintOut = 10
};

class TransactionOutput {
  public:
    /// Encodes the output into the provided buffer.
    virtual void encode(Data& data) const = 0;
    bool operator<(const TransactionOutput& other) const;
    virtual ~TransactionOutput(){}
    virtual std::unique_ptr<TransactionOutput> duplicate() = 0; 
  protected:
    TransactionOutput(){}
};

/// Avalanche transaction output.
class TransferableOutput {
  public:
    Data AssetID;
    std::unique_ptr<TransactionOutput> Output;

    /// Encodes the output into the provided buffer.
    void encode(Data& data) const;

    TransferableOutput(const Data& assetID, std::unique_ptr<TransactionOutput> output)
      : AssetID(assetID), Output(std::move(output)) {}

    TransferableOutput(const TransferableOutput& other) {
      AssetID = other.AssetID;
      Output = other.Output->duplicate();
    }
    
    TransferableOutput& operator=(const TransferableOutput& other);

    bool operator<(const TransferableOutput& other) const;
};


class SECP256k1TransferOutput : public TransactionOutput {
  const uint32_t typeID = TransactionOutputTypeID::SECPTransferOut;
  
  public:
    uint64_t Amount;
    uint64_t Locktime;
    uint32_t Threshold;
    std::vector<Address> Addresses;

    SECP256k1TransferOutput(uint64_t amount, uint64_t locktime, uint32_t threshold, std::vector<Address> &addresses)
      : Amount(amount), 
      Locktime(locktime), Threshold(threshold), Addresses(addresses) {
        std::sort(Addresses.begin(), Addresses.end());
      }
  
    void encode (Data& data) const;

    std::unique_ptr<TransactionOutput> duplicate() {
      auto dup = std::make_unique<SECP256k1TransferOutput>(Amount, Locktime, Threshold, Addresses);
      return dup;
    }
};


class SECP256k1MintOutput : public TransactionOutput {
  const uint32_t typeID = TransactionOutputTypeID::SECPMint;
  
  public:
    uint64_t Locktime;
    uint32_t Threshold;
    std::vector<Address> Addresses;

    SECP256k1MintOutput(uint64_t locktime, uint32_t threshold, std::vector<Address> &addresses)
      : Locktime(locktime), Threshold(threshold), Addresses(addresses) {
        std::sort(Addresses.begin(), Addresses.end());
      }
  
    void encode (Data& data) const;

    std::unique_ptr<TransactionOutput> duplicate() {
      auto dup = std::make_unique<SECP256k1MintOutput>(Locktime, Threshold, Addresses);
      return dup;
    }
};

class NFTTransferOutput : public TransactionOutput {
  const uint32_t typeID = TransactionOutputTypeID::NFTTransferOut;
  
  public:
    uint32_t GroupID;
    Data Payload;
    uint64_t Locktime;
    uint32_t Threshold;
    std::vector<Address> Addresses;

    NFTTransferOutput(uint32_t groupID, Data &payload, uint64_t locktime, uint32_t threshold, std::vector<Address> &addresses)
      : GroupID(groupID), Payload(payload),
      Locktime(locktime), Threshold(threshold), Addresses(addresses) {
        std::sort(Addresses.begin(), Addresses.end());
      }
  
    void encode (Data& data) const;

    std::unique_ptr<TransactionOutput> duplicate() {
      auto dup = std::make_unique<NFTTransferOutput>(GroupID, Payload, Locktime, Threshold, Addresses);
      return dup;
    }
};

class NFTMintOutput : public TransactionOutput {
  const uint32_t typeID = TransactionOutputTypeID::NFTMintOut;
  
  public:
    uint32_t GroupID;
    uint64_t Locktime;
    uint32_t Threshold;
    std::vector<Address> Addresses;

    NFTMintOutput(uint32_t groupID, uint64_t locktime, uint32_t threshold, std::vector<Address> &addresses)
      : GroupID(groupID),
      Locktime(locktime), Threshold(threshold), Addresses(addresses) {
        std::sort(Addresses.begin(), Addresses.end());
      }
  
    void encode (Data& data) const;

    std::unique_ptr<TransactionOutput> duplicate() {
      auto dup = std::make_unique<NFTMintOutput>(GroupID, Locktime, Threshold, Addresses);
      return dup;
    }
};

} // namespace TW::Avalanche
