#include <algorithm>
#include <vector>

#include <gtest/gtest.h>

#include "algo_recorder.h"

TEST(VizRecordings, LinearSearchRecordingHasTerminalResult) {
  std::vector<int> data = {4, 8, 15, 16, 23, 42};
  auto rec = viz::record_linear_search(data, 23);

  ASSERT_FALSE(rec.steps.empty());
  EXPECT_EQ(rec.algorithm_name, "linear_search");
  EXPECT_EQ(rec.steps.back().found_index, 4);
}

TEST(VizRecordings, BinarySearchRecordingUsesSortedData) {
  std::vector<int> data = {9, 1, 5, 3, 7};
  auto rec = viz::record_binary_search(data, 7);

  ASSERT_FALSE(rec.steps.empty());
  EXPECT_TRUE(std::is_sorted(rec.steps.front().data.begin(),
                             rec.steps.front().data.end()));
  EXPECT_GE(rec.steps.back().found_index, 0);
}

TEST(VizRecordings, BubbleSortRecordingEndsSorted) {
  std::vector<int> data = {5, 1, 4, 2, 8};
  auto rec = viz::record_bubble_sort(data);

  ASSERT_FALSE(rec.steps.empty());
  EXPECT_TRUE(
      std::is_sorted(rec.steps.back().data.begin(), rec.steps.back().data.end()));
  EXPECT_EQ(rec.steps.back().sorted_boundary, 0);
}
