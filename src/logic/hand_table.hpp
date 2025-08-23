#pragma once

#include <stdexcept>

#include "../core/thai_poker.hpp"

namespace thai_poker {

struct HandTable {
    std::array<int, 1U << CARD_NB> hand_to_index;
    std::array<Hand, HAND_NB> index_to_hand;

    HandTable() {
        int idx = 0;
        for (u32 h = 0; h < (1U << CARD_NB); h++) {
            if (popcount(h) <= HAND_SZ) {
                hand_to_index[h] = idx;
                index_to_hand[idx] = h;
                idx++;
            }
        }

        if (idx != HAND_NB) {
            throw std::runtime_error("HAND_NB mismatch - constant or generation is wrong");
        }
    }

    int to_index(Hand h) const { return hand_to_index[h]; }
    Hand from_index(int idx) const { return index_to_hand[idx]; }
};

} // namespace thai_poker