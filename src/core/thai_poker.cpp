#include "thai_poker.hpp"

#include <stdexcept>

namespace thai_poker {

// internals
namespace {

[[nodiscard]] inline bool has_high(Hand h, Rank r) noexcept {
    return (h & ALL_RANK[to_i(r)]) != 0;
}
[[nodiscard]] inline bool has_pair(Hand h, Rank r) noexcept {
    return popcount(h & ALL_RANK[to_i(r)]) >= 2;
}
[[nodiscard]] inline bool has_three(Hand h, Rank r) noexcept {
    return popcount(h & ALL_RANK[to_i(r)]) >= 3;
}
[[nodiscard]] inline bool has_quads(Hand h, Rank r) noexcept {
    return popcount(h & ALL_RANK[to_i(r)]) >= 4;
}
[[nodiscard]] inline bool has_flush(Hand h, Suit s) noexcept {
    return popcount(h & ALL_SUIT[to_i(s)]) >= 5;
}
[[nodiscard]] inline bool has_low_straight(Hand h) noexcept {
    return has_high(h, Rank::RANK_9) && has_high(h, Rank::RANK_T)
        && has_high(h, Rank::RANK_J) && has_high(h, Rank::RANK_Q)
        && has_high(h, Rank::RANK_K);
}
[[nodiscard]] inline bool has_high_straight(Hand h) noexcept {
    return has_high(h, Rank::RANK_T) && has_high(h, Rank::RANK_J)
        && has_high(h, Rank::RANK_Q) && has_high(h, Rank::RANK_K)
        && has_high(h, Rank::RANK_A);
}
[[nodiscard]] inline bool has_fullhouse(Hand h, Rank three, Rank two) noexcept {
    if (three == two) return false;
    return has_three(h, three) && has_pair(h, two);
}
[[nodiscard]] inline bool has_straight_flush_any(Hand h, Suit s) noexcept {
    return ((h & SMALL_POKER[to_i(s)]) == SMALL_POKER[to_i(s)]) 
        || ((h & ROYAL_POKER[to_i(s)]) == ROYAL_POKER[to_i(s)]);
}
[[nodiscard]] inline bool has_royal_straight_flush(Hand h, Suit s) noexcept {
    return ((h & ROYAL_POKER[to_i(s)]) == ROYAL_POKER[to_i(s)]);
}

} // namespace

bool satisfies_bet(Hand h, Bet b) {
    int r, s, idx, three, two;
    switch (b) {
        case Bet::HIGH_9 ... Bet::HIGH_A:
            r = to_i(b) - to_i(Bet::HIGH_9);
            return has_high(h, static_cast<Rank>(r));
        case Bet::PAIR_9 ... Bet::PAIR_A:
            r = to_i(b) - to_i(Bet::PAIR_9);
            return has_high(h, static_cast<Rank>(r));
        case Bet::LOW_STRAIGHT:
            return has_low_straight(h);
        case Bet::HIGH_STRAIGHT:
            return has_high_straight(h);
        case Bet::THREE_9 ... Bet::THREE_A:
            r = to_i(b) - to_i(Bet::THREE_9);
            return has_three(h, static_cast<Rank>(r));
        case Bet::FULL_9T ... Bet::FULL_AK:
            idx = to_i(b) - to_i(Bet::FULL_9T);
            three = idx / 5;
            two = idx % 5;
            if (two >= three) two++;
            return has_fullhouse(h, static_cast<Rank>(three), 
                                    static_cast<Rank>(two));
        case Bet::FLUSH_C ... Bet::FLUSH_S:
            s = to_i(b) - to_i(Bet::FLUSH_C);
            return has_flush(h, static_cast<Suit>(s));
        case Bet::QUADS_9 ... Bet::QUADS_A:
            r = to_i(b) - to_i(Bet::QUADS_9);
            return has_quads(h, static_cast<Rank>(r));
        case Bet::POKER_C ... Bet::POKER_S:
            s = to_i(b) - to_i(Bet::POKER_C);
            return has_straight_flush_any(h, static_cast<Suit>(s));
        case Bet::ROYAL_POKER_C ... Bet::ROYAL_POKER_S:
            s = to_i(b) - to_i(Bet::POKER_C);
            return has_royal_straight_flush(h, static_cast<Suit>(s));
        default:
            throw std::out_of_range("bet");
    }
    return false;
}

} // namespace thai_poker