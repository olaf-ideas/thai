#pragma once

#include "../core/thai_poker.hpp"
#include "../logic/hand_table.hpp"
#include "hand_cluster.hpp"

namespace thai_poker {

class CounterfacturalRegretMinimization {

public:

    CounterfacturalRegretMinimization() {

    }

    void trainIteration(int, Hand, Hand);
    void train();

private:

    double regret[HAND_SZ+1][KMEANS_K][BET_NB+1];
    double utility[HAND_SZ+1][KMEANS_K][BET_NB+1];
    HandTable hand_table;
    HandCluster hand_cluster;

};

} // thai_poker