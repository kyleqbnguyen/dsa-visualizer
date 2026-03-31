#pragma once

#include "code_panel.h"
#include "list_snapshot.h"

namespace viz {

// --- Stack ---

inline auto stack_push_code() -> CodePanel {
  return {
      .title = "stack.h — push",
      .lines =
          {
              "data_.push_back(val);", // 0
              "++size_;",              // 1
          },
  };
}

inline auto stack_pop_code() -> CodePanel {
  return {
      .title = "stack.h — pop",
      .lines =
          {
              "T val = data_.back();", // 0
              "data_.pop_back();",     // 1
              "--size_;",              // 2
              "return val;",           // 3
          },
  };
}

// --- Queue ---

inline auto queue_enqueue_code() -> CodePanel {
  return {
      .title = "queue.h — enqueue",
      .lines =
          {
              "data_.push_back(val);", // 0
              "++size_;",              // 1
          },
  };
}

inline auto queue_dequeue_code() -> CodePanel {
  return {
      .title = "queue.h — dequeue",
      .lines =
          {
              "T val = data_.front();", // 0
              "data_.pop_front();",     // 1
              "--size_;",               // 2
              "return val;",            // 3
          },
  };
}

// --- Selectors ---

inline auto stack_queue_idle_code_panel() -> CodePanel {
  return {
      .title = "No operation selected",
      .lines =
          {
              "Press [C] to configure an operation",
          },
  };
}

inline auto get_stack_code_panel(ListOp op) -> CodePanel {
  switch (op) {
  case ListOp::kPush:
    return stack_push_code();
  case ListOp::kPop:
    return stack_pop_code();
  default:
    return stack_queue_idle_code_panel();
  }
}

inline auto get_queue_code_panel(ListOp op) -> CodePanel {
  switch (op) {
  case ListOp::kEnqueue:
    return queue_enqueue_code();
  case ListOp::kDequeue:
    return queue_dequeue_code();
  default:
    return stack_queue_idle_code_panel();
  }
}

} // namespace viz
