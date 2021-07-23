// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include <string>

namespace TW {

/// BIP39 Mnemonic Sentence handling.
class Mnemonic {
public:
    static constexpr size_t MaxWords = 24;
    static constexpr size_t MinWords = 12;

public:
    /// Determines whether a mnemonic phrase is valid.
    // E.g. for a valid mnemonic: "credit expect life fade cover suit response wash pear what skull force"
    static bool isValid(const std::string& mnemonic);

    /// Determines whether word is a valid menemonic word.
    static bool isValidWord(const std::string& word);

    /// Return BIP39 English words that match the given prefix.
    // - A single string is returned, with space-separated list of words (or single word or empty string)
    //   (Why not array?  To simplify the cross-language interfaces)
    // - A maximum of 10 words are returned
    // - Only exact prefix match, saus returns sausage, but saos does not.
    // - Only word start match, rob returns robot, but not aerobic
    // - Prefix is case insensitive
    // - Returned words are in lowercase, and in alphabetical order
    // Examples:
    // - 'rob' -> 'robot robust'
    // - 'ai' -> 'aim air airport aisle'
    // - 'an' -> 'analyst anchor ancient anger angle angry animal ankle announce annual'
    // - 'a'-> 'abandon ability able about above absent absorb abstract absurd abuse'
    static std::string suggest(const std::string& prefix);

    static const int SuggestMaxCount;
};

} // namespace TW

/// Wrapper for C interface.
struct TWMnemonic {
    TW::Mnemonic impl;
};
