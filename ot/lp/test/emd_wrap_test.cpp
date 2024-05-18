#include <gtest/gtest.h>

#include "../EMD.h"

#include <vector>

namespace emd {

TEST(EmdTest, CanSolveEmdWithTwoNodes) {
    int supply_count{2};
    int demand_count{2};
    std::vector<double> supply_weights{1, 0};
    std::vector<double> demand_weights{0, 1};
    std::vector<double> distance_matrix{0, 1, 1, 0};
    std::vector<double> flow_matrix{0, 0, 0, 0};
    std::vector<double> alpha{1, 2};
    std::vector<double> beta{1, 2};
    double cost{};
    uint64_t maxIter{};

    auto result = EMD_wrap(supply_count, demand_count, supply_weights.data(), demand_weights.data(),
             distance_matrix.data(), flow_matrix.data(), alpha.data(), beta.data(),
             &cost, maxIter);

    EXPECT_EQ(1, result);
}

}

