#pragma once

#include <cstdint>
#include <array>
#include <cassert>

namespace thai_poker {

using u32 = std::uint32_t;
using Hand = u32; // 24-bit mask of present cards
using Card = u32; // (suit | (rank << 2))

constexpr int RANK_NB = 6;
constexpr int SUIT_NB = 4;
constexpr int CARD_NB = RANK_NB * SUIT_NB; // 24
constexpr int HAND_SZ = 6;
constexpr int HAND_NB = 190051; // sum C(24, k) for k=0..6

static inline int popcount(u32 v) { return __builtin_popcount(v); }
static inline bool has_bit(Hand h, int bit) { return (h >> bit) & 1; }
static inline Hand set_bit(Hand h, int bit) { return h | (1U << bit); }

enum class Rank : int {
    RANK_9 = 0,
    RANK_T,
    RANK_J,
    RANK_Q,
    RANK_K,
    RANK_A
};

enum class Suit : int {
    SUIT_C = 0, // CLUBS
    SUIT_D,     // DIAMONDS
    SUIT_H,     // HEARTS
    SUIT_S      // SPADES
};

enum class Bet : int {
    CHECK = -1,

    HIGH_9, HIGH_T, HIGH_J, HIGH_Q, HIGH_K, HIGH_A,

    PAIR_9, PAIR_T, PAIR_J, PAIR_Q, PAIR_K, PAIR_A,

    LOW_STRAIGHT, HIGH_STRAIGHT,

    THREE_9, THREE_T, THREE_J, THREE_Q, THREE_K, THREE_A,

    FULL_9T, FULL_9J, FULL_9Q, FULL_9K, FULL_9A,
    FULL_T9, FULL_TJ, FULL_TQ, FULL_TK, FULL_TA,
    FULL_J9, FULL_JT, FULL_JQ, FULL_JK, FULL_JA,
    FULL_Q9, FULL_QT, FULL_QJ, FULL_QK, FULL_QA,
    FULL_K9, FULL_KT, FULL_KJ, FULL_KQ, FULL_KA,
    FULL_A9, FULL_AT, FULL_AJ, FULL_AQ, FULL_AK,

    FLUSH_C, FLUSH_D, FLUSH_H, FLUSH_S,

    QUADS_9, QUADS_T, QUADS_J, QUADS_Q, QUADS_K, QUADS_A,

    POKER_C, POKER_D, POKER_H, POKER_S,

    ROYAL_POKER_C, ROYAL_POKER_D, ROYAL_POKER_H, ROYAL_POKER_S,
};

constexpr int to_i(Rank r) { return std::static_cast<int>(r); }
constexpr int to_i(Suit r) { return std::static_cast<int>(s); }
constexpr int to_i(Bet  b) { return std::static_cast<int>(b); }

// Card encoding: (suit | (rank << 2))
constexpr Card make_card(Suit s, Rank r) { return to_i(s) | (to_i(r) << 2); }

constexpr Hand mask_all_rank(Rank r) {
    return (1U << make_card(Suit::SUIT_C, r)) |
           (1U << make_card(Suit::SUIT_D, r)) |
           (1U << make_card(Suit::SUIT_H, r)) |
           (1U << make_card(Suit::SUIT_S, r));
}
constexpr Hand mask_all_suit(Suit s) {
    return (1U << make_card(s, Rank::RANK_9)) |
           (1U << make_card(s, Rank::RANK_T)) |
           (1U << make_card(s, Rank::RANK_J)) |
           (1U << make_card(s, Rank::RANK_Q)) |
           (1U << make_card(s, Rank::RANK_K)) |
           (1U << make_card(s, Rank::RANK_A));
}

static constexpr std::array<Hand, RANK_NB> ALL_RANK = {
    mask_all_rank(Rank::RANK_9), mask_all_rank(Rank::RANK_T), mask_all_rank(Rank::RANK_J),
    mask_all_rank(Rank::RANK_Q), mask_all_rank(Rank::RANK_K), mask_all_rank(Rank::RANK_A)
};

static constexpr std::array<Hand, SUIT_NB> ALL_SUIT = {
    mask_all_suit(Suit::SUIT_C), mask_all_suit(Suit::SUIT_D),
    mask_all_suit(Suit::SUIT_H), mask_all_suit(Suit::SUIT_S)
};

constexpr Hand mask_small_poker(Suit s) { // 9 T J Q K
    return (1U << make_card(s, Rank::RANK_9)) | (1U << make_card(s, Rank::RANK_T)) |
           (1U << make_card(s, Rank::RANK_J)) | (1U << make_card(s, Rank::RANK_Q)) |
           (1U << make_card(s, Rank::RANK_K));
}

constexpr Hand mask_royal_poker(Suit s) { // T J Q K A
    return (1U << make_card(s, Rank::RANK_T)) | (1U << make_card(s, Rank::RANK_J)) |
           (1U << make_card(s, Rank::RANK_Q)) | (1U << make_card(s, Rank::RANK_K)) |
           (1U << make_card(s, Rank::RANK_A));
}

static constexpr std::array<Hand, SUIT_NB> SMALL_POKER = {
    mask_small_poker(Suit::SUIT_C), mask_small_poker(Suit::SUIT_D),
    mask_small_poker(Suit::SUIT_H), mask_small_poker(Suit::SUIT_S)
};

static constexpr std::array<Hand, SUIT_NB> ROYAL_POKER = {
    mask_royal_poker(Suit::SUIT_C), mask_royal_poker(Suit::SUIT_D),
    mask_royal_poker(Suit::SUIT_H), mask_royal_poker(Suit::SUIT_S)
};

inline bool has_high(Hand h, Rank r) {
    return (h & ALL_RANK[to_i(r)]) != 0;
}
inline bool has_pair(Hand h, Rank r) {
    return popcount(h & ALL_RANK[to_i(r)]) >= 2;
}
inline bool has_three(Hand h, Rank r) {
    return popcount(h & ALL_RANK[to_i(r)]) >= 3;
}
inline bool has_quads(Hand h, Rank r) {
    return popcount(h & ALL_RANK[to_i(r)]) >= 4;
}
inline bool has_flush(Hand h, Suit s) {
    return popcount(h & ALL_SUIT[to_i(s)]) >= 5;
}
inline bool has_low_straight(Hand h) {
    return has_high(h, Rank::RANK_9) && has_high(h, Rank::RANK_T)
        && has_high(h, Rank::RANK_J) && has_high(h, Rank::RANK_Q)
        && has_high(h, Rank::RANK_K);
}
inline bool has_high_straight(Hand h) {
    return has_high(h, Rank::RANK_T) && has_high(h, Rank::RANK_J)
        && has_high(h, Rank::RANK_Q) && has_high(h, Rank::RANK_K)
        && has_high(h, Rank::RANK_A);
}
inline bool has_fullhouse(Hand h, Rank three, Rank two) {
    if (three == two) return false;
    return has_three(h, three) && has_pair(h, two);
}
inline bool has_straight_flush_any(Hand h, Suit s) {
    return ((h & SMALL_POKER[to_i(s)]) == SMALL_POKER[to_i(s)]) 
        || ((h & ROYAL_POKER[to_i(s)]) == ROYAL_POKER[to_i(s)]);
}
inline bool has_royal_poker(Hand h, Suit s) {
    return ((h & ROYAL_POKER[to_i(s)]) == ROYAL_POKER[to_i(s)]);
}

constexpr bool in_range(Bet lo, Bet hi, Bet b) {
    return to_i(lo) <= to_i(b) && to_i(b) <= to_i(hi);
}

bool satisfies_bet(Hand h, Bet b) {
    switch (b) {
        case Bet::HIGH_9 ... Bet::HIGH_A:
            int r = to_i(b) - to_i(Bet::HIGH_9);
            return has_high(h, std::static_cast<Rank>(r));
        case Bet::PAIR_9 ... Bet::PAIR_A:
            int r = to_i(b) - to_i(Bet::PAIR_9);
            return has_high(h, std::static_cast<Rank>(r));
        case Bet::LOW_STRAIGHT:
            return has_low_straight(h);
        case Bet::HIGH_STRAIGHT:
            return has_high_straight(h);
        case Bet::THREE_9 ... Bet::THREE_A:
            int r = to_i(b) - to_i(Bet::THREE_9);
            return has_three(h, std::static_cast<Rank>(r));
        case Bet::FULL_9T ... Bet::FULL_AK:
            int idx = to_i(b) - to_i(Bet::FULL_9T);
            int three = idx / 5;
            int two = idx % 5;
            if (two >= three) two++;
            return has_fullhouse(h, std::static_cast<Rank>(three), 
                                    std::static_cast<Rank>(two));
        case Bet::FLUSH_C ... Bet::FLUSH_S:
            int s = to_i(b) - to_i(Bet::FLUSH_C);
            return has_flush(h, std::static_cast<Suit>(s));
        case Bet::QUADS_9 ... Bet::QUADS_A:
            int r = to_i(b) - to_i(Bet::QUADS_9);
            return has_quads(h, std::static_cast<Rank>(r));
        case Bet::POKER_C ... Bet::POKER_S:
            int s = to_i(b) - to_i(Bet::POKER_C);
            return has_straight_flush_any(h, std::static_cast<Suit>(s));
        case Bet::ROYAL_POKER_C ... Bet::ROYAL_POKER_S:
            int s = to_i(b) - to_i(Bet::POKER_C);
            return has_royal_poker(h, std::static_cast<Suit>(s));
        default:
            assert(false && "Unknown bet");
    }
    return false;
}

} // namespace thai_poker