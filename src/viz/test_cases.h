#pragma once

#include "list_snapshot.h"
#include "ring_buffer_snapshot.h"

#include <string>
#include <vector>

namespace viz {

struct TestCase {
  std::string label;
  std::string description;
  std::vector<int> data;
  int target = 0;
};

struct ListTestCase {
  std::string label;
  std::string description;
  std::vector<int> initial_values;
  ListOp op;
  int value = 0;
  int index = 0;
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

inline auto two_crystal_balls_test_cases() -> std::vector<TestCase> {
  return {
      {
          .label = "Typical: Midpoint break",
          .description = "Floors are safe, then start breaking in the middle",
          .data = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1},
          .target = 0,
      },
      {
          .label = "Edge: Immediate break",
          .description = "First floor already breaks",
          .data = {1, 1, 1, 1, 1, 1, 1, 1},
          .target = 0,
      },
      {
          .label = "Edge: Last floor break",
          .description = "Only the final floor breaks",
          .data = {0, 0, 0, 0, 0, 0, 0, 1},
          .target = 0,
      },
      {
          .label = "Worst: Never breaks",
          .description = "No floor causes a break",
          .data = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
          .target = 0,
      },
      {
          .label = "Edge: Single element safe",
          .description = "One floor and it is safe",
          .data = {0},
          .target = 0,
      },
      {
          .label = "Edge: Single element breaks",
          .description = "One floor and it breaks",
          .data = {1},
          .target = 0,
      },
  };
}

inline auto get_test_cases(const std::string& algo_name)
    -> std::vector<TestCase> {
  if (algo_name == "linear_search")
    return linear_search_test_cases();
  if (algo_name == "binary_search")
    return binary_search_test_cases();
  if (algo_name == "bubble_sort")
    return bubble_sort_test_cases();
  if (algo_name == "two_crystal_balls")
    return two_crystal_balls_test_cases();
  return {};
}

inline auto singly_linked_list_test_cases() -> std::vector<ListTestCase> {
  return {
      {
          .label = "Prepend to non-empty list",
          .description = "Prepend 99 to [3, 7, 12, 5]",
          .initial_values = {3, 7, 12, 5},
          .op = ListOp::kPrepend,
          .value = 99,
          .index = 0,
      },
      {
          .label = "Append to non-empty list",
          .description = "Append 99 to [3, 7, 12]",
          .initial_values = {3, 7, 12},
          .op = ListOp::kAppend,
          .value = 99,
          .index = 0,
      },
      {
          .label = "Insert into middle",
          .description = "Insert 99 at index 2 in [3, 7, 12, 5, 9]",
          .initial_values = {3, 7, 12, 5, 9},
          .op = ListOp::kInsertAt,
          .value = 99,
          .index = 2,
      },
      {
          .label = "Insert at head (idx=0)",
          .description = "Insert 99 at index 0 in [3, 7, 12]",
          .initial_values = {3, 7, 12},
          .op = ListOp::kInsertAt,
          .value = 99,
          .index = 0,
      },
      {
          .label = "Remove from middle",
          .description = "Remove index 2 from [3, 7, 12, 5, 9]",
          .initial_values = {3, 7, 12, 5, 9},
          .op = ListOp::kRemoveAt,
          .value = 0,
          .index = 2,
      },
      {
          .label = "Remove head",
          .description = "Remove index 0 from [3, 7, 12]",
          .initial_values = {3, 7, 12},
          .op = ListOp::kRemoveAt,
          .value = 0,
          .index = 0,
      },
      {
          .label = "Get middle element",
          .description = "Get index 3 from [3, 7, 12, 5, 9]",
          .initial_values = {3, 7, 12, 5, 9},
          .op = ListOp::kGet,
          .value = 0,
          .index = 3,
      },
      {
          .label = "Prepend to empty list",
          .description = "Prepend 42 to empty list",
          .initial_values = {},
          .op = ListOp::kPrepend,
          .value = 42,
          .index = 0,
      },
  };
}

inline auto doubly_linked_list_test_cases() -> std::vector<ListTestCase> {
  return {
      {
          .label = "Prepend to non-empty list",
          .description = "Prepend 99 to [3, 7, 12, 5]",
          .initial_values = {3, 7, 12, 5},
          .op = ListOp::kPrepend,
          .value = 99,
          .index = 0,
      },
      {
          .label = "Append to non-empty list",
          .description = "Append 99 to [3, 7, 12]",
          .initial_values = {3, 7, 12},
          .op = ListOp::kAppend,
          .value = 99,
          .index = 0,
      },
      {
          .label = "Insert into middle",
          .description = "Insert 99 at index 2 in [3, 7, 12, 5, 9]",
          .initial_values = {3, 7, 12, 5, 9},
          .op = ListOp::kInsertAt,
          .value = 99,
          .index = 2,
      },
      {
          .label = "Insert at head (idx=0)",
          .description = "Insert 99 at index 0 in [3, 7, 12]",
          .initial_values = {3, 7, 12},
          .op = ListOp::kInsertAt,
          .value = 99,
          .index = 0,
      },
      {
          .label = "Remove from middle",
          .description = "Remove index 2 from [3, 7, 12, 5, 9]",
          .initial_values = {3, 7, 12, 5, 9},
          .op = ListOp::kRemoveAt,
          .value = 0,
          .index = 2,
      },
      {
          .label = "Remove head",
          .description = "Remove index 0 from [3, 7, 12]",
          .initial_values = {3, 7, 12},
          .op = ListOp::kRemoveAt,
          .value = 0,
          .index = 0,
      },
      {
          .label = "Get middle element",
          .description = "Get index 3 from [3, 7, 12, 5, 9]",
          .initial_values = {3, 7, 12, 5, 9},
          .op = ListOp::kGet,
          .value = 0,
          .index = 3,
      },
      {
          .label = "Prepend to empty list",
          .description = "Prepend 42 to empty list",
          .initial_values = {},
          .op = ListOp::kPrepend,
          .value = 42,
          .index = 0,
      },
  };
}

inline auto get_list_test_cases(const std::string& algo_name)
    -> std::vector<ListTestCase> {
  if (algo_name == "singly_linked_list")
    return singly_linked_list_test_cases();
  if (algo_name == "doubly_linked_list")
    return doubly_linked_list_test_cases();
  return {};
}

struct StackQueueTestCase {
  std::string label;
  std::string description;
  std::vector<int> initial_values;
  ListOp op;
  int value = 0;
};

inline auto stack_test_cases() -> std::vector<StackQueueTestCase> {
  return {
      {
          .label = "Push onto non-empty",
          .description = "Push 10 onto [5, 3, 8, 1, 6]",
          .initial_values = {5, 3, 8, 1, 6},
          .op = ListOp::kPush,
          .value = 10,
      },
      {
          .label = "Push onto empty",
          .description = "Push 42 onto an empty stack",
          .initial_values = {},
          .op = ListOp::kPush,
          .value = 42,
      },
      {
          .label = "Pop from non-empty",
          .description = "Pop top from [5, 3, 8, 1, 6]",
          .initial_values = {5, 3, 8, 1, 6},
          .op = ListOp::kPop,
          .value = 0,
      },
      {
          .label = "Pop from empty (error)",
          .description = "Pop from empty stack — shows error",
          .initial_values = {},
          .op = ListOp::kPop,
          .value = 0,
      },
  };
}

inline auto queue_test_cases() -> std::vector<StackQueueTestCase> {
  return {
      {
          .label = "Enqueue onto non-empty",
          .description = "Enqueue 10 onto [5, 3, 8, 1, 6]",
          .initial_values = {5, 3, 8, 1, 6},
          .op = ListOp::kEnqueue,
          .value = 10,
      },
      {
          .label = "Enqueue onto empty",
          .description = "Enqueue 42 onto an empty queue",
          .initial_values = {},
          .op = ListOp::kEnqueue,
          .value = 42,
      },
      {
          .label = "Dequeue from non-empty",
          .description = "Dequeue front from [5, 3, 8, 1, 6]",
          .initial_values = {5, 3, 8, 1, 6},
          .op = ListOp::kDequeue,
          .value = 0,
      },
      {
          .label = "Dequeue from empty (error)",
          .description = "Dequeue from empty queue — shows error",
          .initial_values = {},
          .op = ListOp::kDequeue,
          .value = 0,
      },
  };
}

inline auto get_stack_queue_test_cases(bool is_stack)
    -> std::vector<StackQueueTestCase> {
  return is_stack ? stack_test_cases() : queue_test_cases();
}

struct RingBufferTestCase {
  std::string label;
  std::string description;
  int capacity = 8;
  std::vector<int> initial_values;
  RingBufferOp op;
  int value = 0;
};

inline auto ring_buffer_test_cases() -> std::vector<RingBufferTestCase> {
  return {
      {
          .label = "Enqueue onto non-empty",
          .description = "Enqueue 10 into cap=8 buffer with [5, 3, 8]",
          .capacity = 8,
          .initial_values = {5, 3, 8},
          .op = RingBufferOp::kEnqueue,
          .value = 10,
      },
      {
          .label = "Enqueue onto empty",
          .description = "Enqueue 42 into empty cap=8 buffer",
          .capacity = 8,
          .initial_values = {},
          .op = RingBufferOp::kEnqueue,
          .value = 42,
      },
      {
          .label = "Enqueue onto full (error)",
          .description = "Enqueue into full cap=4 buffer — shows error",
          .capacity = 4,
          .initial_values = {1, 2, 3, 4},
          .op = RingBufferOp::kEnqueue,
          .value = 99,
      },
      {
          .label = "Dequeue from non-empty",
          .description = "Dequeue head from cap=8 buffer with [5, 3, 8, 1, 6]",
          .capacity = 8,
          .initial_values = {5, 3, 8, 1, 6},
          .op = RingBufferOp::kDequeue,
          .value = 0,
      },
      {
          .label = "Dequeue from empty (error)",
          .description = "Dequeue from empty buffer — shows error",
          .capacity = 8,
          .initial_values = {},
          .op = RingBufferOp::kDequeue,
          .value = 0,
      },
  };
}

} // namespace viz
