#pragma once

#include <string>
#include <vector>

namespace viz {

struct TestCase {
  std::string label;
  std::string description;
  std::vector<int> data;
  int target = 0;
};

inline auto linear_search_test_cases() -> std::vector<TestCase> {
  return {
      {
          .label = "Typical: Random array",
          .description = "15 random elements, target present in the middle",
          .data = {12, 45, 3, 28, 7, 19, 33, 41, 6, 22, 15, 37, 9, 50, 2},
          .target = 41,
      },
      {
          .label = "Edge: Target at first position",
          .description = "Target is the very first element",
          .data = {7, 14, 22, 35, 42, 8, 19, 3, 27, 11},
          .target = 7,
      },
      {
          .label = "Edge: Target at last position",
          .description = "Target is the very last element — worst case scan",
          .data = {14, 22, 35, 42, 8, 19, 3, 27, 11, 7},
          .target = 7,
      },
      {
          .label = "Worst: Target not found",
          .description = "Target does not exist — scans entire array",
          .data = {12, 45, 3, 28, 7, 19, 33, 41, 6, 22, 15, 37, 9, 50, 2},
          .target = 99,
      },
      {
          .label = "Edge: Single element",
          .description = "Array with only one element",
          .data = {42},
          .target = 42,
      },
  };
}

inline auto binary_search_test_cases() -> std::vector<TestCase> {
  return {
      {
          .label = "Typical: Mid-range target",
          .description = "Sorted array, target near the middle",
          .data = {2, 5, 8, 12, 16, 23, 38, 42, 56, 72, 91},
          .target = 23,
      },
      {
          .label = "Edge: Target at start",
          .description = "Target is the smallest element",
          .data = {2, 5, 8, 12, 16, 23, 38, 42, 56, 72, 91},
          .target = 2,
      },
      {
          .label = "Edge: Target at end",
          .description = "Target is the largest element",
          .data = {2, 5, 8, 12, 16, 23, 38, 42, 56, 72, 91},
          .target = 91,
      },
      {
          .label = "Worst: Target not found",
          .description = "Target missing — maximum comparisons",
          .data = {2, 5, 8, 12, 16, 23, 38, 42, 56, 72, 91},
          .target = 50,
      },
      {
          .label = "Edge: Single element",
          .description = "Sorted array with one element",
          .data = {42},
          .target = 42,
      },
      {
          .label = "Edge: Two elements",
          .description = "Minimal interesting binary search case",
          .data = {10, 20},
          .target = 20,
      },
  };
}

inline auto bubble_sort_test_cases() -> std::vector<TestCase> {
  return {
      {
          .label = "Typical: Random array",
          .description = "15 random unsorted elements",
          .data = {38, 27, 43, 3, 9, 82, 10, 55, 14, 36, 22, 68, 1, 47, 5},
          .target = 0,
      },
      {
          .label = "Worst: Reverse sorted",
          .description = "Completely reversed — maximum swaps",
          .data = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
          .target = 0,
      },
      {
          .label = "Edge: Already sorted",
          .description = "Array is already in order — early termination",
          .data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
          .target = 0,
      },
      {
          .label = "Edge: Nearly sorted",
          .description = "Almost sorted with a few elements out of place",
          .data = {1, 2, 3, 5, 4, 6, 7, 9, 8, 10},
          .target = 0,
      },
      {
          .label = "Edge: All same values",
          .description = "All elements identical — no swaps needed",
          .data = {7, 7, 7, 7, 7, 7, 7, 7},
          .target = 0,
      },
      {
          .label = "Edge: Two elements",
          .description = "Minimal case with elements in wrong order",
          .data = {20, 10},
          .target = 0,
      },
  };
}

inline auto get_test_cases(const std::string &algo_name)
    -> std::vector<TestCase> {
  if (algo_name == "linear_search")
    return linear_search_test_cases();
  if (algo_name == "binary_search")
    return binary_search_test_cases();
  if (algo_name == "bubble_sort")
    return bubble_sort_test_cases();
  return {};
}

} // namespace viz
