#pragma once

#include <string>
#include <vector>

#include "list_snapshot.h"

namespace viz {

namespace detail {

inline auto snap_list(const std::vector<int> &vals, bool doubly)
    -> std::vector<ListNodeSnap> {
  std::vector<ListNodeSnap> nodes;
  for (size_t i = 0; i < vals.size(); ++i) {
    ListNodeSnap n;
    n.value = vals[i];
    n.state = ListNodeState::kNormal;
    n.has_next = (i + 1 < vals.size());
    n.has_prev = doubly && (i > 0);
    nodes.push_back(n);
  }
  return nodes;
}

} // namespace detail

// ──────────────────────────────────────────
//  Idle recorder (no operation selected)
// ──────────────────────────────────────────

inline auto record_idle(const std::vector<int> &initial,
                        const std::string &title, bool doubly)
    -> ListAlgorithmRecording {
  ListAlgorithmRecording rec;
  rec.title = title;
  rec.algorithm_name = doubly ? "doubly_linked_list" : "singly_linked_list";
  rec.op = ListOp::kNone;

  ListStepSnapshot snap;
  snap.nodes = detail::snap_list(initial, doubly);
  snap.status_text = "Press [C] to select an operation";
  snap.current_line = -1;
  rec.steps.push_back(std::move(snap));

  return rec;
}

inline auto record_stack_queue_idle(const std::vector<int> &initial,
                                    const std::string &title)
    -> ListAlgorithmRecording {
  ListAlgorithmRecording rec;
  rec.title = title;
  rec.algorithm_name = title.find("Stack") != std::string::npos ? "stack" : "queue";
  rec.op = ListOp::kNone;

  ListStepSnapshot snap;
  snap.nodes = detail::snap_list(initial, false);
  snap.status_text = "Press [C] to select an operation";
  snap.current_line = -1;
  rec.steps.push_back(std::move(snap));

  return rec;
}

// ──────────────────────────────────────────
//  Singly Linked List recorders
// ──────────────────────────────────────────

inline auto record_singly_prepend(const std::vector<int> &initial, int val)
    -> ListAlgorithmRecording {
  ListAlgorithmRecording rec;
  rec.title = "Singly Linked List — Prepend";
  rec.algorithm_name = "singly_linked_list";
  rec.op = ListOp::kPrepend;

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, false);
    snap.status_text = "Initial list";
    snap.trace_entry = "begin prepend(val=" + std::to_string(val) + ")";
    snap.current_line = -1;
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    std::vector<int> with_new = {val};
    with_new.insert(with_new.end(), initial.begin(), initial.end());
    snap.nodes = detail::snap_list(with_new, false);
    snap.nodes[0].state = ListNodeState::kNew;
    snap.nodes[0].has_next = false;
    snap.status_text = "Create new node with value " + std::to_string(val);
    snap.trace_entry = "Node* node = new Node(" + std::to_string(val) + ")";
    snap.current_line = 0;
    snap.variables = {{"val", std::to_string(val)}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    std::vector<int> with_new = {val};
    with_new.insert(with_new.end(), initial.begin(), initial.end());
    snap.nodes = detail::snap_list(with_new, false);
    snap.nodes[0].state = ListNodeState::kNew;
    snap.nodes[0].has_next = true;
    snap.status_text = "Link new node to old head";
    snap.trace_entry = "node->next = head";
    snap.current_line = 1;
    snap.variables = {{"val", std::to_string(val)}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    std::vector<int> with_new = {val};
    with_new.insert(with_new.end(), initial.begin(), initial.end());
    snap.nodes = detail::snap_list(with_new, false);
    snap.nodes[0].state = ListNodeState::kDone;
    snap.status_text = "Update head pointer";
    snap.trace_entry = "head = node; ++length";
    snap.current_line = 2;
    snap.variables = {{"val", std::to_string(val)},
                      {"length", std::to_string(initial.size() + 1)}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    std::vector<int> with_new = {val};
    with_new.insert(with_new.end(), initial.begin(), initial.end());
    snap.nodes = detail::snap_list(with_new, false);
    snap.nodes[0].state = ListNodeState::kDone;
    snap.status_text = "Prepend complete";
    snap.trace_entry = "done";
    snap.current_line = 3;
    snap.variables = {{"length", std::to_string(initial.size() + 1)}};
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

inline auto record_singly_append(const std::vector<int> &initial, int val)
    -> ListAlgorithmRecording {
  ListAlgorithmRecording rec;
  rec.title = "Singly Linked List — Append";
  rec.algorithm_name = "singly_linked_list";
  rec.op = ListOp::kAppend;

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, false);
    snap.status_text = "Initial list";
    snap.trace_entry =
        "begin append(val=" + std::to_string(val) + ")";
    snap.current_line = -1;
    rec.steps.push_back(std::move(snap));
  }

  for (size_t i = 0; i < initial.size(); ++i) {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, false);
    snap.nodes[static_cast<int>(i)].state = ListNodeState::kActive;
    snap.variables = {{"curr", "node[" + std::to_string(i) + "]"},
                      {"val", std::to_string(initial[i])}};

    if (i == 0) {
      snap.status_text = "Start at head";
      snap.trace_entry = "curr = head";
      snap.current_line = 1;
    } else {
      snap.status_text =
          "Traverse to node " + std::to_string(i);
      snap.trace_entry = "curr = curr->next";
      snap.current_line = 3;
    }
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    auto result = initial;
    result.push_back(val);
    snap.nodes = detail::snap_list(result, false);
    snap.nodes.back().state = ListNodeState::kNew;
    snap.nodes.back().has_next = false;
    if (result.size() >= 2) {
      snap.nodes[result.size() - 2].has_next = false;
    }
    snap.status_text = "Create new node with value " + std::to_string(val);
    snap.trace_entry =
        "Node* node = new Node(" + std::to_string(val) + ")";
    snap.current_line = 0;
    snap.variables = {{"val", std::to_string(val)}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    auto result = initial;
    result.push_back(val);
    snap.nodes = detail::snap_list(result, false);
    snap.nodes.back().state = ListNodeState::kNew;
    snap.status_text = "Link tail to new node";
    snap.trace_entry = "curr->next = node";
    snap.current_line = 5;
    snap.variables = {{"val", std::to_string(val)}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    auto result = initial;
    result.push_back(val);
    snap.nodes = detail::snap_list(result, false);
    snap.nodes.back().state = ListNodeState::kDone;
    snap.status_text = "Append complete";
    snap.trace_entry = "tail = node; ++length";
    snap.current_line = 6;
    snap.variables = {{"length", std::to_string(result.size())}};
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

inline auto record_singly_insert_at(const std::vector<int> &initial,
                                    size_t index, int val)
    -> ListAlgorithmRecording {
  ListAlgorithmRecording rec;
  rec.title = "Singly Linked List — Insert At";
  rec.algorithm_name = "singly_linked_list";
  rec.op = ListOp::kInsertAt;

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, false);
    snap.status_text = "Initial list";
    snap.trace_entry = "begin insert_at(idx=" + std::to_string(index) +
                       ", val=" + std::to_string(val) + ")";
    snap.current_line = -1;
    snap.variables = {{"idx", std::to_string(index)},
                      {"val", std::to_string(val)}};
    rec.steps.push_back(std::move(snap));
  }

  size_t traverse_to = (index > 0) ? index - 1 : 0;
  for (size_t i = 0; i <= traverse_to && i < initial.size(); ++i) {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, false);
    snap.nodes[static_cast<int>(i)].state = ListNodeState::kActive;
    snap.status_text = "Traverse: curr at index " + std::to_string(i);
    snap.trace_entry =
        (i == 0) ? "curr = head" : "curr = curr->next";
    snap.current_line = (i == 0) ? 2 : 4;
    snap.variables = {{"i", std::to_string(i)},
                      {"idx", std::to_string(index)},
                      {"val", std::to_string(val)}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    auto result = initial;
    result.insert(result.begin() + static_cast<int>(index), val);
    snap.nodes = detail::snap_list(result, false);
    snap.nodes[static_cast<int>(index)].state = ListNodeState::kNew;
    snap.nodes[static_cast<int>(index)].has_next = false;
    if (index > 0) {
      snap.nodes[static_cast<int>(index - 1)].has_next = false;
    }
    snap.status_text = "Create new node with value " + std::to_string(val);
    snap.trace_entry =
        "Node* node = new Node(" + std::to_string(val) + ")";
    snap.current_line = 6;
    snap.variables = {{"idx", std::to_string(index)},
                      {"val", std::to_string(val)}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    auto result = initial;
    result.insert(result.begin() + static_cast<int>(index), val);
    snap.nodes = detail::snap_list(result, false);
    snap.nodes[static_cast<int>(index)].state = ListNodeState::kTarget;
    snap.status_text = "Link new node into list";
    snap.trace_entry = "node->next = curr->next; curr->next = node";
    snap.current_line = 8;
    snap.variables = {{"idx", std::to_string(index)},
                      {"val", std::to_string(val)},
                      {"length", std::to_string(result.size())}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    auto result = initial;
    result.insert(result.begin() + static_cast<int>(index), val);
    snap.nodes = detail::snap_list(result, false);
    snap.nodes[static_cast<int>(index)].state = ListNodeState::kDone;
    snap.status_text = "Insert complete";
    snap.trace_entry = "++length";
    snap.current_line = 9;
    snap.variables = {{"length", std::to_string(result.size())}};
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

inline auto record_singly_remove_at(const std::vector<int> &initial,
                                    size_t index)
    -> ListAlgorithmRecording {
  ListAlgorithmRecording rec;
  rec.title = "Singly Linked List — Remove At";
  rec.algorithm_name = "singly_linked_list";
  rec.op = ListOp::kRemoveAt;

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, false);
    snap.status_text = "Initial list";
    snap.trace_entry =
        "begin remove_at(idx=" + std::to_string(index) + ")";
    snap.current_line = -1;
    snap.variables = {{"idx", std::to_string(index)}};
    rec.steps.push_back(std::move(snap));
  }

  if (index == 0) {
    {
      ListStepSnapshot snap;
      snap.nodes = detail::snap_list(initial, false);
      snap.nodes[0].state = ListNodeState::kRemoved;
      snap.status_text = "Remove head node";
      snap.trace_entry = "head = head->next";
      snap.current_line = 0;
      snap.variables = {{"idx", "0"},
                        {"removed", std::to_string(initial[0])}};
      rec.steps.push_back(std::move(snap));
    }
  } else {
    for (size_t i = 0; i < index && i < initial.size(); ++i) {
      ListStepSnapshot snap;
      snap.nodes = detail::snap_list(initial, false);
      snap.nodes[static_cast<int>(i)].state = ListNodeState::kActive;
      snap.status_text = "Traverse: prev at index " + std::to_string(i);
      snap.trace_entry =
          (i == 0) ? "prev = head" : "prev = prev->next";
      snap.current_line = (i == 0) ? 1 : 3;
      snap.variables = {{"i", std::to_string(i)},
                        {"idx", std::to_string(index)}};
      rec.steps.push_back(std::move(snap));
    }

    {
      ListStepSnapshot snap;
      snap.nodes = detail::snap_list(initial, false);
      snap.nodes[static_cast<int>(index)].state = ListNodeState::kRemoved;
      snap.status_text = "Found node to remove";
      snap.trace_entry = "found node to remove";
      snap.current_line = 5;
      snap.variables = {{"idx", std::to_string(index)},
                        {"removed", std::to_string(initial[index])}};
      rec.steps.push_back(std::move(snap));
    }

    {
      ListStepSnapshot snap;
      snap.nodes = detail::snap_list(initial, false);
      snap.nodes[static_cast<int>(index)].state = ListNodeState::kRemoved;
      snap.nodes[static_cast<int>(index)].has_next = false;
      if (index > 0) {
        snap.nodes[static_cast<int>(index - 1)].has_next =
            (index + 1 < initial.size());
      }
      snap.status_text = "Rewire prev->next to skip removed node";
      snap.trace_entry = "prev->next = tmp->next";
      snap.current_line = 6;
      snap.variables = {{"idx", std::to_string(index)}};
      rec.steps.push_back(std::move(snap));
    }
  }

  {
    ListStepSnapshot snap;
    auto result = initial;
    if (index < result.size()) {
      result.erase(result.begin() + static_cast<int>(index));
    }
    snap.nodes = detail::snap_list(result, false);
    snap.status_text = "Remove complete";
    snap.trace_entry = "delete tmp; --length";
    snap.current_line = 7;
    snap.variables = {{"length", std::to_string(result.size())}};
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

inline auto record_singly_get(const std::vector<int> &initial, size_t index)
    -> ListAlgorithmRecording {
  ListAlgorithmRecording rec;
  rec.title = "Singly Linked List — Get";
  rec.algorithm_name = "singly_linked_list";
  rec.op = ListOp::kGet;

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, false);
    snap.status_text = "Initial list";
    snap.trace_entry =
        "begin get(idx=" + std::to_string(index) + ")";
    snap.current_line = -1;
    snap.variables = {{"idx", std::to_string(index)}};
    rec.steps.push_back(std::move(snap));
  }

  size_t limit = (index < initial.size()) ? index : initial.size() - 1;
  for (size_t i = 0; i <= limit; ++i) {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, false);

    if (i == index) {
      snap.nodes[static_cast<int>(i)].state = ListNodeState::kFound;
      snap.status_text = "Found value " +
                         std::to_string(initial[i]) + " at index " +
                         std::to_string(i);
      snap.trace_entry = "return curr->val (" +
                         std::to_string(initial[i]) + ")";
      snap.current_line = 4;
    } else {
      snap.nodes[static_cast<int>(i)].state = ListNodeState::kActive;
      snap.status_text = "Traverse: curr at index " + std::to_string(i);
      snap.trace_entry =
          (i == 0) ? "curr = head" : "curr = curr->next";
      snap.current_line = (i == 0) ? 0 : 2;
    }
    snap.variables = {{"i", std::to_string(i)},
                      {"idx", std::to_string(index)}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, false);
    if (index < initial.size()) {
      snap.nodes[static_cast<int>(index)].state = ListNodeState::kFound;
      snap.status_text =
          "Result: " + std::to_string(initial[index]);
      snap.trace_entry =
          "return " + std::to_string(initial[index]);
    } else {
      snap.status_text = "Index out of bounds";
      snap.trace_entry = "throw out_of_range";
    }
    snap.current_line = 4;
    snap.variables = {
        {"result",
         index < initial.size() ? std::to_string(initial[index])
                                : "error"}};
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

// ──────────────────────────────────────────
//  Doubly Linked List recorders
// ──────────────────────────────────────────

inline auto record_doubly_prepend(const std::vector<int> &initial, int val)
    -> ListAlgorithmRecording {
  ListAlgorithmRecording rec;
  rec.title = "Doubly Linked List — Prepend";
  rec.algorithm_name = "doubly_linked_list";
  rec.op = ListOp::kPrepend;

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, true);
    snap.status_text = "Initial list";
    snap.trace_entry = "begin prepend(val=" + std::to_string(val) + ")";
    snap.current_line = -1;
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    std::vector<int> with_new = {val};
    with_new.insert(with_new.end(), initial.begin(), initial.end());
    snap.nodes = detail::snap_list(with_new, true);
    snap.nodes[0].state = ListNodeState::kNew;
    snap.nodes[0].has_next = false;
    snap.nodes[0].has_prev = false;
    if (with_new.size() > 1) {
      snap.nodes[1].has_prev = false;
    }
    snap.status_text = "Create new node with value " + std::to_string(val);
    snap.trace_entry = "Node* node = new Node(" + std::to_string(val) + ")";
    snap.current_line = 0;
    snap.variables = {{"val", std::to_string(val)}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    std::vector<int> with_new = {val};
    with_new.insert(with_new.end(), initial.begin(), initial.end());
    snap.nodes = detail::snap_list(with_new, true);
    snap.nodes[0].state = ListNodeState::kNew;
    snap.status_text = "Link new node to old head";
    snap.trace_entry = "node->next = head; head->prev = node";
    snap.current_line = 1;
    snap.variables = {{"val", std::to_string(val)}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    std::vector<int> with_new = {val};
    with_new.insert(with_new.end(), initial.begin(), initial.end());
    snap.nodes = detail::snap_list(with_new, true);
    snap.nodes[0].state = ListNodeState::kDone;
    snap.status_text = "Update head pointer";
    snap.trace_entry = "head = node; ++length";
    snap.current_line = 3;
    snap.variables = {{"val", std::to_string(val)},
                      {"length", std::to_string(initial.size() + 1)}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    std::vector<int> with_new = {val};
    with_new.insert(with_new.end(), initial.begin(), initial.end());
    snap.nodes = detail::snap_list(with_new, true);
    snap.nodes[0].state = ListNodeState::kDone;
    snap.status_text = "Prepend complete";
    snap.trace_entry = "done";
    snap.current_line = 4;
    snap.variables = {{"length", std::to_string(initial.size() + 1)}};
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

inline auto record_doubly_append(const std::vector<int> &initial, int val)
    -> ListAlgorithmRecording {
  ListAlgorithmRecording rec;
  rec.title = "Doubly Linked List — Append";
  rec.algorithm_name = "doubly_linked_list";
  rec.op = ListOp::kAppend;

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, true);
    snap.status_text = "Initial list";
    snap.trace_entry = "begin append(val=" + std::to_string(val) + ")";
    snap.current_line = -1;
    rec.steps.push_back(std::move(snap));
  }

  for (size_t i = 0; i < initial.size(); ++i) {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, true);
    snap.nodes[static_cast<int>(i)].state = ListNodeState::kActive;
    snap.variables = {{"curr", "node[" + std::to_string(i) + "]"}};
    if (i == 0) {
      snap.status_text = "Start at head";
      snap.trace_entry = "curr = head";
      snap.current_line = 1;
    } else {
      snap.status_text = "Traverse to node " + std::to_string(i);
      snap.trace_entry = "curr = curr->next";
      snap.current_line = 3;
    }
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    auto result = initial;
    result.push_back(val);
    snap.nodes = detail::snap_list(result, true);
    snap.nodes.back().state = ListNodeState::kNew;
    snap.nodes.back().has_prev = false;
    if (result.size() >= 2) {
      snap.nodes[result.size() - 2].has_next = false;
    }
    snap.status_text = "Create new node with value " + std::to_string(val);
    snap.trace_entry =
        "Node* node = new Node(" + std::to_string(val) + ")";
    snap.current_line = 0;
    snap.variables = {{"val", std::to_string(val)}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    auto result = initial;
    result.push_back(val);
    snap.nodes = detail::snap_list(result, true);
    snap.nodes.back().state = ListNodeState::kNew;
    snap.status_text = "Link tail to new node";
    snap.trace_entry = "node->prev = curr; curr->next = node";
    snap.current_line = 6;
    snap.variables = {{"val", std::to_string(val)}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    auto result = initial;
    result.push_back(val);
    snap.nodes = detail::snap_list(result, true);
    snap.nodes.back().state = ListNodeState::kDone;
    snap.status_text = "Append complete";
    snap.trace_entry = "tail = node; ++length";
    snap.current_line = 7;
    snap.variables = {{"length", std::to_string(result.size())}};
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

inline auto record_doubly_insert_at(const std::vector<int> &initial,
                                    size_t index, int val)
    -> ListAlgorithmRecording {
  ListAlgorithmRecording rec;
  rec.title = "Doubly Linked List — Insert At";
  rec.algorithm_name = "doubly_linked_list";
  rec.op = ListOp::kInsertAt;

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, true);
    snap.status_text = "Initial list";
    snap.trace_entry = "begin insert_at(idx=" + std::to_string(index) +
                       ", val=" + std::to_string(val) + ")";
    snap.current_line = -1;
    snap.variables = {{"idx", std::to_string(index)},
                      {"val", std::to_string(val)}};
    rec.steps.push_back(std::move(snap));
  }

  for (size_t i = 0; i < index && i < initial.size(); ++i) {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, true);
    snap.nodes[static_cast<int>(i)].state = ListNodeState::kActive;
    snap.status_text = "Traverse: curr at index " + std::to_string(i);
    snap.trace_entry = (i == 0) ? "curr = head" : "curr = curr->next";
    snap.current_line = (i == 0) ? 2 : 4;
    snap.variables = {{"i", std::to_string(i)},
                      {"idx", std::to_string(index)},
                      {"val", std::to_string(val)}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    auto result = initial;
    result.insert(result.begin() + static_cast<int>(index), val);
    snap.nodes = detail::snap_list(result, true);
    snap.nodes[static_cast<int>(index)].state = ListNodeState::kNew;
    snap.nodes[static_cast<int>(index)].has_next = false;
    snap.nodes[static_cast<int>(index)].has_prev = false;
    if (index > 0) {
      snap.nodes[static_cast<int>(index - 1)].has_next = false;
    }
    if (static_cast<int>(index) + 1 < static_cast<int>(snap.nodes.size())) {
      snap.nodes[static_cast<int>(index) + 1].has_prev = false;
    }
    snap.status_text = "Create new node with value " + std::to_string(val);
    snap.trace_entry =
        "Node* node = new Node(" + std::to_string(val) + ")";
    snap.current_line = 6;
    snap.variables = {{"idx", std::to_string(index)},
                      {"val", std::to_string(val)}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    auto result = initial;
    result.insert(result.begin() + static_cast<int>(index), val);
    snap.nodes = detail::snap_list(result, true);
    snap.nodes[static_cast<int>(index)].state = ListNodeState::kTarget;
    snap.status_text = "Link new node into list";
    snap.trace_entry = "rewire prev/next pointers";
    snap.current_line = 9;
    snap.variables = {{"idx", std::to_string(index)},
                      {"val", std::to_string(val)}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    auto result = initial;
    result.insert(result.begin() + static_cast<int>(index), val);
    snap.nodes = detail::snap_list(result, true);
    snap.nodes[static_cast<int>(index)].state = ListNodeState::kDone;
    snap.status_text = "Insert complete";
    snap.trace_entry = "++length";
    snap.current_line = 11;
    snap.variables = {{"length", std::to_string(result.size())}};
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

inline auto record_doubly_remove_at(const std::vector<int> &initial,
                                    size_t index)
    -> ListAlgorithmRecording {
  ListAlgorithmRecording rec;
  rec.title = "Doubly Linked List — Remove At";
  rec.algorithm_name = "doubly_linked_list";
  rec.op = ListOp::kRemoveAt;

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, true);
    snap.status_text = "Initial list";
    snap.trace_entry =
        "begin remove_at(idx=" + std::to_string(index) + ")";
    snap.current_line = -1;
    snap.variables = {{"idx", std::to_string(index)}};
    rec.steps.push_back(std::move(snap));
  }

  for (size_t i = 0; i <= index && i < initial.size(); ++i) {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, true);
    snap.nodes[static_cast<int>(i)].state = ListNodeState::kActive;
    snap.status_text = "Traverse: curr at index " + std::to_string(i);
    snap.trace_entry = (i == 0) ? "curr = head" : "curr = curr->next";
    snap.current_line = (i == 0) ? 0 : 2;
    snap.variables = {{"i", std::to_string(i)},
                      {"idx", std::to_string(index)}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, true);
    snap.nodes[static_cast<int>(index)].state = ListNodeState::kRemoved;
    snap.status_text = "Found node to remove";
    snap.trace_entry = "found node to remove";
    snap.current_line = 4;
    snap.variables = {{"idx", std::to_string(index)},
                      {"removed", std::to_string(initial[index])}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, true);
    snap.nodes[static_cast<int>(index)].state = ListNodeState::kRemoved;
    snap.status_text = "Rewire prev/next pointers around removed node";
    snap.trace_entry = "prev->next = curr->next; next->prev = curr->prev";
    snap.current_line = 5;
    snap.variables = {{"idx", std::to_string(index)}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    auto result = initial;
    if (index < result.size()) {
      result.erase(result.begin() + static_cast<int>(index));
    }
    snap.nodes = detail::snap_list(result, true);
    snap.status_text = "Remove complete";
    snap.trace_entry = "delete curr; --length";
    snap.current_line = 6;
    snap.variables = {{"length", std::to_string(result.size())}};
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

inline auto record_doubly_get(const std::vector<int> &initial, size_t index)
    -> ListAlgorithmRecording {
  ListAlgorithmRecording rec;
  rec.title = "Doubly Linked List — Get";
  rec.algorithm_name = "doubly_linked_list";
  rec.op = ListOp::kGet;

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, true);
    snap.status_text = "Initial list";
    snap.trace_entry = "begin get(idx=" + std::to_string(index) + ")";
    snap.current_line = -1;
    snap.variables = {{"idx", std::to_string(index)}};
    rec.steps.push_back(std::move(snap));
  }

  size_t limit = (index < initial.size()) ? index : initial.size() - 1;
  for (size_t i = 0; i <= limit; ++i) {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, true);
    if (i == index) {
      snap.nodes[static_cast<int>(i)].state = ListNodeState::kFound;
      snap.status_text = "Found value " +
                         std::to_string(initial[i]) + " at index " +
                         std::to_string(i);
      snap.trace_entry = "return curr->val (" +
                         std::to_string(initial[i]) + ")";
      snap.current_line = 4;
    } else {
      snap.nodes[static_cast<int>(i)].state = ListNodeState::kActive;
      snap.status_text = "Traverse: curr at index " + std::to_string(i);
      snap.trace_entry = (i == 0) ? "curr = head" : "curr = curr->next";
      snap.current_line = (i == 0) ? 0 : 2;
    }
    snap.variables = {{"i", std::to_string(i)},
                      {"idx", std::to_string(index)}};
    rec.steps.push_back(std::move(snap));
  }

  {
    ListStepSnapshot snap;
    snap.nodes = detail::snap_list(initial, true);
    if (index < initial.size()) {
      snap.nodes[static_cast<int>(index)].state = ListNodeState::kFound;
      snap.status_text = "Result: " + std::to_string(initial[index]);
      snap.trace_entry = "return " + std::to_string(initial[index]);
    } else {
      snap.status_text = "Index out of bounds";
      snap.trace_entry = "throw out_of_range";
    }
    snap.current_line = 4;
    snap.variables = {
        {"result",
         index < initial.size() ? std::to_string(initial[index])
                                : "error"}};
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

} // namespace viz
