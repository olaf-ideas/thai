#include <cstdio>

#include "luck.h"

using namespace thai_poker;

void print_hand(Hand hand) {
    static char suit_name[] = {'C', 'D', 'H', 'S'};
    static char rank_name[] = {'9', 'T', 'J', 'Q', 'K', 'A'};

    for (int suit = 0; suit < SUIT_NB; suit++) {
        for (int rank = 0; rank < RANK_NB; rank++) {
            Card card = (suit | (rank << 2));

            if (hand & (1 << card)) {
                fprintf(stderr, "%c%c ", suit_name[suit], rank_name[rank]);
            }
        }
    }

    fprintf(stderr, "\n");
}

float brute_P(Bet bet, int card_nb, int hand_id) {
    Hand hand = luck::index_to_hand[hand_id];

    fprintf(stderr, "hand: ");
    print_hand(hand);

    int all = 0, good = 0;

    for (int deck = 0; deck < (1 << CARD_NB); deck++) {
        if (pop_count(deck) == card_nb && (deck & hand) == hand) {
            all++;

            if (check::check_for_bet(deck, bet)) {
                good++;
            }
        }
    }

    float p = (float) good / (float) all;

    fprintf(stderr, "bet = %d card_nb = %d hand_id = %d hand = %d\n", (int) bet, card_nb, hand_id, hand);
    fprintf(stderr, " => all = %d, good = %d P = %f\n", all, good, p);

    return p;
}

int main() {

    luck::init_luck();
    
    while (true) {
        int x, y, z; scanf("%d %d %d", &x, &y, &z);

        printf("%f vs %f\n", luck::P[x][y][z], brute_P(Bet{x}, y, z));
    }



}