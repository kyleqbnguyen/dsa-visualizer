#pragma once

#include <string>
#include <vector>

namespace viz {

enum class TreeOp {
  kNone,
  kPreorder,
  kInorder,
  kPostorder,
  kBfs,
  kBstFind,
  kBstInsert,
  kBstDelete,
  kCompare,
};

enum class TreeNodeState {
  kNormal,
  kActive,
  kVisiting,
  kVisited,
  kFound,
  kInserted,
  kDeleted,
  kNull,
};

struct TreeNodeSnap {
  int value = 0;
  int left = -1;
  int right = -1;
  int parent = -1;
  TreeNodeState state = TreeNodeState::kNormal;
};

struct TreeStepSnapshot {
  std::vector<TreeNodeSnap> nodes;
  int root = 0;

  std::vector<int> visit_order;

  std::string status_text;
  std::string trace_entry;
  int current_line = -1;
  std::vector<std::pair<std::string, std::string>> variables;
};

struct TreeRecording {
  std::string title;
  std::string algorithm_name;
  TreeOp op = TreeOp::kNone;
  std::vector<TreeStepSnapshot> steps;
};

} // namespace viz
