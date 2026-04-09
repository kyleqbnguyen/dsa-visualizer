#pragma once

#include <string>
#include <vector>

namespace viz {

enum class HeapOp {
  kNone,
  kInsert,
  kDeleteMin,
  kBuildHeap,
};

enum class HeapNodeState { kNormal, kActive, kCompare, kSwapped, kSorted };

struct HeapStepSnapshot {
  std::vector<int> data;
  int highlight_a = -1;
  int highlight_b = -1;
  int boundary = -1;

  std::string status_text;
  std::string trace_entry;
  int current_line = -1;
  std::vector<std::pair<std::string, std::string>> variables;
};

struct HeapRecording {
  std::string title;
  std::string algorithm_name = "min_heap";
  std::vector<HeapStepSnapshot> steps;
};

} // namespace viz
