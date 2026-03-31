#pragma once

#include <string>
#include <vector>

#include "list_recorder.h"
#include "list_snapshot.h"

namespace viz {

// ──────────────────────────────────────────
//  Stack recorders
// ──────────────────────────────────────────

inline auto record_stack_push(const std::vector<int> &initial, int val)
    -> ListAlgorithmRecording {
  ListAlgorithmRecording rec;
  rec.title = "Stack — Push";
  rec.algorithm_name = "stack";
  rec.op = ListOp::kPush;

  const std::size_t new_size = initial.size() + 1;

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, false);
    snap.status_text = "Initial stack  (TOP is rightmost)";
    snap.trace_entry = "begin push(val=" + std::to_string(val) + ")";
    snap.current_line = -1;
    snap.variables = {{"size", std::to_string(initial.size())}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    std::vector<int> with_new = initial;
    with_new.push_back(val);
    snap.nodes = detail::snap_list(with_new, false);
    snap.nodes.back().state = ListNodeState::kNew;
    snap.nodes.back().has_next = false;
    snap.status_text = "Create new node with value " + std::to_string(val);
    snap.trace_entry = "data_.push_back(" + std::to_string(val) + ")";
    snap.current_line = 0;
    snap.variables = {{"val", std::to_string(val)},
                      {"size", std::to_string(initial.size())}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    std::vector<int> with_new = initial;
    with_new.push_back(val);
    snap.nodes = detail::snap_list(with_new, false);
    snap.nodes.back().state = ListNodeState::kDone;
    snap.status_text = "Pushed " + std::to_string(val) +
                       ".  New TOP = " + std::to_string(val);
    snap.trace_entry = "++size_  →  size=" + std::to_string(new_size);
    snap.current_line = 1;
    snap.variables = {{"val", std::to_string(val)},
                      {"size", std::to_string(new_size)}};
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

inline auto record_stack_pop(const std::vector<int> &initial)
    -> ListAlgorithmRecording {
  ListAlgorithmRecording rec;
  rec.title = "Stack — Pop";
  rec.algorithm_name = "stack";
  rec.op = ListOp::kPop;

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, false);
    snap.status_text = "Initial stack  (TOP is rightmost)";
    snap.trace_entry = "begin pop()";
    snap.current_line = -1;
    snap.variables = {{"size", std::to_string(initial.size())}};
    rec.steps.push_back(std::move(snap));
  }

  if (initial.empty()) {
    ListStepSnapshot snap;
    snap.status_text = "Stack is empty — nothing to pop";
    snap.trace_entry = "error: empty stack";
    snap.current_line = 0;
    snap.variables = {{"size", "0"}};
    rec.steps.push_back(std::move(snap));
    return rec;
  }

  const int top_val = initial.back();
  const std::size_t new_size = initial.size() - 1;

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, false);
    snap.nodes.back().state = ListNodeState::kActive;
    snap.status_text = "TOP = " + std::to_string(top_val) + "  (reading before removal)";
    snap.trace_entry = "T val = data_.back()  →  " + std::to_string(top_val);
    snap.current_line = 0;
    snap.variables = {{"val", std::to_string(top_val)},
                      {"size", std::to_string(initial.size())}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, false);
    snap.nodes.back().state = ListNodeState::kRemoved;
    snap.status_text = "Removing TOP element " + std::to_string(top_val);
    snap.trace_entry = "data_.pop_back()";
    snap.current_line = 1;
    snap.variables = {{"val", std::to_string(top_val)},
                      {"size", std::to_string(initial.size())}};
    rec.steps.push_back(std::move(snap));
  }

  {
    std::vector<int> after(initial.begin(), initial.end() - 1);
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(after, false);
    snap.status_text = "Popped " + std::to_string(top_val) +
                       ".  size=" + std::to_string(new_size);
    snap.trace_entry = "--size_; return " + std::to_string(top_val);
    snap.current_line = 3;
    snap.variables = {{"val", std::to_string(top_val)},
                      {"size", std::to_string(new_size)}};
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

// ──────────────────────────────────────────
//  Queue recorders
// ──────────────────────────────────────────

inline auto record_queue_enqueue(const std::vector<int> &initial, int val)
    -> ListAlgorithmRecording {
  ListAlgorithmRecording rec;
  rec.title = "Queue — Enqueue";
  rec.algorithm_name = "queue";
  rec.op = ListOp::kEnqueue;

  const std::size_t new_size = initial.size() + 1;

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, false);
    snap.status_text = "Initial queue  (FRONT=left, REAR=right)";
    snap.trace_entry = "begin enqueue(val=" + std::to_string(val) + ")";
    snap.current_line = -1;
    snap.variables = {{"size", std::to_string(initial.size())}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    std::vector<int> with_new = initial;
    with_new.push_back(val);
    snap.nodes = detail::snap_list(with_new, false);
    snap.nodes.back().state = ListNodeState::kNew;
    snap.nodes.back().has_next = false;
    snap.status_text = "Create new node with value " + std::to_string(val);
    snap.trace_entry = "data_.push_back(" + std::to_string(val) + ")";
    snap.current_line = 0;
    snap.variables = {{"val", std::to_string(val)},
                      {"size", std::to_string(initial.size())}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    std::vector<int> with_new = initial;
    with_new.push_back(val);
    snap.nodes = detail::snap_list(with_new, false);
    snap.nodes.back().state = ListNodeState::kDone;
    snap.status_text = "Enqueued " + std::to_string(val) +
                       ".  New REAR = " + std::to_string(val);
    snap.trace_entry = "++size_  →  size=" + std::to_string(new_size);
    snap.current_line = 1;
    snap.variables = {{"val", std::to_string(val)},
                      {"size", std::to_string(new_size)}};
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

inline auto record_queue_dequeue(const std::vector<int> &initial)
    -> ListAlgorithmRecording {
  ListAlgorithmRecording rec;
  rec.title = "Queue — Dequeue";
  rec.algorithm_name = "queue";
  rec.op = ListOp::kDequeue;

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, false);
    snap.status_text = "Initial queue  (FRONT=left, REAR=right)";
    snap.trace_entry = "begin dequeue()";
    snap.current_line = -1;
    snap.variables = {{"size", std::to_string(initial.size())}};
    rec.steps.push_back(std::move(snap));
  }

  if (initial.empty()) {
    ListStepSnapshot snap;
    snap.status_text = "Queue is empty — nothing to dequeue";
    snap.trace_entry = "error: empty queue";
    snap.current_line = 0;
    snap.variables = {{"size", "0"}};
    rec.steps.push_back(std::move(snap));
    return rec;
  }

  const int front_val = initial.front();
  const std::size_t new_size = initial.size() - 1;

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, false);
    snap.nodes.front().state = ListNodeState::kActive;
    snap.status_text = "FRONT = " + std::to_string(front_val) + "  (reading before removal)";
    snap.trace_entry = "T val = data_.front()  →  " + std::to_string(front_val);
    snap.current_line = 0;
    snap.variables = {{"val", std::to_string(front_val)},
                      {"size", std::to_string(initial.size())}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, false);
    snap.nodes.front().state = ListNodeState::kRemoved;
    snap.status_text = "Removing FRONT element " + std::to_string(front_val);
    snap.trace_entry = "data_.pop_front()";
    snap.current_line = 1;
    snap.variables = {{"val", std::to_string(front_val)},
                      {"size", std::to_string(initial.size())}};
    rec.steps.push_back(std::move(snap));
  }

  {
    std::vector<int> after(initial.begin() + 1, initial.end());
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(after, false);
    snap.status_text = "Dequeued " + std::to_string(front_val) +
                       ".  size=" + std::to_string(new_size);
    snap.trace_entry = "--size_; return " + std::to_string(front_val);
    snap.current_line = 3;
    snap.variables = {{"val", std::to_string(front_val)},
                      {"size", std::to_string(new_size)}};
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

} // namespace viz
