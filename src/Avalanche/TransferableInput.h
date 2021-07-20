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

enum TransactionInputTypeID {
  SECPInput = 5
};

class TransactionInput{
  public: 
    virtual std::vector<uint32_t> getAddressIndices() const = 0;
    virtual uint32_t getTypeID() const = 0;
    virtual void encode (Data& data) const = 0; //we want to enforce that all subclasses can encode
    virtual ~TransactionInput() {}
    virtual std::unique_ptr<TransactionInput> duplicate() = 0;
  protected:
    TransactionInput() {}
};

/// Avalanche transaction input.
class TransferableInput {
  public:
    Data TxID;
    uint32_t UTXOIndex;
    Data AssetID;
    std::unique_ptr<TransactionInput> Input;
    std::vector<Address> SpendableAddresses; // corresponding to the Output this came from. not encoded

    /// Encodes the input into the provided buffer.
    void encode(Data& data) const;

    TransferableInput(Data &txid, uint32_t utxoIndex, Data &assetID, std::unique_ptr<TransactionInput> input, std::vector<Address> &spendableAddresses)
        : TxID(txid) , UTXOIndex(utxoIndex), AssetID(assetID), Input(std::move(input)), SpendableAddresses(spendableAddresses)
         {
           std::sort(SpendableAddresses.begin(), SpendableAddresses.end());
         }

    TransferableInput(const TransferableInput& other) {
      TxID = other.TxID;
      UTXOIndex = other.UTXOIndex;
      AssetID = other.AssetID;
      Input = other.Input->duplicate();
      SpendableAddresses = other.SpendableAddresses;
    }

    bool operator<(const TransferableInput& other) const;

    TransferableInput& operator=(const TransferableInput& other);
};


class SECP256k1TransferInput : public TransactionInput {
    uint32_t TypeID = TransactionInputTypeID::SECPInput; 
    uint64_t Amount;
    std::vector<uint32_t> AddressIndices;
  
  public:

    SECP256k1TransferInput(uint64_t amount, std::vector<uint32_t> addressIndices)
      : Amount(amount) {
        AddressIndices = addressIndices;
        std::sort(AddressIndices.begin(), AddressIndices.end());
      }
  
    void encode (Data& data) const;

    std::vector<uint32_t> getAddressIndices() const {return AddressIndices;}
    uint32_t getTypeID() const {return TypeID;}

    std::unique_ptr<TransactionInput> duplicate() {
      auto dup = std::make_unique<SECP256k1TransferInput>(Amount, AddressIndices);
      return dup;
    }
};

} // namespace TW::Avalanche
