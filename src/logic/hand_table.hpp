#pragma once

#include <stdexcept>

#include "../core/thai_poker.hpp"

namespace thai_poker {

class HandTable {
public:
    HandTable();

    [[nodiscard]] int to_index(Hand h) const;
    [[nodiscard]] Hand from_index(int idx) const;

private:

    std::array<int, 1U << CARD_NB> hand_to_index{};
    std::array<Hand, HAND_NB> index_to_hand{};

};

} // namespace thai_poker