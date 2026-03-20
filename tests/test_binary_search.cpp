#include "binary_search.h"
#include <gtest/gtest.h>
#include <vector>

TEST(BinarySearch, FindsMiddleElement) {
  std::vector<int> data = {1, 3, 5, 7, 9, 11, 13};
  int result = dsa::binary_search(data, 7);
  EXPECT_EQ(result, 3);
}

TEST(BinarySearch, FindsFirstElement) {
  std::vector<int> data = {1, 3, 5, 7, 9};
  int result = dsa::binary_search(data, 1);
  EXPECT_EQ(result, 0);
}

TEST(BinarySearch, FindsLastElement) {
  std::vector<int> data = {1, 3, 5, 7, 9};
  int result = dsa::binary_search(data, 9);
  EXPECT_EQ(result, 4);
}

TEST(BinarySearch, ReturnsNulloptWhenNotFound) {
  std::vector<int> data = {1, 3, 5, 7, 9};
  int result = dsa::binary_search(data, 6);
  EXPECT_EQ(result, -1);
}

TEST(BinarySearch, EmptyArray) {
  std::vector<int> data = {};
  int result = dsa::binary_search(data, 1);
  EXPECT_EQ(result, -1);
}

TEST(BinarySearch, SingleElement_Found) {
  std::vector<int> data = {42};
  int result = dsa::binary_search(data, 42);
  EXPECT_EQ(result, 0);
}

TEST(BinarySearch, SingleElement_NotFound) {
  std::vector<int> data = {42};
  int result = dsa::binary_search(data, 7);
  EXPECT_EQ(result, -1);
}

TEST(BinarySearch, TwoElements) {
  std::vector<int> data = {10, 20};

  int r1 = dsa::binary_search(data, 10);
  EXPECT_EQ(r1, 0);

  int r2 = dsa::binary_search(data, 20);
  EXPECT_EQ(r2, 1);

  int r3 = dsa::binary_search(data, 15);
  EXPECT_EQ(r3, -1);
}
