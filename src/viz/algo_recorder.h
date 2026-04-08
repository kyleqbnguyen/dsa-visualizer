#pragma once

#include "code_panel.h"
#include "snapshot.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <string>
#include <vector>

namespace viz {

inline auto record_linear_search(const std::vector<int>& input, int target)
    -> AlgorithmRecording {
  AlgorithmRecording rec;
  rec.title = "Linear Search";
  rec.algorithm_name = "linear_search";

  {
    StepSnapshot init;
    init.data = input;
    init.status_text =
        "Target: " + std::to_string(target) + "  |  Ready to start.";
    init.current_line = 0;
    init.trace_entry = "begin linear_search(target=" + std::to_string(target) +
                       ", n=" + std::to_string(input.size()) + ")";
    rec.steps.push_back(std::move(init));
  }

  bool was_found = false;
  int found_at = -1;

  for (std::size_t idx = 0; idx < input.size(); ++idx) {
    bool found = input[idx] == target;

    StepSnapshot snap;
    snap.data = input;
    snap.highlight_a = static_cast<int>(idx);

    snap.variables = {
        {"i", std::to_string(idx)},
        {"data[i]", std::to_string(input[idx])},
        {"target", std::to_string(target)},
    };

    if (found) {
      snap.found_index = static_cast<int>(idx);
      snap.current_line = linear_search_line(true, false);
      snap.status_text = "Found " + std::to_string(target) + " at index " +
                         std::to_string(idx) + "!";
      snap.trace_entry = "compare(data[" + std::to_string(idx) +
                         "]=" + std::to_string(input[idx]) +
                         ", target=" + std::to_string(target) + ") -> MATCH";
      was_found = true;
      found_at = static_cast<int>(idx);
      rec.steps.push_back(std::move(snap));
      break;
    }

    snap.current_line = linear_search_line(false, false);
    snap.status_text = "Checking index " + std::to_string(idx) +
                       " -> value=" + std::to_string(input[idx]);
    snap.trace_entry = "compare(data[" + std::to_string(idx) +
                       "]=" + std::to_string(input[idx]) +
                       ", target=" + std::to_string(target) + ") -> skip";
    rec.steps.push_back(std::move(snap));
  }

  {
    StepSnapshot final_snap;
    final_snap.data = input;
    final_snap.current_line = linear_search_line(was_found, true);
    if (was_found) {
      final_snap.found_index = found_at;
      final_snap.status_text = "Result: found " + std::to_string(target) +
                               " at index " + std::to_string(found_at);
      final_snap.trace_entry = "return " + std::to_string(found_at);
    } else {
      final_snap.status_text =
          "Result: " + std::to_string(target) + " not found in the array.";
      final_snap.trace_entry = "return nullopt";
    }
    final_snap.variables = {
        {"result", was_found ? std::to_string(found_at) : "nullopt"},
        {"target", std::to_string(target)},
    };
    rec.steps.push_back(std::move(final_snap));
  }

  return rec;
}

inline auto record_binary_search(std::vector<int> input, int target)
    -> AlgorithmRecording {
  std::sort(input.begin(), input.end());

  AlgorithmRecording rec;
  rec.title = "Binary Search";
  rec.algorithm_name = "binary_search";

  {
    StepSnapshot init;
    init.data = input;
    init.low = 0;
    init.high = static_cast<int>(input.size());
    init.status_text =
        "Target: " + std::to_string(target) + "  |  Ready to start.";
    init.current_line = 1;
    init.trace_entry = "begin binary_search(target=" + std::to_string(target) +
                       ", n=" + std::to_string(input.size()) + ")";
    init.variables = {
        {"low", "0"},
        {"high", std::to_string(input.size())},
        {"target", std::to_string(target)},
    };
    rec.steps.push_back(std::move(init));
  }

  bool was_found = false;
  int found_at = -1;

  std::size_t low = 0;
  std::size_t high = input.size();
  while (low < high) {
    std::size_t mid = low + (high - low) / 2;
    bool found = input[mid] == target;
    bool val_lt = input[mid] < target;

    StepSnapshot snap;
    snap.data = input;
    snap.low = static_cast<int>(low);
    snap.high = static_cast<int>(high);
    snap.highlight_a = static_cast<int>(mid);

    snap.variables = {
        {"low", std::to_string(low)},
        {"high", std::to_string(high)},
        {"mid", std::to_string(mid)},
        {"data[mid]", std::to_string(input[mid])},
        {"target", std::to_string(target)},
    };

    if (found) {
      snap.found_index = static_cast<int>(mid);
      snap.current_line = binary_search_line(true, false, false);
      snap.status_text = "Found " + std::to_string(target) + " at index " +
                         std::to_string(mid) + "!";
      snap.trace_entry = "mid=" + std::to_string(mid) +
                         "  data[mid]=" + std::to_string(input[mid]) +
                         " == target -> FOUND";
      was_found = true;
      found_at = static_cast<int>(mid);
      rec.steps.push_back(std::move(snap));
      break;
    }

    snap.current_line = binary_search_line(false, val_lt, false);
    snap.status_text = "Window [" + std::to_string(low) + ", " +
                       std::to_string(high) + ")  mid=" + std::to_string(mid) +
                       "  val=" + std::to_string(input[mid]);
    if (val_lt) {
      snap.trace_entry = "mid=" + std::to_string(mid) +
                         "  data[mid]=" + std::to_string(input[mid]) +
                         " < target -> low = mid+1";
      low = mid + 1;
    } else {
      snap.trace_entry = "mid=" + std::to_string(mid) +
                         "  data[mid]=" + std::to_string(input[mid]) +
                         " > target -> high = mid";
      high = mid;
    }

    rec.steps.push_back(std::move(snap));
  }

  {
    StepSnapshot final_snap;
    final_snap.data = input;
    final_snap.current_line = binary_search_line(was_found, false, true);
    if (was_found) {
      final_snap.found_index = found_at;
      final_snap.status_text = "Result: found " + std::to_string(target) +
                               " at index " + std::to_string(found_at);
      final_snap.trace_entry = "return " + std::to_string(found_at);
    } else {
      final_snap.status_text =
          "Result: " + std::to_string(target) + " not found in the array.";
      final_snap.trace_entry = "return nullopt";
    }
    final_snap.variables = {
        {"result", was_found ? std::to_string(found_at) : "nullopt"},
        {"target", std::to_string(target)},
    };
    rec.steps.push_back(std::move(final_snap));
  }

  return rec;
}

inline auto record_bubble_sort(const std::vector<int>& input)
    -> AlgorithmRecording {
  AlgorithmRecording rec;
  rec.title = "Bubble Sort";
  rec.algorithm_name = "bubble_sort";

  auto data = input;

  {
    StepSnapshot init;
    init.data = data;
    init.status_text = "Ready to start.";
    init.current_line = 0;
    init.trace_entry =
        "begin bubble_sort(n=" + std::to_string(data.size()) + ")";
    init.variables = {
        {"n", std::to_string(data.size())},
        {"pass", "0"},
        {"swaps", "0"},
    };
    rec.steps.push_back(std::move(init));
  }

  int total_swaps = 0;
  int passes = 0;

  if (!data.empty()) {
    for (std::size_t i = 0; i < data.size(); ++i) {
      bool swapped = false;
      ++passes;
      for (std::size_t j = 0; j + 1 < data.size() - i; ++j) {
        bool will_swap = data[j] > data[j + 1];

        StepSnapshot snap;
        snap.data = data;
        snap.highlight_a = static_cast<int>(j);
        snap.highlight_b = static_cast<int>(j + 1);
        snap.sorted_boundary = static_cast<int>(data.size() - i);

        snap.variables = {
            {"pass", std::to_string(passes)},
            {"j", std::to_string(j)},
            {"data[j]", std::to_string(data[j])},
            {"data[j+1]", std::to_string(data[j + 1])},
            {"swaps", std::to_string(total_swaps)},
        };

        snap.current_line = bubble_sort_line(will_swap, false);

        if (will_swap) {
          snap.status_text = "Swap index " + std::to_string(j) + " (" +
                             std::to_string(data[j]) + ") <-> " +
                             std::to_string(j + 1) + " (" +
                             std::to_string(data[j + 1]) + ")";
          snap.trace_entry = "swap(data[" + std::to_string(j) +
                             "]=" + std::to_string(data[j]) + ", data[" +
                             std::to_string(j + 1) +
                             "]=" + std::to_string(data[j + 1]) + ")";
          std::swap(data[j], data[j + 1]);
          ++total_swaps;
          swapped = true;
          snap.data = data;
        } else {
          snap.status_text = "Compare index " + std::to_string(j) + " (" +
                             std::to_string(data[j]) + ") and " +
                             std::to_string(j + 1) + " (" +
                             std::to_string(data[j + 1]) + ") -> no swap";
          snap.trace_entry = "compare(data[" + std::to_string(j) +
                             "]=" + std::to_string(data[j]) + ", data[" +
                             std::to_string(j + 1) +
                             "]=" + std::to_string(data[j + 1]) + ") -> skip";
        }

        snap.variables[4].second = std::to_string(total_swaps);
        rec.steps.push_back(std::move(snap));
      }
      if (!swapped) {
        break;
      }
    }
  }

  {
    StepSnapshot final_snap;
    final_snap.data = data;
    final_snap.sorted_boundary = 0;
    final_snap.current_line = bubble_sort_line(false, true);
    final_snap.status_text = "Bubble Sort complete! (" +
                             std::to_string(total_swaps) + " total swaps)";
    final_snap.trace_entry = "done - sorted in " + std::to_string(passes) +
                             " passes, " + std::to_string(total_swaps) +
                             " swaps";
    final_snap.variables = {
        {"passes", std::to_string(passes)},
        {"total_swaps", std::to_string(total_swaps)},
    };
    rec.steps.push_back(std::move(final_snap));
  }

  return rec;
}

inline auto record_two_crystal_balls(const std::vector<int>& input)
    -> AlgorithmRecording {
  AlgorithmRecording rec;
  rec.title = "Two Crystal Balls";
  rec.algorithm_name = "two_crystal_balls";

  if (input.empty()) {
    StepSnapshot empty;
    empty.data = input;
    empty.current_line = two_crystal_balls_line(true, false, true);
    empty.status_text = "Result: empty input (no breaking floor).";
    empty.trace_entry = "return -1";
    empty.variables = {
        {"result", "-1"},
    };
    rec.steps.push_back(std::move(empty));
    return rec;
  }

  std::size_t jump = std::max<std::size_t>(
      1, static_cast<std::size_t>(std::sqrt(input.size())));

  {
    StepSnapshot init;
    init.data = input;
    init.current_line = 0;
    init.status_text = "Ready to find first breaking floor.";
    init.trace_entry =
        "begin two_crystal_balls(n=" + std::to_string(input.size()) + ")";
    init.variables = {
        {"jump", std::to_string(jump)},
        {"i", std::to_string(jump)},
    };
    rec.steps.push_back(std::move(init));
  }

  int found_at = -1;
  std::size_t i = jump;
  for (; i < input.size(); i += jump) {
    StepSnapshot snap;
    snap.data = input;
    snap.highlight_a = static_cast<int>(i);
    snap.current_line = two_crystal_balls_line(true, false, false);
    snap.status_text = "Drop first ball at index " + std::to_string(i) +
                       " -> value=" + std::to_string(input[i]);
    snap.trace_entry = "jump-check breaks[" + std::to_string(i) +
                       "]=" + std::to_string(input[i]);
    snap.variables = {
        {"jump", std::to_string(jump)},
        {"i", std::to_string(i)},
    };
    rec.steps.push_back(std::move(snap));

    if (input[i] > 0) {
      break;
    }
  }

  std::size_t start = (i >= jump) ? i - jump : 0;
  std::size_t end = std::min(i, input.size() - 1);

  for (std::size_t j = start; j <= end; ++j) {
    bool found = input[j] > 0;

    StepSnapshot snap;
    snap.data = input;
    snap.highlight_a = static_cast<int>(j);
    snap.current_line = two_crystal_balls_line(false, found, false);
    snap.status_text = "Linear scan index " + std::to_string(j) +
                       " -> value=" + std::to_string(input[j]);
    snap.trace_entry = "linear-check breaks[" + std::to_string(j) +
                       "]=" + std::to_string(input[j]);
    snap.variables = {
        {"jump", std::to_string(jump)},
        {"start", std::to_string(start)},
        {"end", std::to_string(end)},
        {"j", std::to_string(j)},
    };

    if (found) {
      found_at = static_cast<int>(j);
      snap.found_index = found_at;
      snap.status_text =
          "Found first breaking index at " + std::to_string(found_at) + "!";
      snap.trace_entry = "return " + std::to_string(found_at);
      rec.steps.push_back(std::move(snap));
      break;
    }

    rec.steps.push_back(std::move(snap));
  }

  {
    StepSnapshot final_snap;
    final_snap.data = input;
    final_snap.current_line = two_crystal_balls_line(false, false, true);
    if (found_at >= 0) {
      final_snap.found_index = found_at;
      final_snap.status_text =
          "Result: first breaking floor at index " + std::to_string(found_at);
      final_snap.trace_entry = "return " + std::to_string(found_at);
    } else {
      final_snap.status_text = "Result: no breaking floor found.";
      final_snap.trace_entry = "return -1";
    }
    final_snap.variables = {
        {"result", std::to_string(found_at)},
    };
    rec.steps.push_back(std::move(final_snap));
  }

  return rec;
}

} // namespace viz
