// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "TWBase.h"

TW_EXTERN_C_BEGIN

TW_EXPORT_ENUM(uint32_t)
enum TWBlockchain {
    TWBlockchainBitcoin = 0,
    TWBlockchainEthereum = 1,
    TWBlockchainVechain = 3,
    TWBlockchainTron = 4,
    TWBlockchainIcon = 5,
    TWBlockchainBinance = 6,
    TWBlockchainRipple = 7,
    TWBlockchainTezos = 8,
    TWBlockchainNimiq = 9,
    TWBlockchainStellar = 10,
    TWBlockchainAion = 11,
    TWBlockchainCosmos = 12,
    TWBlockchainTheta = 13,
    TWBlockchainOntology = 14,
    TWBlockchainZilliqa = 15,
    TWBlockchainIoTeX = 16,
    TWBlockchainEOS = 17,
    TWBlockchainNano = 18,
    TWBlockchainNULS = 19,
    TWBlockchainWaves = 20,
    TWBlockchainAeternity = 21,
    TWBlockchainNebulas = 22,
    TWBlockchainFIO = 23,
    TWBlockchainSolana = 24,
    TWBlockchainHarmony = 25,
    TWBlockchainNEAR = 26,
    TWBlockchainAlgorand = 27,
    TWBlockchainPolkadot = 29,
    TWBlockchainCardano = 30,
    TWBlockchainNEO = 31,
    TWBlockchainFilecoin = 32,
    TWBlockchainElrondNetwork = 33,
    TWBlockchainOasisNetwork = 34,
    TWBlockchainAvalanche = 35,
};

TW_EXTERN_C_END
