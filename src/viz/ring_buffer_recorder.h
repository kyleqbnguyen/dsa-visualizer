#pragma once

#include "ring_buffer_snapshot.h"

#include <string>
#include <vector>

namespace viz {

namespace detail {

inline auto snap_ring(int capacity, const std::vector<int>& values, int head)
    -> RingBufferStepSnapshot {
  RingBufferStepSnapshot snap;
  snap.slots.resize(capacity);
  snap.head = head;
  snap.size = static_cast<int>(values.size());
  snap.tail = (head + snap.size) % capacity;
  for (int i = 0; i < snap.size; ++i) {
    int idx = (head + i) % capacity;
    snap.slots[idx].value = values[i];
    snap.slots[idx].state = RingSlotState::kOccupied;
  }
  return snap;
}

} // namespace detail

inline auto record_ring_buffer_idle(const std::vector<int>& initial,
                                    int capacity) -> RingBufferRecording {
  RingBufferRecording rec;
  rec.title = "Ring Buffer";

  auto snap = detail::snap_ring(capacity, initial, 0);
  snap.status_text = "Press [C] to select an operation";
  snap.current_line = -1;
  snap.variables = {{"capacity", std::to_string(capacity)},
                    {"size", std::to_string(initial.size())}};
  rec.steps.push_back(std::move(snap));
  return rec;
}

inline auto record_ring_buffer_enqueue(const std::vector<int>& initial,
                                       int capacity, int val)
    -> RingBufferRecording {
  RingBufferRecording rec;
  rec.title = "Ring Buffer — Enqueue";

  const int init_size = static_cast<int>(initial.size());
  const int tail = init_size % capacity;

  {
    auto snap = detail::snap_ring(capacity, initial, 0);
    snap.status_text = "Initial ring buffer  (head=0, tail=" +
                       std::to_string(tail) + ")";
    snap.trace_entry = "begin enqueue(val=" + std::to_string(val) + ")";
    snap.current_line = -1;
    snap.variables = {{"capacity", std::to_string(capacity)},
                      {"size", std::to_string(init_size)},
                      {"head_", "0"},
                      {"tail_", std::to_string(tail)}};
    rec.steps.push_back(std::move(snap));
  }

  if (init_size == capacity) {
    auto snap = detail::snap_ring(capacity, initial, 0);
    snap.status_text = "Buffer is full — cannot enqueue";
    snap.trace_entry = "throw out_of_range(\"full\")";
    snap.current_line = 0;
    snap.variables = {{"capacity", std::to_string(capacity)},
                      {"size", std::to_string(init_size)}};
    rec.steps.push_back(std::move(snap));
    return rec;
  }

  {
    auto snap = detail::snap_ring(capacity, initial, 0);
    snap.slots[tail].value = val;
    snap.slots[tail].state = RingSlotState::kNew;
    snap.status_text =
        "Write " + std::to_string(val) + " into slot " + std::to_string(tail);
    snap.trace_entry =
        "data_[tail_] = " + std::to_string(val) + "  (tail_=" +
        std::to_string(tail) + ")";
    snap.current_line = 1;
    snap.variables = {{"capacity", std::to_string(capacity)},
                      {"size", std::to_string(init_size)},
                      {"val", std::to_string(val)},
                      {"head_", "0"},
                      {"tail_", std::to_string(tail)}};
    rec.steps.push_back(std::move(snap));
  }

  const int new_tail = (tail + 1) % capacity;
  const int new_size = init_size + 1;

  {
    std::vector<int> after = initial;
    after.push_back(val);
    auto snap = detail::snap_ring(capacity, after, 0);
    snap.slots[tail].state = RingSlotState::kDone;
    snap.status_text = "Enqueued " + std::to_string(val) +
                       ".  tail_ = " + std::to_string(new_tail) +
                       "  size = " + std::to_string(new_size);
    snap.trace_entry = "tail_ = (tail_+1)%N  →  " + std::to_string(new_tail) +
                       ";  ++size_";
    snap.current_line = 2;
    snap.variables = {{"capacity", std::to_string(capacity)},
                      {"size", std::to_string(new_size)},
                      {"val", std::to_string(val)},
                      {"head_", "0"},
                      {"tail_", std::to_string(new_tail)}};
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

inline auto record_ring_buffer_dequeue(const std::vector<int>& initial,
                                       int capacity) -> RingBufferRecording {
  RingBufferRecording rec;
  rec.title = "Ring Buffer — Dequeue";

  const int init_size = static_cast<int>(initial.size());

  {
    auto snap = detail::snap_ring(capacity, initial, 0);
    snap.status_text = "Initial ring buffer  (head=0, tail=" +
                       std::to_string(init_size % capacity) + ")";
    snap.trace_entry = "begin dequeue()";
    snap.current_line = -1;
    snap.variables = {{"capacity", std::to_string(capacity)},
                      {"size", std::to_string(init_size)},
                      {"head_", "0"},
                      {"tail_", std::to_string(init_size % capacity)}};
    rec.steps.push_back(std::move(snap));
  }

  if (initial.empty()) {
    auto snap = detail::snap_ring(capacity, initial, 0);
    snap.status_text = "Buffer is empty — nothing to dequeue";
    snap.trace_entry = "throw out_of_range(\"empty\")";
    snap.current_line = 0;
    snap.variables = {{"capacity", std::to_string(capacity)}, {"size", "0"}};
    rec.steps.push_back(std::move(snap));
    return rec;
  }

  const int head_val = initial.front();

  {
    auto snap = detail::snap_ring(capacity, initial, 0);
    snap.slots[0].state = RingSlotState::kActive;
    snap.status_text =
        "Read head slot (slot 0) = " + std::to_string(head_val);
    snap.trace_entry = "T val = data_[head_]  →  " + std::to_string(head_val);
    snap.current_line = 1;
    snap.variables = {{"capacity", std::to_string(capacity)},
                      {"size", std::to_string(init_size)},
                      {"val", std::to_string(head_val)},
                      {"head_", "0"}};
    rec.steps.push_back(std::move(snap));
  }

  {
    auto snap = detail::snap_ring(capacity, initial, 0);
    snap.slots[0].state = RingSlotState::kRemoved;
    snap.status_text = "Removing slot 0 (value " + std::to_string(head_val) +
                       ")  head_ advances";
    snap.trace_entry = "head_ = (head_+1)%N  →  1";
    snap.current_line = 2;
    snap.variables = {{"capacity", std::to_string(capacity)},
                      {"size", std::to_string(init_size)},
                      {"val", std::to_string(head_val)},
                      {"head_", "0"}};
    rec.steps.push_back(std::move(snap));
  }

  const int new_size = init_size - 1;
  const int new_head = 1 % capacity;
  const int new_tail = init_size % capacity;

  {
    std::vector<int> after(initial.begin() + 1, initial.end());
    auto snap = detail::snap_ring(capacity, after, new_head);
    snap.status_text = "Dequeued " + std::to_string(head_val) +
                       ".  head_ = " + std::to_string(new_head) +
                       "  size = " + std::to_string(new_size);
    snap.trace_entry = "--size_;  return " + std::to_string(head_val);
    snap.current_line = 3;
    snap.variables = {{"capacity", std::to_string(capacity)},
                      {"size", std::to_string(new_size)},
                      {"val", std::to_string(head_val)},
                      {"head_", std::to_string(new_head)},
                      {"tail_", std::to_string(new_tail)}};
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

} // namespace viz
