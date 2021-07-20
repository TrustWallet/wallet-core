// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "TransferableOutput.h"

namespace TW::Avalanche {

    class InitialState {
      public: 
        enum FeatureExtension {SECP256k1 = 0, NFT = 1};
        FeatureExtension FxID;
        std::vector<std::unique_ptr<TransactionOutput>> Outputs;
        
        InitialState(FeatureExtension fxid, std::vector<std::unique_ptr<TransactionOutput>> outputs)
         : FxID(fxid), Outputs(std::move(outputs)) {
          std::sort(Outputs.begin(), Outputs.end(), [](const auto& lhs, const auto& rhs)
          {
            // just calling std::sort will sort based on the unique_ptrs, rather than using the underlying TransactionOutputs' operator<
            return *lhs < *rhs;
          });
         }

        InitialState(const InitialState &other) {
          FxID = other.FxID;
          Outputs.clear();
          for (auto &output : other.Outputs) {
            Outputs.push_back(output->duplicate());
          }
          std::sort(Outputs.begin(), Outputs.end(), [](const auto& lhs, const auto& rhs)
          {
            // just calling std::sort will sort based on the unique_ptrs, rather than using the underlying TransactionOutputs' operator<
            return *lhs < *rhs;
          });          
        }

        InitialState& operator=(const InitialState &other);

        /// Encodes the InitialState into the provided buffer.
        void encode(Data& data) const;

        bool operator<(const InitialState& other) const;
    };
} // namespace TW::Avalanche