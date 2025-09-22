#include "hand_table.hpp"

namespace thai_poker {

std::array<int, 1U << CARD_NB> HandTable::hand_to_index{};
std::array<Hand, HAND_NB> HandTable::index_to_hand{};

HandTable& HandTable::instance() {
    static HandTable singleton;
    return singleton;
}

HandTable::HandTable() {
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

int HandTable::to_index(Hand h) const {
    return hand_to_index[h];
}

Hand HandTable::from_index(int idx) const {
    return index_to_hand[idx];
}

} // namespace thai_poker