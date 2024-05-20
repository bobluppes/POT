#include <gtest/gtest.h>

#include "../cost_matrix.h"

#include <utility>
#include <vector>

namespace emd {

namespace {

    [[nodiscard]] std::vector<std::size_t> compute_retained_nodes(const std::vector<int>& nodes) {
        std::vector<std::size_t> mapping{};

        for (int i{0}; i < nodes.size(); ++i) {
            if (nodes.at(i) != 0) {
                mapping.emplace_back(i);
            }
        }

        return mapping;
    }

}

TEST(CostMatrixTest, NotRetainingAllNodes) {
    // GIVEN
    std::vector<int> source_nodes{1, 0, 1};
    std::vector<int> demand_nodes{0, 1, 1};

    const auto source_count{source_nodes.size()};
    const auto demand_count{demand_nodes.size()};

    const auto source_mapping{compute_retained_nodes(source_nodes)};
    const auto retained_source_count{source_mapping.size()};

    const auto demand_mapping{compute_retained_nodes(demand_nodes)};
    const auto retained_demand_count{demand_mapping.size()};

    // Distance matrix:
    // 1, 2, 3
    // 4, 5, 6
    // 7, 8, 9
    std::vector<int> distance{1, 2, 3, 4, 5, 6, 7, 8, 9};

    cost_matrix<int, std::size_t> cost_matrix{distance.data(), source_count, demand_count, retained_source_count, retained_demand_count, source_mapping, demand_mapping};

    // WHEN - THEN
    // 2, 3
    // 8, 9
    const std::vector<int> expected_retained_distances{2, 3, 8, 9};

    int i{0};
    for (const auto expected : expected_retained_distances) {
        const auto val{cost_matrix[i++]};
        EXPECT_EQ(expected, val);
    }
}

}