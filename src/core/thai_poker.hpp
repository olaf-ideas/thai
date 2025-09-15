#pragma once

#include <cstdint>
#include <array>
#include <bit>

namespace thai_poker {

using u32 = std::uint32_t;
using Hand = u32; // 24-bit mask of present cards
using Card = u32; // (suit | (rank << 2))

constexpr int RANK_NB = 6;
constexpr int SUIT_NB = 4;
constexpr int CARD_NB = RANK_NB * SUIT_NB; // 24
constexpr int BET_NB  = 68;
constexpr int HAND_SZ = 6;
constexpr int HAND_NB = 190051; // sum C(24, k) for k=0..6

static inline int popcount(u32 v) { return std::popcount(v); }

enum class Rank : int { RANK_9 = 0, RANK_T, RANK_J, RANK_Q, RANK_K, RANK_A };
enum class Suit : int { SUIT_C = 0, SUIT_D, SUIT_H, SUIT_S };

enum class Bet : int {
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

    CHECK
};

[[nodiscard]] constexpr int to_i(Rank r) noexcept { return static_cast<int>(r); }
[[nodiscard]] constexpr int to_i(Suit s) noexcept { return static_cast<int>(s); }
[[nodiscard]] constexpr int to_i(Bet  b) noexcept { return static_cast<int>(b); }

// Card encoding: (suit | (rank << 2))
[[nodiscard]] constexpr Card make_card(Suit s, Rank r) noexcept { return to_i(s) | (to_i(r) << 2); }

[[nodiscard]] constexpr Hand mask_all_rank(Rank r) {
    return (1U << make_card(Suit::SUIT_C, r)) |
           (1U << make_card(Suit::SUIT_D, r)) |
           (1U << make_card(Suit::SUIT_H, r)) |
           (1U << make_card(Suit::SUIT_S, r));
}
[[nodiscard]] constexpr Hand mask_all_suit(Suit s) {
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

[[nodiscard]] constexpr Hand mask_small_poker(Suit s) { // 9 T J Q K
    return (1U << make_card(s, Rank::RANK_9)) | (1U << make_card(s, Rank::RANK_T)) |
           (1U << make_card(s, Rank::RANK_J)) | (1U << make_card(s, Rank::RANK_Q)) |
           (1U << make_card(s, Rank::RANK_K));
}

[[nodiscard]] constexpr Hand mask_royal_poker(Suit s) { // T J Q K A
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

[[nodiscard]] bool satisfies_bet(Hand h, Bet b);

} // namespace thai_poker