#pragma once

#include "code_panel.h"
#include "ring_buffer_snapshot.h"

namespace viz {

inline auto ring_buffer_enqueue_code() -> CodePanel {
  return {
      .title = "ring_buffer.h — enqueue",
      .lines =
          {
              "if (is_full()) throw out_of_range(\"full\");", // 0
              "data_[tail_] = val;",                         // 1
              "tail_ = (tail_ + 1) % N;",                   // 2
              "++size_;",                                    // 3
          },
  };
}

inline auto ring_buffer_dequeue_code() -> CodePanel {
  return {
      .title = "ring_buffer.h — dequeue",
      .lines =
          {
              "if (is_empty()) throw out_of_range(\"empty\");", // 0
              "T val = data_[head_];",                          // 1
              "head_ = (head_ + 1) % N;",                      // 2
              "--size_;",                                       // 3
              "return val;",                                    // 4
          },
  };
}

inline auto ring_buffer_idle_code_panel() -> CodePanel {
  return {
      .title = "No operation selected",
      .lines =
          {
              "Press [C] to configure an operation",
          },
  };
}

inline auto get_ring_buffer_code_panel(RingBufferOp op) -> CodePanel {
  switch (op) {
  case RingBufferOp::kEnqueue:
    return ring_buffer_enqueue_code();
  case RingBufferOp::kDequeue:
    return ring_buffer_dequeue_code();
  default:
    return ring_buffer_idle_code_panel();
  }
}

} // namespace viz
