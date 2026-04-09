#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace viz {

struct StepSnapshot {
  std::vector<int> data;

  int highlight_a = -1;
  int highlight_b = -1;
  int found_index = -1;

  int low = -1;
  int high = -1;

  int pivot_index = -1;
  int sorted_boundary = -1;

  int current_line = -1;

  std::string status_text;
  std::string trace_entry;
  std::vector<std::pair<std::string, std::string>> variables;
};

struct AlgorithmRecording {
  std::string title;
  std::string algorithm_name;
  std::vector<StepSnapshot> steps;
};

} // namespace viz
