#pragma once

#include <string>
#include <vector>

#include "../core/thai_poker.hpp"
#include "combinatorics.hpp"
#include "hand_table.hpp"

namespace thai_poker {

class ProbabilityTable {
public:
    static constexpr int VERSION = 1;

    ProbabilityTable() = default;

    void load(const std::string& path) {

    }

    void save(const std::string& path) const {

    }

    double get(Bet b, int card_nb, Hand h) const {
        if (b == Bet::CHECK)
            return 0.0;
        if (card_nb < 0 || card_nb > CARD_NB)
            throw std::out_of_range("card_nb");
        int hand_index = table_.to_index(h);
        if (hand_index < 0 || hand_index >= HAND_NB)
            throw std::out_of_range(hand_index);
        int in_hand = popcount(h);
        return P_[to_i(b)][card_nb][hand_index]
             * comb_.get_inv(CARD_NB - in_hand, card_nb - in_hand);
    }

    void build() {
        std::array<int, 1U << CARD_NB> H;
        for (int bet = 0; bet < BET_NB; bet++) {
            const Hand full_hand = (1 << CARD_NB) - 1;

            for (int card_nb = 0; card_nb <= CARD_NB; card_nb++) {
                for (Card deck = 0; deck < (1U << CARD_NB); deck++) {
                    if (popcount(deck) == card_nb && 
                        satisfies_bet(deck, std::static_cast<Bet>(b)))
                        H[deck] = 1;
                    else
                        H[deck] = 0;
                }

                for (int bit = 0; bit < CARD_NB; bit++) {
                    for (Card deck = 0; deck < (1U << CARD_NB); deck++) {
                        if ((deck >> bit & 1) == 0) {
                            H[deck] += H[deck ^ (1 << bit)];
                        }
                    }
                }

                for (Hand hand = 0; hand < (1U << CARD_NB); hand++) {
                    int hand_index = table_.to_index(hand);
                    int in_hand = popcount(hand);
                    if (hand_index != -1 && in_hand <= card_nb) {
                        P_[bet][card_nb][hand_index] = H[hand];
                    }
                }
            }
        }
    }

private:

    Comb24 comb_;
    HandTable table_;
    int P_[BET_NB][CARD_NB][HAND_NB];
    std::vector<float> data_;
};

} // namespace thai_poker