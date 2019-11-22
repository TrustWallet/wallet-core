
// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Coins.h"

#include "WalletConsole.h"
#include <TrustWalletCore/TWCoinTypeConfiguration.h>
#include <TrustWalletCore/TWPublicKeyType.h>
#include "Data.h"

#include <iostream>
#include <vector>
#include <cassert>

#define WRAPS(x) std::shared_ptr<TWString>(x, TWStringDelete)

namespace TW::WalletConsole {

using namespace std;

void Coins::coins() const {
    for (auto c: _coinsById) {
        cout << c.second.symbol << " \t " << c.second.id << " \t '" << c.second.name << "'" << endl;
    }
    cout << _coinsById.size() << " coins listed." << endl;
}

/*
bool Coins::coin(const string& coin) const {
    Coin c;
    if (!findCoin(coin, c)) {
        // not found
        return false;
    }
    cout << "Coin id: " << c.id << "  name: '" << c.name << "'  symbol: " << c.symbol << "  numericalid: " << c.c << endl;
    return false;
}
*/

int Coins::findCoinId(const string& coin) const {
    if (_coinsById.find(coin) != _coinsById.end()) {
        // Note: simple map[key] does not work for const...
        return _coinsById.find(coin)->second.c;
    }
    if (_coinsByName.find(coin) != _coinsByName.end()) {
        return _coinsByName.find(coin)->second.c;
    }
    if (_coinsBySymbol.find(coin) != _coinsBySymbol.end()) {
        return _coinsBySymbol.find(coin)->second.c;
    }
    // not found
    return -1;
}

bool Coins::findCoin(const string& coin, Coin& coin_out) const {
    int c = findCoinId(coin);
    if (c < 0) {
        cout << "Error: No such coin '" << coin << "'" << endl;
        return false;
    }
    coin_out = _coinsByNum.find(c)->second;
    return true;
}

void Coins::init() {
    // not very nice method: try each ID number, and record the ones that are valid coins
    cout << "Loading coins ... ";
    scanCoinRange(0, 65536);
    scanCoinRange(5700000, 5800000);
    cout << _coinsById.size() << " coins loaded." << endl;
}

void Coins::scanCoinRange(int from, int to) {
    for (int i = from; i < to; ++i) {
        TWCoinType c = (TWCoinType)i;
        auto symbolTw = WRAPS(TWCoinTypeConfigurationGetSymbol(c));
        if (TWStringSize(symbolTw.get()) == 0) { continue; }
        string id = TWStringUTF8Bytes(WRAPS(TWCoinTypeConfigurationGetID(c)).get());
        WalletConsole::toLower(id);
        string symbol = TWStringUTF8Bytes(symbolTw.get());
        WalletConsole::toLower(symbol);
        string name = TWStringUTF8Bytes(WRAPS(TWCoinTypeConfigurationGetName(c)).get());
        WalletConsole::toLower(name);
        int curve = (int)TWCoinTypeCurve(c);
        int pubKeyType = pubKeyTypeFromCurve(curve);
        Coin coin = Coin{c, id, name, symbol, curve, pubKeyType};
        _coinsByNum[c] = coin;
        _coinsById[id] = coin;
        _coinsByName[name] = coin;
        _coinsBySymbol[symbol] = coin;
    }
}

int Coins::pubKeyTypeFromCurve(int cc) {
    TWCurve c = (TWCurve)cc;
    TWPublicKeyType t;
    switch (c) {
        case TWCurveSECP256k1: t = TWPublicKeyTypeSECP256k1; break;
        case TWCurveED25519: t = TWPublicKeyTypeED25519; break;
        case TWCurveED25519Blake2bNano: t = TWPublicKeyTypeED25519Blake2b; break;
        case TWCurveCurve25519: t = TWPublicKeyTypeCURVE25519; break;
        case TWCurveNIST256p1: t = TWPublicKeyTypeNIST256p1; break;
    }
    return (int)t;
}

} // namespace TW::WalletConsole
