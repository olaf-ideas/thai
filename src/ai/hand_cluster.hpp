#pragma once

#include <array>
#include <random>
#include <utility>

#include "../logic/probability_table.hpp"

constexpr int KMEANS_K = 4000;
constexpr int KMEANS_ITER = 100;

namespace thai_poker {

struct GameSample {
    Hand h1; int h1_block;
    Hand h2; int h2_block;
};

class HandCluster {

    struct Point {
        std::array<double, BET_NB> p;
        int hand_index;
        int opp_size;

        [[nodiscard]] double distance(Point const&) const;
    };

    struct Cluster {
        std::vector<std::vector<Point>> blocks;
        std::vector<int> blocks_prefix_sum;
        std::vector<Point> centers;
    };

public:

    HandCluster();

    void build_kmeans();
    void build_clusters_ds();

    [[nodiscard]] GameSample sample(int, int);

private:

    std::pair<int, Hand> sample_hand(Cluster const&);

    ProbabilityTable const& prob_table;
    HandTable const& hand_table;
    std::mt19937_64 rng;

    std::vector<Cluster> clusters;
};

} // thai_poker