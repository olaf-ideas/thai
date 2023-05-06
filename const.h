#ifndef THAI_CONST_H
#define THAI_CONST_H

namespace thai_poker {

enum Rank : int {
    RANK_9 = 0,
    RANK_T,
    RANK_J,
    RANK_Q,
    RANK_K,
    RANK_A,
    RANK_NB
};

enum Suit : int {
    SUIT_C = 0, // CLUBS
    SUIT_D,     // DIAMONDS
    SUIT_H,     // HEARTS
    SUIT_S,     // SPADES
    SUIT_NB
};

constexpr int CARD_NB = RANK_NB * SUIT_NB;
constexpr int HAND_SZ = 6;
constexpr int HAND_NB = 190051; // number of possible hands (set)

enum Bet : int {

    BET_CHECK = -1,

    BET_HIGH_9,
    BET_HIGH_T,
    BET_HIGH_J,
    BET_HIGH_Q,
    BET_HIGH_K,
    BET_HIGH_A,

    BET_PAIR_9,
    BET_PAIR_T,
    BET_PAIR_J,
    BET_PAIR_Q,
    BET_PAIR_K,
    BET_PAIR_A,

    BET_LOW_STRAIGHT,
    BET_HIGH_STRAIGHT,

    BET_THREE_9,
    BET_THREE_T,
    BET_THREE_J,
    BET_THREE_Q,
    BET_THREE_K,
    BET_THREE_A,

    BET_FULL_9T,
    BET_FULL_9J,
    BET_FULL_9Q,
    BET_FULL_9K,
    BET_FULL_9A,
    BET_FULL_T9,
    BET_FULL_TJ,
    BET_FULL_TQ,
    BET_FULL_TK,
    BET_FULL_TA,
    BET_FULL_J9,
    BET_FULL_JT,
    BET_FULL_JQ,
    BET_FULL_JK,
    BET_FULL_JA,
    BET_FULL_Q9,
    BET_FULL_QT,
    BET_FULL_QJ,
    BET_FULL_QK,
    BET_FULL_QA,
    BET_FULL_K9,
    BET_FULL_KT,
    BET_FULL_KJ,
    BET_FULL_KQ,
    BET_FULL_KA,
    BET_FULL_A9,
    BET_FULL_AT,
    BET_FULL_AJ,
    BET_FULL_AQ,
    BET_FULL_AK,

    BET_FLUSH_C,
    BET_FLUSH_D,
    BET_FLUSH_H,
    BET_FLUSH_S,

    BET_QUADS_9,
    BET_QUADS_T,
    BET_QUADS_J,
    BET_QUADS_Q,
    BET_QUADS_K,
    BET_QUADS_A,

    BET_POKER_C,
    BET_POKER_D,
    BET_POKER_H,
    BET_POKER_S,

    BET_ROYAL_POKER_C,
    BET_ROYAL_POKER_D,
    BET_ROYAL_POKER_H,
    BET_ROYAL_POKER_S,

    BET_NB,
};

typedef unsigned int Card; // SUIT | (RANK << 2) // (0...23)
typedef unsigned int Hand; // (1 << CARD_0) | (1 << CARD_1) ... | (1 << CARD_N)

constexpr Hand ALL_RANK[RANK_NB] = {
    (1 << (SUIT_C | (RANK_9 << 2))) |
    (1 << (SUIT_D | (RANK_9 << 2))) |
    (1 << (SUIT_H | (RANK_9 << 2))) |
    (1 << (SUIT_S | (RANK_9 << 2))),
    
    (1 << (SUIT_C | (RANK_T << 2))) |
    (1 << (SUIT_D | (RANK_T << 2))) |
    (1 << (SUIT_H | (RANK_T << 2))) |
    (1 << (SUIT_S | (RANK_T << 2))),

    (1 << (SUIT_C | (RANK_J << 2))) |
    (1 << (SUIT_D | (RANK_J << 2))) |
    (1 << (SUIT_H | (RANK_J << 2))) |
    (1 << (SUIT_S | (RANK_J << 2))),

    (1 << (SUIT_C | (RANK_Q << 2))) |
    (1 << (SUIT_D | (RANK_Q << 2))) |
    (1 << (SUIT_H | (RANK_Q << 2))) |
    (1 << (SUIT_S | (RANK_Q << 2))),

    (1 << (SUIT_C | (RANK_K << 2))) |
    (1 << (SUIT_D | (RANK_K << 2))) |
    (1 << (SUIT_H | (RANK_K << 2))) |
    (1 << (SUIT_S | (RANK_K << 2))),
    
    (1 << (SUIT_C | (RANK_A << 2))) |
    (1 << (SUIT_D | (RANK_A << 2))) |
    (1 << (SUIT_H | (RANK_A << 2))) |
    (1 << (SUIT_S | (RANK_A << 2))),
};

constexpr Hand ALL_SUIT[SUIT_NB] = {
    (1 << (SUIT_C | (RANK_9 << 2))) |
    (1 << (SUIT_C | (RANK_T << 2))) |
    (1 << (SUIT_C | (RANK_J << 2))) |
    (1 << (SUIT_C | (RANK_Q << 2))) |
    (1 << (SUIT_C | (RANK_K << 2))) |
    (1 << (SUIT_C | (RANK_A << 2))),

    (1 << (SUIT_D | (RANK_9 << 2))) |
    (1 << (SUIT_D | (RANK_T << 2))) |
    (1 << (SUIT_D | (RANK_J << 2))) |
    (1 << (SUIT_D | (RANK_Q << 2))) |
    (1 << (SUIT_D | (RANK_K << 2))) |
    (1 << (SUIT_D | (RANK_A << 2))),

    (1 << (SUIT_H | (RANK_9 << 2))) |
    (1 << (SUIT_H | (RANK_T << 2))) |
    (1 << (SUIT_H | (RANK_J << 2))) |
    (1 << (SUIT_H | (RANK_Q << 2))) |
    (1 << (SUIT_H | (RANK_K << 2))) |
    (1 << (SUIT_H | (RANK_A << 2))),

    (1 << (SUIT_S | (RANK_9 << 2))) |
    (1 << (SUIT_S | (RANK_T << 2))) |
    (1 << (SUIT_S | (RANK_J << 2))) |
    (1 << (SUIT_S | (RANK_Q << 2))) |
    (1 << (SUIT_S | (RANK_K << 2))) |
    (1 << (SUIT_S | (RANK_A << 2))),
};

constexpr Hand SMALL_POKER[SUIT_NB] = {
    (1 << (SUIT_C | (RANK_9 << 2))) |
    (1 << (SUIT_C | (RANK_T << 2))) |
    (1 << (SUIT_C | (RANK_J << 2))) |
    (1 << (SUIT_C | (RANK_Q << 2))) |
    (1 << (SUIT_C | (RANK_K << 2))),

    (1 << (SUIT_D | (RANK_9 << 2))) |
    (1 << (SUIT_D | (RANK_T << 2))) |
    (1 << (SUIT_D | (RANK_J << 2))) |
    (1 << (SUIT_D | (RANK_Q << 2))) |
    (1 << (SUIT_D | (RANK_K << 2))),

    (1 << (SUIT_H | (RANK_9 << 2))) |
    (1 << (SUIT_H | (RANK_T << 2))) |
    (1 << (SUIT_H | (RANK_J << 2))) |
    (1 << (SUIT_H | (RANK_Q << 2))) |
    (1 << (SUIT_H | (RANK_K << 2))),

    (1 << (SUIT_S | (RANK_9 << 2))) |
    (1 << (SUIT_S | (RANK_T << 2))) |
    (1 << (SUIT_S | (RANK_J << 2))) |
    (1 << (SUIT_S | (RANK_Q << 2))) |
    (1 << (SUIT_S | (RANK_K << 2))),
};

constexpr Hand ROYAL_POKER[SUIT_NB] = {
    (1 << (SUIT_C | (RANK_T << 2))) |
    (1 << (SUIT_C | (RANK_J << 2))) |
    (1 << (SUIT_C | (RANK_Q << 2))) |
    (1 << (SUIT_C | (RANK_K << 2))) |
    (1 << (SUIT_C | (RANK_A << 2))),

    (1 << (SUIT_D | (RANK_T << 2))) |
    (1 << (SUIT_D | (RANK_J << 2))) |
    (1 << (SUIT_D | (RANK_Q << 2))) |
    (1 << (SUIT_D | (RANK_K << 2))) |
    (1 << (SUIT_D | (RANK_A << 2))),

    (1 << (SUIT_H | (RANK_T << 2))) |
    (1 << (SUIT_H | (RANK_J << 2))) |
    (1 << (SUIT_H | (RANK_Q << 2))) |
    (1 << (SUIT_H | (RANK_K << 2))) |
    (1 << (SUIT_H | (RANK_A << 2))),

    (1 << (SUIT_S | (RANK_T << 2))) |
    (1 << (SUIT_S | (RANK_J << 2))) |
    (1 << (SUIT_S | (RANK_Q << 2))) |
    (1 << (SUIT_S | (RANK_K << 2))) |
    (1 << (SUIT_S | (RANK_A << 2))),
};

} // thai_poker

#endif // THAI_CONST_H