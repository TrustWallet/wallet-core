// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "TransactionBuilder.h"
#include "TransactionSigner.h"

#include "../Coin.h"
#include "../proto/Bitcoin.pb.h"

#include <algorithm>
#include <cassert>

namespace TW::Bitcoin {

/// Estimate encoded size by simple formula
int64_t estimateSimpleFee(const FeeCalculator& feeCalculator, const TransactionPlan& plan, int outputSize, int64_t byteFee) {
    return feeCalculator.calculate(plan.utxos.size(), outputSize, byteFee);
}

/// Estimate encoded size by invoking sign(sizeOnly), get actual size
int64_t estimateSegwitFee(const FeeCalculator& feeCalculator, const TransactionPlan& plan, int outputSize, const Bitcoin::Proto::SigningInput& input) {
    TWPurpose coinPurpose = TW::purpose(static_cast<TWCoinType>(input.coin_type()));
    if (coinPurpose != TWPurposeBIP84) {
        // not segwit, return default simple estimate
        return estimateSimpleFee(feeCalculator, plan, outputSize, input.byte_fee());
    }

    // duplicate input, with the current plan
    auto inputWithPlan = std::move(input);
    *inputWithPlan.mutable_plan() = plan.proto();

    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(inputWithPlan), true);
    auto result = signer.sign();
    if (!result) {
        // signing failed; return default simple estimate
        return estimateSimpleFee(feeCalculator, plan, outputSize, input.byte_fee());
    }

    // Obtain the encoded size
    auto transaction = result.payload();
    Data dataNonSegwit;
    transaction.encode(dataNonSegwit, Transaction::SegwitFormatMode::NonSegwit);
    int64_t sizeNonSegwit = dataNonSegwit.size();
    uint64_t vSize = 0;
    // Check if there is segwit
    if (!transaction.hasWitness()) {
        // no segwit, virtual size is defined as non-segwit size
        vSize = sizeNonSegwit;
    } else {
        Data dataWitness;
        transaction.encodeWitness(dataWitness);
        int64_t witnessSize = 2 + dataWitness.size();
        // compute virtual size:  (smaller) non-segwit + 1/4 of the diff (witness-only)
        // (in other way: 3/4 of (smaller) non-segwit + 1/4 of segwit size)
        vSize = sizeNonSegwit + witnessSize/4 + (witnessSize % 4 != 0);
    }
    uint64_t fee = input.byte_fee() * vSize;

    return fee;
}

int extraOutputCount(const Bitcoin::Proto::SigningInput& input) {
    int count = int(input.output_op_return().length() > 0);
    return count;
}

TransactionPlan TransactionBuilder::plan(const Bitcoin::Proto::SigningInput& input) {
    const auto& feeCalculator = getFeeCalculator(static_cast<TWCoinType>(input.coin_type()));
    auto unspentSelector = UnspentSelector(feeCalculator);
    bool maxAmount = input.use_max_amount();

    auto plan = TransactionPlan();
    if (input.output_op_return().length() > 0) {
        plan.outputOpReturn = data(input.output_op_return());
    }

    if (input.amount() == 0 && !maxAmount) {
        plan.error = Common::Proto::Error_zero_amount_requested;
    } else if (input.utxo().empty()) {
        plan.error = Common::Proto::Error_missing_input_utxos;
    } else {
        // select UTXOs
        plan.amount = input.amount();

        // if amount requested is the same or more than available amount, it cannot be satisifed, but
        // treat this case as MaxAmount, and send maximum available (which will be less)
        if (!maxAmount && input.amount() >= UnspentSelector::sum(input.utxo())) {
            maxAmount = true;
        }

        auto extraOutputs = extraOutputCount(input);
        auto output_size = 2 + extraOutputs;
        if (!maxAmount) {
            output_size = 2 + extraOutputs; // output + change
            plan.utxos = unspentSelector.select(input.utxo(), plan.amount, input.byte_fee(), output_size);
        } else {
            output_size = 1 + extraOutputs; // output, no change
            plan.utxos = unspentSelector.selectMaxAmount(input.utxo(), input.byte_fee());
        }

        if (plan.utxos.size() == 0) {
            plan.amount = 0;
            plan.error = Common::Proto::Error_not_enough_utxos;
        } else {
            plan.availableAmount = UnspentSelector::sum(plan.utxos);

            // Compute fee.
            // must preliminary set change so that there is a second output
            if (!maxAmount) {
                assert(input.amount() <= plan.availableAmount);
                plan.amount = input.amount();
                plan.fee = 0;
                plan.change = plan.availableAmount - plan.amount;
            } else {
                plan.amount = plan.availableAmount;
                plan.fee = 0;
                plan.change = 0;
            }
            plan.fee = estimateSegwitFee(feeCalculator, plan, output_size, input);
            // If fee is larger then availableAmount (can happen in special maxAmount case), we reduce it (and hope it will go through)
            plan.fee = std::min(plan.availableAmount, plan.fee);
            assert(plan.fee >= 0 && plan.fee <= plan.availableAmount);

            // adjust/compute amount
            if (!maxAmount) {
                // reduce amount if needed
                plan.amount = std::max(Amount(0), std::min(plan.amount, plan.availableAmount - plan.fee));
            } else {
                // max available amount
                plan.amount = std::max(Amount(0), plan.availableAmount - plan.fee);
            }
            assert(plan.amount >= 0 && plan.amount <= plan.availableAmount);

            // compute change
            plan.change = plan.availableAmount - plan.amount - plan.fee;
        }
    }
    assert(plan.change >= 0 && plan.change <= plan.availableAmount);
    assert(!maxAmount || plan.change == 0); // change is 0 in max amount case

    assert(plan.amount + plan.change + plan.fee == plan.availableAmount);

    return plan;
}

} // namespace TW::Bitcoin
