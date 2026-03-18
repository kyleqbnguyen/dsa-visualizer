#include "linear_search.h"
#include <gtest/gtest.h>
#include <vector>

TEST(LinearSearch, FindsExistingElement) {
  std::vector<int> data = {1, 3, 5, 7, 9, 11};
  auto result = dsa::linear_search<int>(data, 7);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 3);
}

TEST(LinearSearch, FindsFirstElement) {
  std::vector<int> data = {10, 20, 30};
  auto result = dsa::linear_search<int>(data, 10);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 0);
}

TEST(LinearSearch, FindsLastElement) {
  std::vector<int> data = {10, 20, 30};
  auto result = dsa::linear_search<int>(data, 30);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 2);
}

TEST(LinearSearch, ReturnsNulloptWhenNotFound) {
  std::vector<int> data = {1, 2, 3, 4, 5};
  auto result = dsa::linear_search<int>(data, 99);
  EXPECT_FALSE(result.has_value());
}

TEST(LinearSearch, EmptyArray) {
  std::vector<int> data = {};
  auto result = dsa::linear_search<int>(data, 1);
  EXPECT_FALSE(result.has_value());
}

TEST(LinearSearch, SingleElement_Found) {
  std::vector<int> data = {42};
  auto result = dsa::linear_search<int>(data, 42);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 0);
}

TEST(LinearSearch, SingleElement_NotFound) {
  std::vector<int> data = {42};
  auto result = dsa::linear_search<int>(data, 7);
  EXPECT_FALSE(result.has_value());
}

TEST(LinearSearch, CallbackIsInvoked) {
  std::vector<int> data = {10, 20, 30};
  int callback_count = 0;

  dsa::linear_search<int>(data, 20,
                          [&](std::span<const int>, std::size_t, const int &,
                              bool) { ++callback_count; });

  EXPECT_EQ(callback_count, 2);
}
