// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Signer.h"
#include "Address.h"
#include "../Cbor.h"
#include "../Data.h"
#include "../HexCoding.h"
#include "../PrivateKey.h"
#include "../PublicKey.h"

#include <cmath>
#include <cassert>

using namespace TW;
using namespace TW::Cardano;
using namespace TW::Cbor;
using namespace std;


Proto::TransactionPlan Signer::planTransaction(const Proto::SigningInput& input) noexcept {
    try {
        Proto::TransactionPlan plan = planTransactionNoFee(input);
        return plan;
    } catch (exception& ex) {
        // return empty plan
        Proto::TransactionPlan plan;
        plan.set_error(ex.what());
        return plan;
    }
}

Proto::SigningOutput Signer::sign(const Proto::SigningInput& input, const Proto::TransactionPlan& plan) noexcept {
    try {
        // check plan
        checkPlan(plan);
        assert(plan.fee() != 0);

        // prepare first part of tx data
        Data unisgnedEncodedCborData = prepareUnsignedTx(input, plan);
        // compute txId, sign, complete with second half of the tx data
        Proto::SigningOutput output = prepareSignedTx(input, plan, unisgnedEncodedCborData);
        return output;
    } catch (exception& ex) {
        // return empty output with error
        Proto::SigningOutput output;
        output.set_error(ex.what());
        return output;
    }
}

Proto::TransactionPlan Signer::planTransactionNoFee(const Proto::SigningInput& input) {
    // To compute fee:
    // - build plan with minimal fee
    // - prepare tx, take size
    // - estimate fee (based on size)
    // - build plan with fee

    // build plan with minimal fee
    Proto::TransactionPlan planPre = planTransactionWithFee(input, MinimalFee);
    // prepare first part of tx data
    Data unisgnedEncodedCborDataPre = prepareUnsignedTx(input, planPre);
    // compute txId, sign, complete with second half of the tx data
    auto outputPre = prepareSignedTx(input, planPre, unisgnedEncodedCborDataPre);
    // compute fee from estimated tx size and linear equation
    auto txSize = outputPre.encoded().length();
    uint64_t fee = (uint64_t)std::ceil((double)FeeLinearCoeffA + FeeLinearCoeffB * (double)txSize);
    assert(fee != 0);

    // build final plan with computed fee
    Proto::TransactionPlan plan = planTransactionWithFee(input, fee);
    return plan;
}

void Signer::checkPlan(const Proto::TransactionPlan& plan) {
    if (plan.fee() == 0) {
        throw logic_error("Zero fee is invalid");
    }
    uint64_t sum_utxo = 0;
    for (int i = 0; i < plan.utxo_size(); ++i) { sum_utxo += plan.utxo(i).amount(); }
    if (plan.amount() + plan.fee() + plan.change() != sum_utxo) {
        // amount mismatch
        throw logic_error("Amount mismatch");
    }
    assert(plan.fee() != 0);
    assert(plan.utxo_size() > 0);
}

Proto::TransactionPlan Signer::planTransactionWithFee(const Proto::SigningInput& input, uint64_t fee) {
    Proto::TransactionPlan plan;

    // compute amounts
    uint64_t sumAllUtxo = 0;
    for (int i = 0; i < input.utxo_size(); ++i) {
        sumAllUtxo += input.utxo(i).amount();
    }
    plan.set_available_amount(sumAllUtxo);
    uint64_t amount = input.amount();
    // compute change, check if enough
    if ((amount + fee) > sumAllUtxo) {
        throw logic_error("Insufficent balance");
    }
    plan.set_amount(amount);
    plan.set_fee(fee);

    // select UTXOs
    // TODO do it in non-default order
    uint64_t sumSelected = 0;
    for (int i = 0; i < input.utxo_size(); ++i) {
        auto utxo = plan.add_utxo();
        utxo->mutable_out_point()->set_txid(input.utxo(i).out_point().txid());
        utxo->mutable_out_point()->set_index(input.utxo(i).out_point().index());
        utxo->set_amount(input.utxo(i).amount());
        sumSelected += input.utxo(i).amount();
        if (sumSelected >= (amount + fee)) {
            // enough
            break;
        }
    }
    assert(sumSelected >= (amount + fee));
    uint64_t changeAmount = sumSelected - (amount + fee);
    plan.set_change(changeAmount);
    assert(plan.amount() + plan.fee() + plan.change() == plan.available_amount());

    return plan;
}

Data Signer::prepareUnsignedTx(const Proto::SigningInput& input, const Proto::TransactionPlan& plan) {
    assert(plan.fee() != 0);
    // inputs from plan.utxo
    auto inputsArray = Encode::indefArray();
    for (int i = 0; i < plan.utxo_size(); ++i) {
        Data outPointData = Encode::array({
            Encode::bytes(TW::data(plan.utxo(i).out_point().txid())),
            Encode::uint(plan.utxo(i).out_point().index()),
        }).encoded();
        inputsArray.addIndefArrayElem(
            Encode::array({
                Encode::uint(0), // type
                Encode::tag(Address::PayloadTag, Encode::bytes(outPointData))
            })
        );
    }
    inputsArray.closeIndefArray();

    // outputs array
    auto outputsArray = Encode::indefArray();
    Address toAddr = Address(input.to_address());
    outputsArray.addIndefArrayElem(
        Encode::array({
            Encode::fromRaw(toAddr.getCborData()),
            Encode::uint(plan.amount()),
        })
    );
    if (plan.change() != 0) {
        Address changeAddr = Address(input.change_address());
        outputsArray.addIndefArrayElem(
            Encode::array({
                Encode::fromRaw(changeAddr.getCborData()),
                Encode::uint(plan.change()),
            })
        );
    }
    outputsArray.closeIndefArray();

    Data enc = Encode::array({
        inputsArray,
        outputsArray,
        // attributes
        Encode::map({})
    }).encoded();
    return enc;
}

Proto::SigningOutput Signer::prepareSignedTx(const Proto::SigningInput& input, const Proto::TransactionPlan& plan, const Data& unisgnedEncodedCborData) {
    Data txId = Hash::blake2b(unisgnedEncodedCborData, 32);

    // private key per input UTXO is needed
    if (input.private_key_size() < plan.utxo_size()) {
        throw logic_error("Not enough private keys");
    }
    // array with signatures
    vector<Encode> signatures;
    for (int i = 0; i < input.private_key_size(); ++i) {
        PrivateKey fromPri = PrivateKey(input.private_key(i));
        PublicKey fromPub = fromPri.getPublicKey(TWPublicKeyTypeED25519Extended);
        // sign; msg is txId with prefix
        Data txToSign = parse_hex("01"); // transaction prefix
        TW::append(txToSign, Encode::uint(Network_Mainnet_Protocol_Magic).encoded());
        TW::append(txToSign, Encode::bytes(txId).encoded());
        Data signature = fromPri.sign(txToSign, TWCurveED25519Extended);
        Data signatureCbor = Encode::array({
            Encode::bytes(fromPub.bytes),
            Encode::bytes(signature),
        }).encoded();
        signatures.push_back(
            Encode::array({
                Encode::uint(0), // type
                Encode::tag(Address::PayloadTag,
                    Encode::bytes(signatureCbor)
                ),
            })
        );
    }

    Data encoded = Encode::array({
        Encode::fromRaw(unisgnedEncodedCborData),
        Encode::array(signatures),
    }).encoded();

    Proto::SigningOutput output;
    output.set_encoded(encoded.data(), encoded.size());
    output.set_transaction_id(hex(txId));
    output.set_fee(plan.fee());
    return output;
}