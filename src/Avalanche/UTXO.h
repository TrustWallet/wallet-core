// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "TransferableOutput.h"

namespace TW::Avalanche {

    class UTXO {
      public: 
        const uint16_t CodecID = 0;
        Data TxID;
        uint32_t UTXOIndex;
        Data AssetID;
        TransferableOutput Output;
        
        UTXO(const Data& txID, uint32_t UTXOIndex, const Data& assetID, const TransferableOutput& output)
        : TxID(txID), UTXOIndex(UTXOIndex), AssetID(assetID), Output(output) {}

        /// Encodes the UTXO into the provided buffer.
        void encode(Data& data) const;
    };

} // namespace TW::Avalanche
