#include <gtest/gtest.h>
#include <string>
#include <iostream>

#include "logic/probability_table.hpp"
using namespace thai_poker;

TEST(ProbabilityTableTest, BuildSaveLoad) {

    ProbabilityTable& prob_table = ProbabilityTable::instance();

    const std::string table_filename = std::string(DATA_DIR) + "/TTP0.bin";
    std::cerr << "Saving probability table..." << std::endl;
    //EXPECT_NO_THROW(prob_table.save(table_filename));
    std::cerr << "Loading probability table..." << std::endl;
    EXPECT_NO_THROW(prob_table.load(table_filename));
}


TEST(ProbabilityTableTest, Comp) {
    ProbabilityTable& prob_table = ProbabilityTable::instance();

    EXPECT_EQ(
        prob_table.get_comp(Bet::HIGH_9, 1, Hand{0}),
        4
    );

    EXPECT_EQ(
        prob_table.get_comp(Bet::QUADS_K, 4, Hand{0}),
        1
    );

    EXPECT_EQ(
        prob_table.get_comp(Bet::FLUSH_C, 3, Hand{0}),
        0
    );
}