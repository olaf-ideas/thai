#pragma once

#include <stdexcept>

#include "../core/thai_poker.hpp"

namespace thai_poker {

struct HandTable {
    int hand_to_index[1U << CARD_NB];
    Hand index_to_hand[HAND_NB];

    HandTable() {
        int idx = 0;
        for (u32 h = 0; h < (1U << CARD_NB); h++) {
            if (popcount(h) <= HAND_SZ) {
                hand_to_index[h] = idx;
                index_to_hand[idx] = h;
                idx++;
            }
            else {
                hand_to_index[h] = -1;
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