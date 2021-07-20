// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "../Bech32.h"

#include "Address.h"

#include <TrustWalletCore/TWHRP.h>

using namespace TW::Avalanche;

const std::string Address::hrp = HRP_AVALANCHEX;
const size_t Address::hashLen = 20;

bool Address::isValid(const std::string& string) {
    // split into before and after - 
    auto hyphenPos = string.find("-");
    if (hyphenPos == std::string::npos) {
        return false;
    }
    if (hyphenPos == 0) {
        return false;
    }
    auto chainID = string.substr(hyphenPos - 1, 1);
    if ((chainID != "X") && (chainID != "P")) {
        // implementation is currently X-chain only, but a passed address starting with P- follows the same rules
        return false;
    }
    auto afterHyphen = string.substr(hyphenPos + 1);
   return Bech32Address::isValid(afterHyphen, hrp);
}

void Address::extractKeyHashFromString (const std::string& string) {
    if (!isValid(string)) {
        throw std::invalid_argument("Invalid address string");
    }
    auto hyphenPos = string.find("-");
    auto pad = true;
    Bech32Address::decode(string.substr(hyphenPos + 1), *this, hrp, pad);
}

std::string Address::string() const {
    // for now, we only support x-chain addresses
    return "X-" + Bech32Address::string();
}
