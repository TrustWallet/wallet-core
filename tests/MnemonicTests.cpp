// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Mnemonic.h"

#include <gtest/gtest.h>

namespace TW {

std::vector<std::string> ValidInput = {
    "ripple scissors kick mammal hire column oak again sun offer wealth tomorrow wagon turn fatal",
    // 12
    "credit expect life fade cover suit response wash pear what skull force", 
    // 15
    "rebuild park fatigue flame one clap grocery scheme upon symbol rifle flush brave feed clutch",
    // 18
    "find view amazing inject mistake school zone ticket deposit edit deer fuel expect pioneer alpha mirror joke private",
    // 21
    "tiger parent future endorse chuckle crazy seat tomato orient prevent swarm nerve duty crazy chief cruel purity team happy strategy level",
    // 24
    "admit smart swim bulk empty mystery state lyrics wrap welcome install seat supreme sunny sting roof once accuse envelope uncover arrive twice spoon squeeze", 
};

std::vector<std::string> InvalidInput = {
    // invalid word
    "ripple scissors hisc mammal hire column oak again sun offer wealth tomorrow",
    // invalid word
    "high culture ostrich wrist exist ignore interest hybridous exclude width more",
    // invalid checksum
    "ripple scissors kick mammal hire column oak again sun offer wealth tomorrow",
    // invalid word count
    "credit expect life fade cover suit response wash what skull force",
    // extra space
    "  credit expect life fade   cover suit response wash pear what skull force    ",
    // upper
    "CREDIT expect life fade cover suit response wash pear what skull force",
    // back is invalid word
    "ripple scissors kick mammal hire column oak again sun offer wealth tomorrow wagon turn back",
};

TEST(Mnemonic, isValid) {
    for (auto m: ValidInput) {
        EXPECT_TRUE(Mnemonic::isValid(m)) << "Expected valid " << m;
    }
}

TEST(Mnemonic, invalid) {
    for (auto m: InvalidInput) {
        EXPECT_FALSE(Mnemonic::isValid(m))  << "Expected invalid " << m;
    }
}

TEST(Mnemonic, isValidWord) {
    EXPECT_TRUE(Mnemonic::isValidWord("credit"));
    EXPECT_TRUE(Mnemonic::isValidWord("airport"));
    EXPECT_TRUE(Mnemonic::isValidWord("robot"));

    EXPECT_FALSE(Mnemonic::isValidWord("hybridous"));
    EXPECT_FALSE(Mnemonic::isValidWord("CREDIT"));
    EXPECT_FALSE(Mnemonic::isValidWord("credit  "));
    EXPECT_FALSE(Mnemonic::isValidWord("back"));
}

TEST(Mnemonic, suggest) {
    EXPECT_EQ(Mnemonic::suggest("air"), "air airport");
    EXPECT_EQ(Mnemonic::suggest("AIR"), "air airport");
    EXPECT_EQ(Mnemonic::suggest("abc"), "");
    EXPECT_EQ(Mnemonic::suggest("ai"), "aim air airport aisle");
    EXPECT_EQ(Mnemonic::suggest("an"), "analyst anchor ancient anger angle angry animal ankle announce annual");
    EXPECT_EQ(Mnemonic::suggest("a"), "abandon ability able about above absent absorb abstract absurd abuse");
    EXPECT_EQ(Mnemonic::suggest("str"), "strategy street strike strong struggle");
    EXPECT_EQ(Mnemonic::suggest("rob"), "robot robust");
    EXPECT_EQ(Mnemonic::suggest("saus"), "sausage");
    EXPECT_EQ(Mnemonic::suggest("saos"), "");
    EXPECT_EQ(Mnemonic::suggest(""), "");
    EXPECT_EQ(Mnemonic::suggest("3"), "");
    EXPECT_EQ(Mnemonic::suggest(" a"), "");
    EXPECT_EQ(Mnemonic::suggest(" "), "");
    EXPECT_EQ(Mnemonic::suggest("f"), "fabric face faculty fade faint faith fall false fame family");
    EXPECT_EQ(Mnemonic::suggest("fa"), "fabric face faculty fade faint faith fall false fame family");
    EXPECT_EQ(Mnemonic::suggest("fam"), "fame family famous");
    EXPECT_EQ(Mnemonic::suggest("fami"), "family");
    EXPECT_EQ(Mnemonic::suggest("famil"), "family");
    EXPECT_EQ(Mnemonic::suggest("family"), "family");
    EXPECT_EQ(Mnemonic::suggest("p"), "pact paddle page pair palace palm panda panel panic panther");
    EXPECT_EQ(Mnemonic::suggest("pr"), "practice praise predict prefer prepare present pretty prevent price pride");
    EXPECT_EQ(Mnemonic::suggest("pro"), "problem process produce profit program project promote proof property prosper");
    EXPECT_EQ(Mnemonic::suggest("prog"), "program");
    EXPECT_EQ(Mnemonic::suggest("progr"), "program");
    EXPECT_EQ(Mnemonic::suggest("progra"), "program");
    EXPECT_EQ(Mnemonic::suggest("program"), "program");
}

} // namespace
