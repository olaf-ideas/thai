#pragma once

#include "../core/thai_poker.hpp"

namespace thai_poker {

struct Comb24 {
    int C[CARD_NB + 1][CARD_NB + 1];
    Comb24() {
        C[0][0] = 1;
        for (int n = 1; n <= CARD_NB; n++) {
            C[n][0] = C[n][n] = 1;
            for (int k = 1; k < n; k++)
                C[n][k] = C[n-1][k] + C[n-1][k-1];
        }
    }

    int operator()(int n, int k) const { return C[n][k]; }
};

} // namespace thai_poker