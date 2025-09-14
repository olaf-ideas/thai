#include <gtest/gtest.h>
#include "core/thai_poker.hpp"
using namespace thai_poker;

TEST(ThaiPoker, EnumShape) {
    EXPECT_EQ(static_cast<int>(Bet::CHECK), BET_NB);
}

TEST(ThaiPoker, SatisfiesBet) {
    Hand hand = 
        (1U << make_card(Suit::SUIT_C, Rank::RANK_9)) |
        (1U << make_card(Suit::SUIT_D, Rank::RANK_9));

    EXPECT_TRUE(
        satisfies_bet(
            hand,
            Bet::PAIR_9
        )
    );

    EXPECT_FALSE(
        satisfies_bet(
            hand,
            Bet::THREE_9
        )
    );
}