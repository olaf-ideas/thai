#include <gtest/gtest.h>
#include "core/thai_poker.hpp"
using namespace thai_poker;

TEST(ThaiPoker, EnumShape) {
    EXPECT_EQ(static_cast<int>(Bet::CHECK), BET_NB);
}

TEST(ThaiPoker, High) {
    for (int suit = 0; suit < 4; suit++) {
        Suit s = static_cast<Suit>(suit);
        for (int rank = 0; rank < 6; rank++) {
            Rank r = static_cast<Rank>(rank);
            Bet bet = static_cast<Bet>(to_i(Bet::HIGH_9) + to_i(r));

            Hand hand = (1U << make_card(s, r));
            EXPECT_TRUE(satisfies_bet(hand, bet));
            EXPECT_FALSE(satisfies_bet(0, bet));
        }
    }
}

TEST(ThaiPoker, Pair) {
    for (int suit1 = 0; suit1 < 4; suit1++) {
        for (int suit2 = suit1 + 1; suit2 < 4; suit2++) {
            Suit s1 = static_cast<Suit>(suit1);
            Suit s2 = static_cast<Suit>(suit2);
            for (int rank = 0; rank < 6; rank++) {
                Rank r = static_cast<Rank>(rank);
                Bet bet = static_cast<Bet>(to_i(Bet::PAIR_9) + to_i(r));

                Hand hand = (1U << make_card(s1, r)) |
                            (1U << make_card(s2, r));
                EXPECT_TRUE(satisfies_bet(hand, bet));
                EXPECT_FALSE(satisfies_bet(0, bet));
            }
        }
    }
}

TEST(ThaiPoker, Straight) {
    EXPECT_FALSE(satisfies_bet(0, Bet::LOW_STRAIGHT));
    EXPECT_FALSE(satisfies_bet(0, Bet::HIGH_STRAIGHT));

    EXPECT_TRUE(
        satisfies_bet(
            (1U << make_card(Suit::SUIT_C, Rank::RANK_9)) |
            (1U << make_card(Suit::SUIT_D, Rank::RANK_T)) |
            (1U << make_card(Suit::SUIT_S, Rank::RANK_J)) |
            (1U << make_card(Suit::SUIT_D, Rank::RANK_Q)) |
            (1U << make_card(Suit::SUIT_C, Rank::RANK_K)),
        Bet::LOW_STRAIGHT
        )
    );

    EXPECT_TRUE(
        satisfies_bet(
            (1U << make_card(Suit::SUIT_C, Rank::RANK_T)) |
            (1U << make_card(Suit::SUIT_D, Rank::RANK_J)) |
            (1U << make_card(Suit::SUIT_S, Rank::RANK_Q)) |
            (1U << make_card(Suit::SUIT_D, Rank::RANK_K)) |
            (1U << make_card(Suit::SUIT_C, Rank::RANK_A)),
        Bet::HIGH_STRAIGHT
        )
    );

    EXPECT_FALSE(
        satisfies_bet(
            (1U << make_card(Suit::SUIT_C, Rank::RANK_9)) |
            (1U << make_card(Suit::SUIT_D, Rank::RANK_T)) |
            (1U << make_card(Suit::SUIT_S, Rank::RANK_J)) |
            (1U << make_card(Suit::SUIT_D, Rank::RANK_Q)) |
            (1U << make_card(Suit::SUIT_C, Rank::RANK_K)),
        Bet::HIGH_STRAIGHT
        )
    );

    EXPECT_FALSE(
        satisfies_bet(
            (1U << make_card(Suit::SUIT_C, Rank::RANK_T)) |
            (1U << make_card(Suit::SUIT_D, Rank::RANK_J)) |
            (1U << make_card(Suit::SUIT_S, Rank::RANK_Q)) |
            (1U << make_card(Suit::SUIT_D, Rank::RANK_K)) |
            (1U << make_card(Suit::SUIT_C, Rank::RANK_A)),
        Bet::LOW_STRAIGHT
        )
    );
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