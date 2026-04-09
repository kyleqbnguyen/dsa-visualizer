#pragma once

#include "heap_snapshot.h"

#include <algorithm>
#include <string>
#include <vector>

namespace viz {

namespace detail {

inline auto snap_heap(const std::vector<int>& data) -> HeapStepSnapshot {
  HeapStepSnapshot snap;
  snap.data = data;
  return snap;
}

inline auto parent_of(int i) -> int { return (i - 1) / 2; }
inline auto left_of(int i) -> int { return 2 * i + 1; }
inline auto right_of(int i) -> int { return 2 * i + 2; }

} // namespace detail

inline auto record_heap_idle(const std::vector<int>& data) -> HeapRecording {
  HeapRecording rec;
  rec.title = "Min Heap";

  auto snap = detail::snap_heap(data);
  snap.status_text = "Press [C] to select an operation";
  snap.variables = {{"size", std::to_string(data.size())}};
  rec.steps.push_back(std::move(snap));
  return rec;
}

inline auto record_heap_insert(const std::vector<int>& initial, int value)
    -> HeapRecording {
  HeapRecording rec;
  rec.title = "Min Heap — Insert";

  auto data = initial;

  {
    auto snap = detail::snap_heap(data);
    snap.status_text = "Initial heap  (size=" + std::to_string(data.size()) +
                       ")";
    snap.trace_entry = "begin push(" + std::to_string(value) + ")";
    snap.current_line = 0;
    snap.variables = {{"size", std::to_string(data.size())},
                      {"value", std::to_string(value)}};
    rec.steps.push_back(std::move(snap));
  }

  data.push_back(value);
  int idx = static_cast<int>(data.size()) - 1;

  {
    auto snap = detail::snap_heap(data);
    snap.highlight_a = idx;
    snap.status_text = "Append " + std::to_string(value) + " at index " +
                       std::to_string(idx);
    snap.trace_entry = "data[" + std::to_string(idx) +
                       "] = " + std::to_string(value);
    snap.current_line = 1;
    snap.variables = {{"size", std::to_string(data.size())},
                      {"value", std::to_string(value)},
                      {"idx", std::to_string(idx)}};
    rec.steps.push_back(std::move(snap));
  }

  while (idx > 0) {
    int par = detail::parent_of(idx);

    {
      auto snap = detail::snap_heap(data);
      snap.highlight_a = idx;
      snap.highlight_b = par;
      snap.status_text = "Compare data[" + std::to_string(idx) +
                         "]=" + std::to_string(data[idx]) + " with parent[" +
                         std::to_string(par) +
                         "]=" + std::to_string(data[par]);
      snap.trace_entry = "compare idx=" + std::to_string(idx) +
                         " parent=" + std::to_string(par);
      snap.current_line = 4;
      snap.variables = {{"idx", std::to_string(idx)},
                        {"parent", std::to_string(par)},
                        {"data[idx]", std::to_string(data[idx])},
                        {"data[parent]", std::to_string(data[par])}};
      rec.steps.push_back(std::move(snap));
    }

    if (data[par] > data[idx]) {
      std::swap(data[par], data[idx]);

      {
        auto snap = detail::snap_heap(data);
        snap.highlight_a = par;
        snap.highlight_b = idx;
        snap.status_text = "Swap " + std::to_string(data[idx]) + " and " +
                           std::to_string(data[par]);
        snap.trace_entry = "swap(" + std::to_string(idx) + ", " +
                           std::to_string(par) + ")";
        snap.current_line = 5;
        snap.variables = {{"idx", std::to_string(par)},
                          {"parent", std::to_string(detail::parent_of(par))}};
        rec.steps.push_back(std::move(snap));
      }

      idx = par;
    } else {
      break;
    }
  }

  {
    auto snap = detail::snap_heap(data);
    snap.highlight_a = idx;
    snap.status_text = "Insert complete — heap property restored";
    snap.trace_entry = "done: value " + std::to_string(value) +
                       " at index " + std::to_string(idx);
    snap.current_line = 7;
    snap.variables = {{"size", std::to_string(data.size())}};
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

inline auto record_heap_delete_min(const std::vector<int>& initial)
    -> HeapRecording {
  HeapRecording rec;
  rec.title = "Min Heap — Delete Min";

  auto data = initial;

  if (data.empty()) {
    auto snap = detail::snap_heap(data);
    snap.status_text = "Heap is empty — nothing to delete";
    snap.trace_entry = "throw: heap is empty";
    snap.current_line = 0;
    rec.steps.push_back(std::move(snap));
    return rec;
  }

  int min_val = data[0];

  {
    auto snap = detail::snap_heap(data);
    snap.highlight_a = 0;
    snap.status_text = "Min value is " + std::to_string(min_val) +
                       " at root (index 0)";
    snap.trace_entry = "begin pop()  min=" + std::to_string(min_val);
    snap.current_line = 0;
    snap.variables = {{"size", std::to_string(data.size())},
                      {"min", std::to_string(min_val)}};
    rec.steps.push_back(std::move(snap));
  }

  int last_idx = static_cast<int>(data.size()) - 1;
  data[0] = data[last_idx];
  data.pop_back();
  int n = static_cast<int>(data.size());

  if (n == 0) {
    auto snap = detail::snap_heap(data);
    snap.status_text = "Removed last element — heap is now empty";
    snap.trace_entry = "heap empty after removal";
    snap.current_line = 3;
    snap.variables = {{"removed", std::to_string(min_val)}, {"size", "0"}};
    rec.steps.push_back(std::move(snap));
    return rec;
  }

  {
    auto snap = detail::snap_heap(data);
    snap.highlight_a = 0;
    snap.highlight_b = last_idx;
    snap.status_text = "Swap root with last element (" +
                       std::to_string(data[0]) +
                       "), remove last";
    snap.trace_entry = "swap root with last, shrink";
    snap.current_line = 1;
    snap.variables = {{"size", std::to_string(n)},
                      {"root", std::to_string(data[0])}};
    rec.steps.push_back(std::move(snap));
  }

  int idx = 0;
  while (true) {
    int smallest = idx;
    int left = detail::left_of(idx);
    int right = detail::right_of(idx);

    if (left < n && data[left] < data[smallest])
      smallest = left;
    if (right < n && data[right] < data[smallest])
      smallest = right;

    {
      auto snap = detail::snap_heap(data);
      snap.highlight_a = idx;
      if (smallest != idx)
        snap.highlight_b = smallest;
      std::string child_info;
      if (left < n)
        child_info += "left[" + std::to_string(left) +
                      "]=" + std::to_string(data[left]);
      if (right < n) {
        if (!child_info.empty())
          child_info += ", ";
        child_info += "right[" + std::to_string(right) +
                      "]=" + std::to_string(data[right]);
      }
      snap.status_text = "Compare node[" + std::to_string(idx) +
                         "]=" + std::to_string(data[idx]) +
                         " with children: " + child_info;
      snap.trace_entry = "sinkDown idx=" + std::to_string(idx) +
                         " smallest=" + std::to_string(smallest);
      snap.current_line = 2;
      snap.variables = {{"idx", std::to_string(idx)},
                        {"smallest", std::to_string(smallest)},
                        {"data[idx]", std::to_string(data[idx])}};
      rec.steps.push_back(std::move(snap));
    }

    if (smallest == idx)
      break;

    std::swap(data[idx], data[smallest]);

    {
      auto snap = detail::snap_heap(data);
      snap.highlight_a = smallest;
      snap.highlight_b = idx;
      snap.status_text = "Swap data[" + std::to_string(idx) + "] and data[" +
                         std::to_string(smallest) + "]";
      snap.trace_entry = "swap(" + std::to_string(idx) + ", " +
                         std::to_string(smallest) + ")";
      snap.current_line = 2;
      snap.variables = {{"idx", std::to_string(smallest)}};
      rec.steps.push_back(std::move(snap));
    }

    idx = smallest;
  }

  {
    auto snap = detail::snap_heap(data);
    snap.status_text = "Delete min complete — removed " +
                       std::to_string(min_val);
    snap.trace_entry = "done: removed " + std::to_string(min_val);
    snap.current_line = 3;
    snap.variables = {{"size", std::to_string(data.size())},
                      {"removed", std::to_string(min_val)}};
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

inline auto record_heap_build(const std::vector<int>& input) -> HeapRecording {
  HeapRecording rec;
  rec.title = "Min Heap — Build Heap";

  auto data = input;
  int n = static_cast<int>(data.size());

  {
    auto snap = detail::snap_heap(data);
    snap.status_text = "Unsorted array — will heapify from n/2-1 down to 0";
    snap.trace_entry = "begin buildHeap  n=" + std::to_string(n);
    snap.current_line = 0;
    snap.variables = {{"n", std::to_string(n)}};
    rec.steps.push_back(std::move(snap));
  }

  for (int start = n / 2 - 1; start >= 0; --start) {
    int idx = start;

    {
      auto snap = detail::snap_heap(data);
      snap.highlight_a = idx;
      snap.status_text = "heapifyDown starting at index " +
                         std::to_string(idx);
      snap.trace_entry = "heapifyDown(" + std::to_string(idx) + ")";
      snap.current_line = 1;
      snap.variables = {{"start", std::to_string(start)},
                        {"idx", std::to_string(idx)}};
      rec.steps.push_back(std::move(snap));
    }

    while (true) {
      int smallest = idx;
      int left = detail::left_of(idx);
      int right = detail::right_of(idx);

      if (left < n && data[left] < data[smallest])
        smallest = left;
      if (right < n && data[right] < data[smallest])
        smallest = right;

      if (smallest == idx)
        break;

      {
        auto snap = detail::snap_heap(data);
        snap.highlight_a = idx;
        snap.highlight_b = smallest;
        snap.status_text = "Swap data[" + std::to_string(idx) +
                           "]=" + std::to_string(data[idx]) +
                           " with data[" + std::to_string(smallest) +
                           "]=" + std::to_string(data[smallest]);
        snap.trace_entry = "swap(" + std::to_string(idx) + ", " +
                           std::to_string(smallest) + ")";
        snap.current_line = 2;
        snap.variables = {{"idx", std::to_string(idx)},
                          {"smallest", std::to_string(smallest)}};
        rec.steps.push_back(std::move(snap));
      }

      std::swap(data[idx], data[smallest]);

      {
        auto snap = detail::snap_heap(data);
        snap.highlight_a = smallest;
        snap.status_text = "After swap — continue sinking at " +
                           std::to_string(smallest);
        snap.current_line = 2;
        snap.variables = {{"idx", std::to_string(smallest)}};
        rec.steps.push_back(std::move(snap));
      }

      idx = smallest;
    }
  }

  {
    auto snap = detail::snap_heap(data);
    snap.status_text = "Build heap complete — min-heap property satisfied";
    snap.trace_entry = "done: heap built";
    snap.current_line = 3;
    snap.variables = {{"size", std::to_string(n)}};
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

} // namespace viz
