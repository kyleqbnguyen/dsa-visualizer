#include "binary_search.h"
#include <gtest/gtest.h>
#include <vector>

TEST(BinarySearch, FindsMiddleElement) {
  std::vector<int> data = {1, 3, 5, 7, 9, 11, 13};
  auto result = dsa::binary_search<int>(data, 7);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 3);
}

TEST(BinarySearch, FindsFirstElement) {
  std::vector<int> data = {1, 3, 5, 7, 9};
  auto result = dsa::binary_search<int>(data, 1);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 0);
}

TEST(BinarySearch, FindsLastElement) {
  std::vector<int> data = {1, 3, 5, 7, 9};
  auto result = dsa::binary_search<int>(data, 9);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 4);
}

TEST(BinarySearch, ReturnsNulloptWhenNotFound) {
  std::vector<int> data = {1, 3, 5, 7, 9};
  auto result = dsa::binary_search<int>(data, 6);
  EXPECT_FALSE(result.has_value());
}

TEST(BinarySearch, EmptyArray) {
  std::vector<int> data = {};
  auto result = dsa::binary_search<int>(data, 1);
  EXPECT_FALSE(result.has_value());
}

TEST(BinarySearch, SingleElement_Found) {
  std::vector<int> data = {42};
  auto result = dsa::binary_search<int>(data, 42);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 0);
}

TEST(BinarySearch, SingleElement_NotFound) {
  std::vector<int> data = {42};
  auto result = dsa::binary_search<int>(data, 7);
  EXPECT_FALSE(result.has_value());
}

TEST(BinarySearch, TwoElements) {
  std::vector<int> data = {10, 20};

  auto r1 = dsa::binary_search<int>(data, 10);
  ASSERT_TRUE(r1.has_value());
  EXPECT_EQ(r1.value(), 0);

  auto r2 = dsa::binary_search<int>(data, 20);
  ASSERT_TRUE(r2.has_value());
  EXPECT_EQ(r2.value(), 1);

  auto r3 = dsa::binary_search<int>(data, 15);
  EXPECT_FALSE(r3.has_value());
}

TEST(BinarySearch, CallbackIsInvoked) {
  std::vector<int> data = {1, 2, 3, 4, 5, 6, 7};
  int callback_count = 0;

  dsa::binary_search<int>(data, 5,
                          [&](std::span<const int>, std::size_t, std::size_t,
                              std::size_t, const int &,
                              bool) { ++callback_count; });

  EXPECT_GT(callback_count, 0);
  EXPECT_LE(callback_count, 4);
}
