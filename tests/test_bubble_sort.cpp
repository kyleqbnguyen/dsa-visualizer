#include "bubble_sort.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <vector>

TEST(BubbleSort, SortsUnsortedArray) {
  std::vector<int> data = {5, 3, 8, 1, 9, 2};
  dsa::bubble_sort<int>(data);
  EXPECT_TRUE(std::is_sorted(data.begin(), data.end()));
  EXPECT_EQ(data, (std::vector<int>{1, 2, 3, 5, 8, 9}));
}

TEST(BubbleSort, AlreadySorted) {
  std::vector<int> data = {1, 2, 3, 4, 5};
  dsa::bubble_sort<int>(data);
  EXPECT_EQ(data, (std::vector<int>{1, 2, 3, 4, 5}));
}

TEST(BubbleSort, ReverseSorted) {
  std::vector<int> data = {5, 4, 3, 2, 1};
  dsa::bubble_sort<int>(data);
  EXPECT_EQ(data, (std::vector<int>{1, 2, 3, 4, 5}));
}

TEST(BubbleSort, SingleElement) {
  std::vector<int> data = {42};
  dsa::bubble_sort<int>(data);
  EXPECT_EQ(data, std::vector<int>{42});
}

TEST(BubbleSort, EmptyArray) {
  std::vector<int> data = {};
  dsa::bubble_sort<int>(data);
  EXPECT_TRUE(data.empty());
}

TEST(BubbleSort, DuplicateElements) {
  std::vector<int> data = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
  dsa::bubble_sort<int>(data);
  EXPECT_TRUE(std::is_sorted(data.begin(), data.end()));
}

TEST(BubbleSort, TwoElements) {
  std::vector<int> data = {2, 1};
  dsa::bubble_sort<int>(data);
  EXPECT_EQ(data, (std::vector<int>{1, 2}));
}

TEST(BubbleSort, CallbackIsInvoked) {
  std::vector<int> data = {3, 1, 2};
  int callback_count = 0;

  dsa::bubble_sort<int>(data, [&](std::span<const int>, std::size_t,
                                  std::size_t, bool) { ++callback_count; });

  EXPECT_GT(callback_count, 0);
  EXPECT_TRUE(std::is_sorted(data.begin(), data.end()));
}
