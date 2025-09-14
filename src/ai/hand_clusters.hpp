#pragma once

#include "../logic/probability_table.hpp"

constexpr int MIN_CLUSTER_SIZE = 4;

namespace thai_poker {

class Cluster {

    struct Point {
        std::array<double, BET_NB> p;
        int hand_index;
        int opp_size;
    };

public:

    Cluster() {
        std::vector<Point> data;
        for (int opp_size = 1; opp_size <= HAND_SZ; opp_size++) {
            for (int hand_size = MIN_CLUSTER_SIZE; hand_size <= HAND_SZ; hand_size++) {
                for (int hand_index = 0; hand_index < HAND_NB; hand_index++) {
                    Hand hand = hand_table.from_index(hand_index);
                    if (popcount(hand) == hand_size) {
                        std::array<double, BET_NB> p;
                        for (int bet = 0; bet < BET_NB; bet++) {
                            p[bet] = prob_table.get(std::static_cast<Bet>(bet), opp_size + hand_size, hand);
                        }

                        data.push_back(Point{p, hand_index, opp_size});
                    }
                }
            }
        }
    }

private:

    ProbabilityTable prob_table;
    HandTable hand_table;
};

} // thai_poker