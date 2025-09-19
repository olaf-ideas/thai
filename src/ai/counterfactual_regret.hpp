#pragma once

#include "../core/thai_poker.hpp"
#include "../logic/hand_table.hpp"
#include "hand_cluster.hpp"

namespace thai_poker {

class CounterfacturalRegretMinimization {

public:

    CounterfacturalRegretMinimization() {

    }

    void trainIteration(int trainingStep, Hand h1, Hand h2) {
        int h1_index = hand_table.to_index(h1);
        int h1_size = popcount(h1);
        int h2_index = hand_table.to_index(h2);
        int h2_size = popcount(h2);

        for (int cur_bet = 0; cur_bet < BET_NB; cur_bet++) {
            double counterfactualUtility = 0;
            double positiveRegretSum = 0;
            for (int nxt_bet = bet + 1; nxt_bet < BET_NB; nxt_bet++) {
                if (regret[h1_index][h2_size][nxt_bet] > 0) {
                    positiveRegretSum += regret[h1_index][h2_size][nxt_bet];
                }
            }
            
            double sigma[BET_NB];
            if (positiveRegretSum < 1e-7) {
                for (int nxt_bet = bet + 1; nxt_bet < BET_NB; nxt_bet++) {
                    sigma[nxt_bet] = 1.0 / (BET_NB - bet);
                }
            }
            else {
                for (int nxt_bet = bet + 1; nxt_bet < BET_NB; nxt_bet++) {
                    sigma[nxt_bet] = regret[h1_index][h2_size][nxt_bet] / positiveRegretSum;
                }
            }
        }
    }

    void train() {
        // randomize hands of both players
        // iterate over the possible plays of the players from the biggest bid to the smallest
        // calculate the immediate counterfactual regret
        // update the policy
        // parallelize the same hands?
    }

private:

    double regret[HAND_SZ+1][KMEANS_K][BET_NB+1];
    double utility[HAND_SZ+1][KMEANS_K][BET_NB+1];
    HandTable hand_table;
    HandCluster hand_cluster;

};

} // thai_poker