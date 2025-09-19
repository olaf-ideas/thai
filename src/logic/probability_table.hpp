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

    ProbabilityTable() : comb_(), table_() { }
    ProbabilityTable(const ProbabilityTable&) = delete;
    ProbabilityTable& operator=(const ProbabilityTable&) = delete;

    static ProbabilityTable& instance() {
        static ProbabilityTable singleton;
        return singleton;
    }

    [[nodiscard]] double get_prob(Bet b, int card_nb, Hand h) const;
    [[nodiscard]] int get_comp(Bet b, int card_nb, Hand h) const;
    void build();
    void load(const std::string& path);
    void save(const std::string& path) const;

private:

    Comb24 comb_;
    HandTable table_;
    static int P_[BET_NB][CARD_NB+1][HAND_NB];
};


} // namespace thai_poker