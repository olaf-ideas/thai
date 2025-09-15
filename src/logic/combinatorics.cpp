#include "combinatorics.hpp"

namespace thai_poker {

Comb24::Comb24() {
    C[0][0] = 1;
    for (int n = 1; n <= CARD_NB; n++) {
        C[n][0] = C[n][n] = 1;
        for (int k = 1; k < n; k++)
            C[n][k] = C[n-1][k] + C[n-1][k-1];
    }

    for (int n = 0; n <= CARD_NB; n++) {
        for (int k = 0; k <= n; k++) {
            I[n][k] = (double) 1 / C[n][k];
        }
    }
}

int Comb24::get(int n, int k) const {
    return C[n][k];
}

double Comb24::get_inv(int n, int k) const {
    return I[n][k];
}

} // namespace thai_poker