#include <gtest/gtest.h>
#include <string>
#include <iostream>

#include "logic/probability_table.hpp"
using namespace thai_poker;

TEST(ProbabilityTableTest, BuildSaveLoad) {

    ProbabilityTable& prob_table = ProbabilityTable::instance();

    const std::string table_filename = std::string(DATA_DIR) + "/TTP0.bin";
    std::cerr << "Building probability table..." << std::endl;
    EXPECT_NO_THROW(prob_table.build()); // it may take a while (~4-10min)
    std::cerr << "Saving probability table..." << std::endl;
    EXPECT_NO_THROW(prob_table.save(table_filename));
    std::cerr << "Loading probability table..." << std::endl;
    EXPECT_NO_THROW(prob_table.load(table_filename));
}