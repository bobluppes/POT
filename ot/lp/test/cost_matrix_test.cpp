#include <gtest/gtest.h>

#include "../cost_matrix.h"

#include <numeric>
#include <utility>
#include <vector>

namespace emd {

namespace {

    const auto create_identity_map{[](const std::size_t size) {
        std::vector<std::size_t> result{};
        result.resize(size);
        std::iota(result.begin(), result.end(), 0);
        return result;
    }};

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

class CostMatrixResizeTest : public testing::Test {
protected:
    const std::size_t source_count{3};
    const std::size_t demand_count{3};

    std::vector<int> distance{1, 2, 3, 4, 5, 6, 7, 8, 9};   // A distance matrix with size 9

    const std::vector<size_t> source_mapping{create_identity_map(source_count)};
    const std::vector<size_t> demand_mapping{create_identity_map(demand_count)};

    cost_matrix<int, std::size_t> cost{distance.data(), source_count, demand_count, source_count, demand_count, create_identity_map(source_count), create_identity_map(demand_count)};
};

TEST_F(CostMatrixResizeTest, NoResize) {
    // WHEN: we don't resize
    // THEN: the size of the cost_matrix is equal to the size of the distance matrix
    EXPECT_EQ(distance.size(), cost.size());
}

TEST_F(CostMatrixResizeTest, ResizeToSmallerValue) {
    // GIVEN: a smaller size than the distance matrix
    const auto smaller_size{distance.size() / 2};

    // WHEN: we resize to something smaller than the distance matrix size
    cost.resize(smaller_size);

    // THEN: the size of the cost_matrix is unchanged
    EXPECT_EQ(distance.size(), cost.size());
}

TEST_F(CostMatrixResizeTest, ResizeToLargerValue) {
    // GIVEN: a larger size than the distance matrix
    const auto larger_size{distance.size() * 2};

    // WHEN: we resize to a larger size
    cost.resize(larger_size);

    // THEN: the size has changed accordingly
    EXPECT_EQ(larger_size, cost.size());
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