#pragma once

#include "graph_recorder.h"
#include "heap_snapshot.h"
#include "list_snapshot.h"
#include "lru_snapshot.h"
#include "maze_snapshot.h"
#include "ring_buffer_snapshot.h"
#include "tree_snapshot.h"
#include "trie_snapshot.h"

#include <string>
#include <utility>
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

inline auto quick_sort_test_cases() -> std::vector<TestCase> {
  return {
      {
          .label = "Typical: Random array",
          .description = "15 random unsorted elements",
          .data = {38, 27, 43, 3, 9, 82, 10, 55, 14, 36, 22, 68, 1, 47, 5},
          .target = 0,
      },
      {
          .label = "Worst: Reverse sorted",
          .description = "Completely reversed — many swaps and poor pivot choices",
          .data = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
          .target = 0,
      },
      {
          .label = "Edge: Already sorted",
          .description = "Already in order — shows pivot placement with no swaps",
          .data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
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
          .description = "All elements identical — pivot at each rightmost position",
          .data = {7, 7, 7, 7, 7, 7, 7, 7},
          .target = 0,
      },
      {
          .label = "Edge: Two elements",
          .description = "Minimal interesting case",
          .data = {20, 10},
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
  if (algo_name == "quick_sort")
    return quick_sort_test_cases();
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

// ──────────────────────────────────────────
//  Heap
// ──────────────────────────────────────────

struct HeapTestCase {
  std::string label;
  std::string description;
  std::vector<int> initial_values;
  HeapOp op;
  int value = 0;
};

inline auto heap_test_cases() -> std::vector<HeapTestCase> {
  return {
      {
          .label = "Insert: multiple bubble-ups",
          .description = "Insert 1 into [10, 5, 20, 8, 15] — bubbles to root",
          .initial_values = {3, 5, 8, 10, 20},
          .op = HeapOp::kInsert,
          .value = 1,
      },
      {
          .label = "Insert: no swap needed",
          .description = "Insert 25 — already in order",
          .initial_values = {3, 5, 8, 10, 20},
          .op = HeapOp::kInsert,
          .value = 25,
      },
      {
          .label = "Delete min: small heap",
          .description = "Delete min from [3, 5, 8, 10, 20]",
          .initial_values = {3, 5, 8, 10, 20},
          .op = HeapOp::kDeleteMin,
          .value = 0,
      },
      {
          .label = "Delete min: deep sink",
          .description = "Delete min from [1, 5, 2, 10, 8, 3, 4]",
          .initial_values = {1, 5, 2, 10, 8, 3, 4},
          .op = HeapOp::kDeleteMin,
          .value = 0,
      },
      {
          .label = "Build heap: reverse sorted",
          .description = "Build from [20, 15, 10, 8, 5, 3, 1]",
          .initial_values = {20, 15, 10, 8, 5, 3, 1},
          .op = HeapOp::kBuildHeap,
          .value = 0,
      },
  };
}

// ──────────────────────────────────────────
//  Tree
// ──────────────────────────────────────────

struct TreeTestCase {
  std::string label;
  std::string description;
  std::vector<int> tree_data;
  TreeOp op;
  int value = 0;
};

inline auto tree_test_cases() -> std::vector<TreeTestCase> {
  return {
      {
          .label = "Pre-order: balanced BST",
          .description = "Pre-order on 7-node balanced BST",
          .tree_data = {8, 4, 12, 2, 6, 10, 14},
          .op = TreeOp::kPreorder,
      },
      {
          .label = "In-order: balanced BST",
          .description = "In-order (sorted output) on balanced BST",
          .tree_data = {8, 4, 12, 2, 6, 10, 14},
          .op = TreeOp::kInorder,
      },
      {
          .label = "Post-order: balanced BST",
          .description = "Post-order on balanced BST",
          .tree_data = {8, 4, 12, 2, 6, 10, 14},
          .op = TreeOp::kPostorder,
      },
      {
          .label = "BFS: balanced BST",
          .description = "Level-order traversal",
          .tree_data = {8, 4, 12, 2, 6, 10, 14},
          .op = TreeOp::kBfs,
      },
      {
          .label = "BST Find: existing value",
          .description = "Find 6 in balanced BST",
          .tree_data = {8, 4, 12, 2, 6, 10, 14},
          .op = TreeOp::kBstFind,
          .value = 6,
      },
      {
          .label = "BST Find: missing value",
          .description = "Find 99 — not in tree",
          .tree_data = {8, 4, 12, 2, 6, 10, 14},
          .op = TreeOp::kBstFind,
          .value = 99,
      },
      {
          .label = "BST Insert: new leaf",
          .description = "Insert 5 into balanced BST",
          .tree_data = {8, 4, 12, 2, 6, 10, 14},
          .op = TreeOp::kBstInsert,
          .value = 5,
      },
      {
          .label = "BST Delete: leaf node",
          .description = "Delete 2 (leaf) from BST",
          .tree_data = {8, 4, 12, 2, 6, 10, 14},
          .op = TreeOp::kBstDelete,
          .value = 2,
      },
      {
          .label = "BST Delete: two children",
          .description = "Delete 4 (has two children)",
          .tree_data = {8, 4, 12, 2, 6, 10, 14},
          .op = TreeOp::kBstDelete,
          .value = 4,
      },
  };
}

// ──────────────────────────────────────────
//  Maze
// ──────────────────────────────────────────

struct MazeTestCase {
  std::string label;
  std::string description;
  std::vector<std::string> grid;
  int start_row = 0;
  int start_col = 0;
  int end_row = 0;
  int end_col = 0;
};

inline auto maze_test_cases() -> std::vector<MazeTestCase> {
  return {
      {
          .label = "Small solvable",
          .description = "8x10 maze with a clear solution path",
          .grid =
              {
                  "S.....#...",
                  ".#.#..#...",
                  ".#....#...",
                  ".###.##...",
                  "......#...",
                  ".#.####...",
                  ".#........",
                  ".####.###.",
                  "........#E",
              },
          .start_row = 0,
          .start_col = 0,
          .end_row = 8,
          .end_col = 9,
      },
      {
          .label = "Heavy backtracking",
          .description = "Maze requiring many dead-end backtracks",
          .grid =
              {
                  "S.#.......",
                  ".##.#####.",
                  "....#...#.",
                  "###.#.#.#.",
                  "..#...#...",
                  ".####.###.",
                  ".........E",
              },
          .start_row = 0,
          .start_col = 0,
          .end_row = 6,
          .end_col = 9,
      },
      {
          .label = "Unsolvable",
          .description = "Start and end are disconnected by walls",
          .grid =
              {
                  "S..#....",
                  "...#....",
                  "...#....",
                  "########",
                  "....#..E",
              },
          .start_row = 0,
          .start_col = 0,
          .end_row = 4,
          .end_col = 7,
      },
      {
          .label = "Straight corridor",
          .description = "Trivial single-row maze",
          .grid = {"S.......E"},
          .start_row = 0,
          .start_col = 0,
          .end_row = 0,
          .end_col = 8,
      },
  };
}

// ──────────────────────────────────────────
//  LRU Cache
// ──────────────────────────────────────────

struct LruTestCase {
  std::string label;
  std::string description;
  std::vector<std::pair<int, int>> initial_entries;
  int capacity = 4;
  LruOp op;
  int key = 0;
  int value = 0;
};

inline auto lru_test_cases() -> std::vector<LruTestCase> {
  return {
      {
          .label = "Get hit (move to head)",
          .description = "Get key 2 — moves to MRU position",
          .initial_entries = {{1, 10}, {2, 20}, {3, 30}},
          .capacity = 4,
          .op = LruOp::kGet,
          .key = 2,
      },
      {
          .label = "Get miss",
          .description = "Get key 99 — cache miss",
          .initial_entries = {{1, 10}, {2, 20}, {3, 30}},
          .capacity = 4,
          .op = LruOp::kGet,
          .key = 99,
      },
      {
          .label = "Put new (no eviction)",
          .description = "Put key 4 — space available",
          .initial_entries = {{1, 10}, {2, 20}, {3, 30}},
          .capacity = 4,
          .op = LruOp::kPut,
          .key = 4,
          .value = 40,
      },
      {
          .label = "Put new (eviction)",
          .description = "Put key 4 — evicts LRU (key 1)",
          .initial_entries = {{1, 10}, {2, 20}, {3, 30}},
          .capacity = 3,
          .op = LruOp::kPut,
          .key = 4,
          .value = 40,
      },
      {
          .label = "Put existing (update)",
          .description = "Put key 2 with new value — moves to head",
          .initial_entries = {{1, 10}, {2, 20}, {3, 30}},
          .capacity = 4,
          .op = LruOp::kPut,
          .key = 2,
          .value = 99,
      },
  };
}

// ──────────────────────────────────────────
//  Graph
// ──────────────────────────────────────────

struct GraphPreset {
  std::string label;
  std::string description;
  GraphDef def;
};

inline auto graph_presets() -> std::vector<GraphPreset> {
  std::vector<GraphPreset> presets;

  {
    GraphPreset p;
    p.label = "6-node undirected";
    p.description = "Simple undirected graph — 6 nodes, connected";
    p.def.num_nodes = 6;
    p.def.labels = {"A", "B", "C", "D", "E", "F"};
    p.def.positions = {{10, 5}, {30, 5}, {50, 5}, {10, 15}, {30, 15}, {50, 15}};
    p.def.directed = false;
    p.def.edges = {{0, 1, 1}, {1, 2, 1}, {0, 3, 1},
                   {1, 4, 1}, {2, 5, 1}, {3, 4, 1}, {4, 5, 1}};
    presets.push_back(std::move(p));
  }

  {
    GraphPreset p;
    p.label = "Disconnected";
    p.description = "Graph with unreachable nodes";
    p.def.num_nodes = 6;
    p.def.labels = {"A", "B", "C", "D", "E", "F"};
    p.def.positions = {{10, 5}, {30, 5}, {50, 5}, {10, 15}, {30, 15}, {50, 15}};
    p.def.directed = false;
    p.def.edges = {{0, 1, 1}, {1, 2, 1}, {3, 4, 1}};
    presets.push_back(std::move(p));
  }

  {
    GraphPreset p;
    p.label = "Weighted (Dijkstra)";
    p.description = "Weighted graph — non-obvious shortest path";
    p.def.num_nodes = 6;
    p.def.labels = {"A", "B", "C", "D", "E", "F"};
    p.def.positions = {{10, 5}, {30, 2}, {50, 5}, {10, 15}, {30, 18}, {50, 15}};
    p.def.directed = false;
    p.def.edges = {{0, 1, 4}, {0, 3, 2}, {1, 2, 3}, {1, 4, 3},
                   {3, 4, 3}, {2, 5, 2}, {4, 5, 1}};
    presets.push_back(std::move(p));
  }

  {
    GraphPreset p;
    p.label = "Directed";
    p.description = "Directed graph for DFS traversal order";
    p.def.num_nodes = 6;
    p.def.labels = {"A", "B", "C", "D", "E", "F"};
    p.def.positions = {{10, 5}, {30, 5}, {50, 5}, {10, 15}, {30, 15}, {50, 15}};
    p.def.directed = true;
    p.def.edges = {{0, 1, 1}, {0, 3, 1}, {1, 2, 1},
                   {1, 4, 1}, {3, 4, 1}, {4, 5, 1}};
    presets.push_back(std::move(p));
  }

  return presets;
}

// ──────────────────────────────────────────
//  Trie
// ──────────────────────────────────────────

struct TrieTestCase {
  std::string label;
  std::string description;
  std::vector<std::string> dictionary;
  TrieVizOp op;
  std::string target_word;
};

inline auto trie_test_cases() -> std::vector<TrieTestCase> {
  return {
      {
          .label = "Insert: shared prefix",
          .description = "Insert \"bad\" into dict with \"bat\",\"ball\"",
          .dictionary = {"app", "apple", "ape", "bat", "ball"},
          .op = TrieVizOp::kInsert,
          .target_word = "bad",
      },
      {
          .label = "Insert: prefix of existing",
          .description = "Insert \"app\" (already a prefix of \"apple\")",
          .dictionary = {"apple", "ape"},
          .op = TrieVizOp::kInsert,
          .target_word = "app",
      },
      {
          .label = "Search: word exists",
          .description = "Search for \"apple\" in dictionary",
          .dictionary = {"app", "apple", "ape", "bat", "ball"},
          .op = TrieVizOp::kSearch,
          .target_word = "apple",
      },
      {
          .label = "Search: prefix only",
          .description = "Search \"ap\" — prefix exists but not a word",
          .dictionary = {"app", "apple", "ape"},
          .op = TrieVizOp::kSearch,
          .target_word = "ap",
      },
      {
          .label = "Delete: leaf word",
          .description = "Delete \"ape\" — prunes leaf nodes",
          .dictionary = {"app", "apple", "ape", "bat"},
          .op = TrieVizOp::kDelete,
          .target_word = "ape",
      },
      {
          .label = "Delete: shared prefix",
          .description = "Delete \"app\" — keeps \"apple\" intact",
          .dictionary = {"app", "apple"},
          .op = TrieVizOp::kDelete,
          .target_word = "app",
      },
  };
}

} // namespace viz
