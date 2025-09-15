#include "probability_table.hpp"

#include <array>
#include <cstring>
#include <iostream>

namespace thai_poker {

int ProbabilityTable::P_[BET_NB][CARD_NB+1][HAND_NB]{};


int ProbabilityTable::get_comp(Bet b, int card_nb, Hand h) const {
    if (card_nb < 0 || card_nb > CARD_NB)
        throw std::out_of_range("card_nb");
    int hand_index = table_.to_index(h);
    if (hand_index < 0 || hand_index >= HAND_NB)
        throw std::out_of_range("hand does not exist");
    return P_[to_i(b)][card_nb][hand_index];
}

double ProbabilityTable::get_prob(Bet b, int card_nb, Hand h) const {
    if (b == Bet::CHECK)
        return 0.0;
    int in_hand = popcount(h);
    // P of satisfied bet = # (possible games with satisfied bet) / # of possible games
    return get_comp(b, card_nb, h) * comb_.get_inv(CARD_NB - in_hand, card_nb - in_hand);
}

void ProbabilityTable::build() {
    std::vector<int> H(1U << CARD_NB);
    for (int bet = 0; bet < BET_NB; bet++) {
        std::cerr << "Bet: " << bet << '\n';
        for (int card_nb = 0; card_nb <= CARD_NB; card_nb++) {
            for (Hand deck = 0; deck < (1U << CARD_NB); deck++) {
                if (popcount(deck) == card_nb &&
                    satisfies_bet(deck, static_cast<Bet>(bet)))
                    H[deck] = 1;
                else
                    H[deck] = 0;
            }

            // SOS dp
            for (int bit = 0; bit < CARD_NB; bit++) {
                for (Hand deck = 0; deck < (1U << CARD_NB); deck++) {
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

void ProbabilityTable::load(const std::string& path) {
    FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) throw std::runtime_error("Cannot open " + path);
    char magic[4];
    if (std::fread(magic, 1, 4, f) != 4 || std::memcmp(magic, "TTP0", 4) != 0) {
        std::fclose(f);
        throw std::runtime_error("Bad P table magic");
    }
    u32 version=0, bets=0, cards=0, hands=0;
    std::fread(&version, 4, 1, f);
    std::fread(&bets, 4, 1, f);
    std::fread(&cards, 4, 1, f);
    std::fread(&hands, 4, 1, f);
    if (version != VERSION || bets != BET_NB || cards != CARD_NB+1 || hands != HAND_NB) {
        std::fclose(f);
        throw std::runtime_error("P table dim/version mismatch");
    }
    size_t need = BET_NB * (CARD_NB+1) * HAND_NB;
    size_t got = std::fread(P_, sizeof(int), need, f);
    std::fclose(f);
    if (got != need)
        throw std::runtime_error("Could not read P table from " + path);
}

void ProbabilityTable::save(const std::string& path) const {
    FILE* f = std::fopen(path.c_str(), "wb");
    if (!f) throw std::runtime_error("Cannot open " + path);
    std::fwrite("TTP0", 1, 4, f);
    u32 version = VERSION, bets = BET_NB, cards = CARD_NB+1, hands = HAND_NB;
    std::fwrite(&version, 4, 1, f);
    std::fwrite(&bets, 4, 1, f);
    std::fwrite(&cards, 4, 1, f);
    std::fwrite(&hands, 4, 1, f);
    std::fwrite(P_, sizeof(int), BET_NB*(CARD_NB+1)*HAND_NB, f);
    std::fclose(f);
}

} // namespace thai_poker