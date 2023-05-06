#ifndef THAI_CHECK_H
#define THAI_CHECK_H

#include <cassert>
#include <cstdio>

#include "const.h"
#include "utils.h"


namespace thai_poker {

namespace check {

int check_for_bet(Hand hand, Bet bet) {

    assert(bet != BET_CHECK);

    if (bet <= BET_HIGH_A) {
        int rank = bet - BET_HIGH_9;
        return (hand & ALL_RANK[rank]) >= 1;
    }
    else
    if (bet <= BET_PAIR_A) {
        int rank = bet - BET_PAIR_9;
        return pop_count(hand & ALL_RANK[rank]) >= 2;
    }
    else
    if (bet <= BET_LOW_STRAIGHT) {
        return (hand & ALL_RANK[RANK_9]) >= 1 &&
               (hand & ALL_RANK[RANK_T]) >= 1 &&
               (hand & ALL_RANK[RANK_J]) >= 1 &&
               (hand & ALL_RANK[RANK_Q]) >= 1 &&
               (hand & ALL_RANK[RANK_K]) >= 1;
    }
    else
    if (bet <= BET_HIGH_STRAIGHT) {
        return (hand & ALL_RANK[RANK_T]) >= 1 &&
               (hand & ALL_RANK[RANK_J]) >= 1 &&
               (hand & ALL_RANK[RANK_Q]) >= 1 &&
               (hand & ALL_RANK[RANK_K]) >= 1 &&
               (hand & ALL_RANK[RANK_A]) >= 1;
    }
    else
    if (bet <= BET_THREE_A) {
        int rank = bet - BET_THREE_9;
        return pop_count(hand & ALL_RANK[rank]) >= 3;
    }
    else
    if (bet <= BET_FULL_AK) {
        int three = (bet - BET_FULL_9T) / 5;
        int two = bet - BET_FULL_9T - three * 5;

        if (two >= three)
            two++;

        return pop_count(hand & ALL_RANK[three]) >= 3 &&
               pop_count(hand & ALL_RANK[two]) >= 2;
    }
    else
    if (bet <= BET_FLUSH_S) {
        int suit = bet - BET_FLUSH_C;
        return pop_count(hand & ALL_SUIT[suit]) >= 5;
    }
    else
    if (bet <= BET_QUADS_A) {
        int rank = bet - BET_QUADS_9;
        return pop_count(hand & ALL_RANK[rank]) >= 4;
    }
    else
    if (bet <= BET_POKER_S) {
        int suit = bet - BET_POKER_C;
        return (hand & SMALL_POKER[suit]) == SMALL_POKER[suit] ||
               (hand & ROYAL_POKER[suit]) == ROYAL_POKER[suit];
    }
    else
    if (bet <= BET_ROYAL_POKER_S) {
        int suit = bet - BET_ROYAL_POKER_C;
        return (hand & ROYAL_POKER[suit]) == ROYAL_POKER[suit];
    }

    assert(false);

    return -1;
}

}

}

#endif // THAI_CHECK_H