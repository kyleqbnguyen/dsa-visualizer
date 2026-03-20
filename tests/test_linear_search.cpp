#include "linear_search.h"
#include <gtest/gtest.h>
#include <vector>

TEST(LinearSearch, FindsExistingElement) {
  std::vector<int> data = {1, 3, 5, 7, 9, 11};
  int result = dsa::linear_search(data, 7);
  EXPECT_EQ(result, 3);
}

TEST(LinearSearch, FindsFirstElement) {
  std::vector<int> data = {10, 20, 30};
  int result = dsa::linear_search(data, 10);
  EXPECT_EQ(result, 0);
}

TEST(LinearSearch, FindsLastElement) {
  std::vector<int> data = {10, 20, 30};
  int result = dsa::linear_search(data, 30);
  EXPECT_EQ(result, 2);
}

TEST(LinearSearch, ReturnsNulloptWhenNotFound) {
  std::vector<int> data = {1, 2, 3, 4, 5};
  int result = dsa::linear_search(data, 99);
  EXPECT_EQ(result, -1);
}

TEST(LinearSearch, EmptyArray) {
  std::vector<int> data = {};
  int result = dsa::linear_search(data, 1);
  EXPECT_EQ(result, -1);
}

TEST(LinearSearch, SingleElement_Found) {
  std::vector<int> data = {42};
  int result = dsa::linear_search(data, 42);
  EXPECT_EQ(result, 0);
}

TEST(LinearSearch, SingleElement_NotFound) {
  std::vector<int> data = {42};
  int result = dsa::linear_search(data, 7);
  EXPECT_EQ(result, -1);
}
