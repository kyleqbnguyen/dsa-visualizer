#pragma once

#include <string>
#include <vector>

namespace viz {

enum class ListOp { kPrepend, kAppend, kInsertAt, kRemoveAt, kGet };

enum class ListNodeState {
  kNormal,
  kActive,
  kNew,
  kTarget,
  kFound,
  kRemoved,
};

struct ListNodeSnap {
  int value = 0;
  ListNodeState state = ListNodeState::kNormal;
  bool has_next = false;
  bool has_prev = false;
};

struct ListStepSnapshot {
  std::vector<ListNodeSnap> nodes;

  std::string status_text;
  std::string trace_entry;
  int current_line = -1;
  std::vector<std::pair<std::string, std::string>> variables;
};

struct ListAlgorithmRecording {
  std::string title;
  std::string algorithm_name;
  ListOp op;
  std::vector<ListStepSnapshot> steps;
};

} // namespace viz
