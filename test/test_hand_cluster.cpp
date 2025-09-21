#include <gtest/gtest.h>
#include <string>
#include <iostream>

#include "ai/hand_cluster.hpp"

using namespace thai_poker;

TEST(HandClusterTest, BuildSaveLoad) {
    HandCluster hc;

    GameSample gs = hc.sample(3, 5);

    std::cerr << gs.h1 << ' ' << gs.h1_block << std::endl;
    std::cerr << gs.h2 << ' ' << gs.h2_block << std::endl;
    std::cerr << "h1 size: " << popcount(gs.h1) << std::endl;
    std::cerr << "h2 size: " << popcount(gs.h2) << std::endl;
}