#pragma once

#include "../core/thai_poker.hpp"

namespace thai_poker {

struct Comb24 {
    int C[CARD_NB + 1][CARD_NB + 1]{};
    double I[CARD_NB + 1][CARD_NB + 1]{};

    Comb24();

    int get(int n, int k) const;
    double get_inv(int n, int k) const;
};

} // namespace thai_poker