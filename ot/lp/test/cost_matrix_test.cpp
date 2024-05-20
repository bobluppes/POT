#include <gtest/gtest.h>

#include "../cost_matrix.h"

#include <algorithm>
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

class CostMatrixDataTest : public testing::Test {
protected:
    std::vector<int> source_nodes{1, 1, 1};
    std::vector<int> demand_nodes{1, 1, 1};

    const std::size_t source_count{source_nodes.size()};
    const std::size_t demand_count{demand_nodes.size()};

    [[nodiscard]] std::vector<std::size_t> get_source_mapping() { return compute_retained_nodes(source_nodes); }
    [[nodiscard]] std::vector<std::size_t> get_demand_mapping() { return compute_retained_nodes(demand_nodes); }

    // Distance matrix:
    // 1, 2, 3
    // 4, 5, 6
    // 7, 8, 9
    std::vector<int> distance{1, 2, 3, 4, 5, 6, 7, 8, 9};
};

TEST_F(CostMatrixDataTest, AllNodesAreRetained) {
    // GIVEN: a cost matrix
    const auto source_mapping{get_source_mapping()};
    const auto demand_mapping{get_demand_mapping()};
    
    cost_matrix<int, std::size_t> cost_matrix{distance.data(), source_count, demand_count,
                                              source_mapping.size(), demand_mapping.size(),
                                              source_mapping, demand_mapping};

    // WHEN: we access all elements in the cost matrix
    std::vector<int> result{};
    for (int i{0}; i < cost_matrix.size(); ++i) {
        result.emplace_back(cost_matrix[i]);
    }

    // THEN: we expect to find all values in the distance matrix
    EXPECT_EQ(distance, result);
}

TEST_F(CostMatrixDataTest, NotAllNodesAreRetained) {
    // GIVEN: source and demand nodes with a 0 contribution
    source_nodes[1] = 0;
    demand_nodes[0] = 0;

    const auto source_mapping{get_source_mapping()};
    const auto demand_mapping{get_demand_mapping()};

    cost_matrix<int, std::size_t> cost_matrix{distance.data(), source_count, demand_count,
                                              source_mapping.size(), demand_mapping.size(),
                                              source_mapping, demand_mapping};

    // Retained distances:
    // 2, 3
    // 8, 9
    const std::vector<int> expected_retained_distances{2, 3, 8, 9};

    // WHEN: we access all elements in the cost matrix
    std::vector<int> result{};
    for (int i{0}; i < cost_matrix.size(); ++i) {
        result.emplace_back(cost_matrix[i]);
    }

    // THEN
    EXPECT_EQ(expected_retained_distances, result);
}



}