#pragma once

#include <algorithm>
#include <span>
#include <string>
#include <vector>

#include "binary_search.h"
#include "bubble_sort.h"
#include "code_panel.h"
#include "linear_search.h"
#include "snapshot.h"

namespace viz {

inline auto record_linear_search(const std::vector<int> &input, int target)
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

  auto data_copy = input;
  bool was_found = false;
  int found_at = -1;

  dsa::linear_search<int>(
      std::span<const int>(data_copy), target,
      [&](std::span<const int>, std::size_t idx, const int & /*tgt*/,
          bool found) {
        StepSnapshot snap;
        snap.data = data_copy;
        snap.highlight_a = static_cast<int>(idx);
        snap.highlight_b = -1;

        snap.variables = {
            {"i", std::to_string(idx)},
            {"data[i]", std::to_string(data_copy[idx])},
            {"target", std::to_string(target)},
        };

        if (found) {
          snap.found_index = static_cast<int>(idx);
          snap.current_line = linear_search_line(true, false);
          snap.status_text = "Found " + std::to_string(target) + " at index " +
                             std::to_string(idx) + "!";
          snap.trace_entry = "compare(data[" + std::to_string(idx) +
                             "]=" + std::to_string(data_copy[idx]) +
                             ", target=" + std::to_string(target) +
                             ") -> MATCH";
          was_found = true;
          found_at = static_cast<int>(idx);
        } else {
          snap.current_line = linear_search_line(false, false);
          snap.status_text = "Checking index " + std::to_string(idx) +
                             " -> value=" + std::to_string(data_copy[idx]);
          snap.trace_entry = "compare(data[" + std::to_string(idx) +
                             "]=" + std::to_string(data_copy[idx]) +
                             ", target=" + std::to_string(target) + ") -> skip";
        }

        rec.steps.push_back(std::move(snap));
      });

  {
    StepSnapshot final_snap;
    final_snap.data = data_copy;
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
    init.trace_entry = "begin binary_search(target=" +
                       std::to_string(target) +
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

  dsa::binary_search<int>(
      std::span<const int>(input), target,
      [&](std::span<const int>, std::size_t lo, std::size_t hi,
          std::size_t mid, const int & /*tgt*/, bool found) {
        bool val_lt = input[mid] < target;

        StepSnapshot snap;
        snap.data = input;
        snap.low = static_cast<int>(lo);
        snap.high = static_cast<int>(hi);
        snap.highlight_a = static_cast<int>(mid);

        snap.variables = {
            {"low", std::to_string(lo)},
            {"high", std::to_string(hi)},
            {"mid", std::to_string(mid)},
            {"data[mid]", std::to_string(input[mid])},
            {"target", std::to_string(target)},
        };

        if (found) {
          snap.found_index = static_cast<int>(mid);
          snap.current_line = binary_search_line(true, false, false);
          snap.status_text = "Found " + std::to_string(target) + " at index " +
                             std::to_string(mid) + "!";
          snap.trace_entry = "mid=" + std::to_string(mid) + "  data[mid]=" +
                             std::to_string(input[mid]) + " == target -> FOUND";
          was_found = true;
          found_at = static_cast<int>(mid);
        } else {
          snap.current_line =
              binary_search_line(false, val_lt, false);
          snap.status_text = "Window [" + std::to_string(lo) + ", " +
                             std::to_string(hi) +
                             ")  mid=" + std::to_string(mid) +
                             "  val=" + std::to_string(input[mid]);
          if (val_lt) {
            snap.trace_entry = "mid=" + std::to_string(mid) + "  data[mid]=" +
                               std::to_string(input[mid]) +
                               " < target -> low = mid+1";
          } else {
            snap.trace_entry = "mid=" + std::to_string(mid) + "  data[mid]=" +
                               std::to_string(input[mid]) +
                               " > target -> high = mid";
          }
        }

        rec.steps.push_back(std::move(snap));
      });

  {
    StepSnapshot final_snap;
    final_snap.data = input;
    final_snap.current_line =
        binary_search_line(was_found, false, true);
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

inline auto record_bubble_sort(const std::vector<int> &input)
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

  int pass = 0;
  int total_swaps = 0;

  std::size_t last_a = 0;

  dsa::bubble_sort<int>(
      std::span<int>(data),
      [&](std::span<const int> current, std::size_t a, std::size_t b,
          bool did_swap) {
        if (!rec.steps.empty() && a == 0 && last_a > 0) {
          pass++;
        }
        last_a = a;

        if (did_swap)
          total_swaps++;

        StepSnapshot snap;
        snap.data.assign(current.begin(), current.end());
        snap.highlight_a = static_cast<int>(a);
        snap.highlight_b = static_cast<int>(b);
        snap.sorted_boundary =
            static_cast<int>(current.size()) - pass;

        snap.variables = {
            {"pass", std::to_string(pass + 1)},
            {"j", std::to_string(a)},
            {"data[j]", std::to_string(current[a])},
            {"data[j+1]", std::to_string(current[b])},
            {"swaps", std::to_string(total_swaps)},
        };

        snap.current_line = bubble_sort_line(did_swap, false);

        if (did_swap) {
          snap.status_text = "Swap index " + std::to_string(a) + " (" +
                             std::to_string(current[a]) + ") <-> " +
                             std::to_string(b) + " (" +
                             std::to_string(current[b]) + ")";
          snap.trace_entry = "swap(data[" + std::to_string(a) + "]=" +
                             std::to_string(current[a]) + ", data[" +
                             std::to_string(b) +
                             "]=" + std::to_string(current[b]) + ")";
        } else {
          snap.status_text = "Compare index " + std::to_string(a) + " (" +
                             std::to_string(current[a]) + ") and " +
                             std::to_string(b) + " (" +
                             std::to_string(current[b]) + ") -> no swap";
          snap.trace_entry = "compare(data[" + std::to_string(a) + "]=" +
                             std::to_string(current[a]) + ", data[" +
                             std::to_string(b) +
                             "]=" + std::to_string(current[b]) + ") -> skip";
        }

        rec.steps.push_back(std::move(snap));
      });

  {
    StepSnapshot final_snap;
    final_snap.data = data;
    final_snap.sorted_boundary = 0; // everything sorted
    final_snap.current_line = bubble_sort_line(false, true);
    final_snap.status_text = "Bubble Sort complete! (" +
                             std::to_string(total_swaps) + " total swaps)";
    final_snap.trace_entry = "done — sorted in " +
                             std::to_string(pass + 1) + " passes, " +
                             std::to_string(total_swaps) + " swaps";
    final_snap.variables = {
        {"passes", std::to_string(pass + 1)},
        {"total_swaps", std::to_string(total_swaps)},
    };
    rec.steps.push_back(std::move(final_snap));
  }

  return rec;
}

} // namespace viz
