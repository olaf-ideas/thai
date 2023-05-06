#ifndef THAI_LUCK_H
#define THAI_LUCK_H

#include <cassert>
#include <cstdio>

#include <omp.h>

#include "const.h"
#include "utils.h"
#include "check.h"

namespace thai_poker {

namespace luck {

// netwon binomial
int C[CARD_NB + 1][CARD_NB + 1];

// probability that bet exist (~ 1.1 GB)
float P[BET_NB][CARD_NB + 1][HAND_NB];

// given hand return index (only hands with <= 6 cards), -1 otherwise
int hand_to_index[1 << CARD_NB];

// given index return hand
int index_to_hand[HAND_NB];

void init_netwon_binomial() {
    C[0][0] = 1;
    for (int n = 1; n <= CARD_NB; n++) {
        C[n][0] = C[n][n] = 1;

        for (int k = 1; k < n; k++) {
            C[n][k] = C[n - 1][k] + C[n - 1][k - 1];
        }
    }
}

void init_hand_lookup() {
    fprintf(stderr, "INIT: hand lookup");

    int index = 0;
    for (int hand = 0; hand < (1 << CARD_NB); hand++) {
        if (pop_count(hand) > 6) {
            hand_to_index[hand] = -1;
        }
        else {
            hand_to_index[hand] = index;
            index_to_hand[index] = hand;
            index++;
        }
    }

    assert(index == HAND_NB);

    fprintf(stderr, " => finished\n");
}

void init_luck_tables() {
    fprintf(stderr, "INIT: luck tables\n");

    /*
    for (int bet = 0; bet < BET_NB; bet++) {

        const int full = (1 << CARD_NB) - 1;
        static int _P[1 << CARD_NB];

        for (int card_nb = 0; card_nb <= CARD_NB; card_nb++) {
            fprintf(stderr, "starting : %d %d\n", bet, card_nb);

            for (int deck = 0; deck < (1 << CARD_NB); deck++) {
                _P[deck ^ full] = (pop_count(deck) == card_nb && check::check_for_bet(deck, Bet(bet)) == true);
            }

            for (int bit = 0; bit < CARD_NB; bit++) {
                for (int deck = 0; deck < (1 << CARD_NB); deck++) {
                    if (deck & (1 << bit)) {
                        _P[deck] += _P[deck ^ (1 << bit)];
                    }
                }
            }

            for (int hand = 0; hand < (1 << CARD_NB); hand++) {
                int hand_id = hand_to_index[hand];
                int in_hand = pop_count(hand);

                if (hand_id != -1 && in_hand <= card_nb) {
                    P[bet][card_nb][hand_id] = (float) _P[hand ^ full] / (float) C[CARD_NB - in_hand][card_nb - in_hand];
                    // fprintf(stderr, "bet = %d card_nb = %d hand_id = %d => (P = %d, C = %d) %f\n", 
                        // bet, card_nb, hand_id, _P[hand ^ full], C[CARD_NB - in_hand][card_nb - in_hand], P[bet][card_nb][hand_id]);
                }
            }
        }
    }
    // */

    fprintf(stderr, "reading from a file...\n");

    FILE *file = fopen("P_db.bin", "rb");

    for (int bet = 0; bet < BET_NB; bet++) {
        for (int card_nb = 0; card_nb <= CARD_NB; card_nb++) {
            fread(P[bet][card_nb], sizeof(float), HAND_NB, file);
        }
    }

    fclose(file);

    fprintf(stderr, "\n => finished\n");
}

void init_luck() {
    init_netwon_binomial();
    init_hand_lookup();
    init_luck_tables();
}

}

} // thai_poker

#endif // THAI_LUCK_H