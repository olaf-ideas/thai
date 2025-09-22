#pragma once

#include <stdexcept>

#include "../core/thai_poker.hpp"

namespace thai_poker {

class HandTable {
public:
    HandTable(const HandTable&) = delete;
    HandTable& operator=(const HandTable&) = delete;

    static HandTable& instance();

    [[nodiscard]] int to_index(Hand h) const;
    [[nodiscard]] Hand from_index(int idx) const;

private:

    HandTable();

    static std::array<int, 1U << CARD_NB> hand_to_index;
    static std::array<Hand, HAND_NB> index_to_hand;

};

} // namespace thai_poker