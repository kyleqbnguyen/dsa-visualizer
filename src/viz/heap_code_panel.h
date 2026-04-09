#pragma once

#include "code_panel.h"
#include "heap_snapshot.h"

namespace viz {

inline auto heap_insert_code() -> CodePanel {
  return {
      .title = "min_heap.h — push",
      .lines =
          {
              "void push(heap, value) {",          // 0
              "    heap.data[heap.length] = value;", // 1
              "    heap.length++;",                  // 2
              "    heapifyUp(heap, heap.length-1);", // 3
              "    if (idx == 0) return;",           // 4
              "    if (data[parent] > data[idx]) {", // 5
              "        swap(data[parent], data[idx]);", // 6
              "}",                                   // 7
          },
  };
}

inline auto heap_delete_min_code() -> CodePanel {
  return {
      .title = "min_heap.h — pop",
      .lines =
          {
              "int val = data[0];",              // 0
              "data[0] = data[last]; shrink();", // 1
              "heapifyDown(0);",                 // 2
              "return val;",                     // 3
          },
  };
}

inline auto heap_build_code() -> CodePanel {
  return {
      .title = "min_heap.h — buildHeap",
      .lines =
          {
              "for (i = n/2-1; i >= 0; --i) {", // 0
              "    heapifyDown(i);",             // 1
              "    // sink until heap property", // 2
              "}",                               // 3
          },
  };
}

inline auto heap_idle_code_panel() -> CodePanel {
  return {
      .title = "No operation selected",
      .lines =
          {
              "Press [C] to configure an operation",
          },
  };
}

inline auto get_heap_code_panel(HeapOp op) -> CodePanel {
  switch (op) {
  case HeapOp::kInsert:
    return heap_insert_code();
  case HeapOp::kDeleteMin:
    return heap_delete_min_code();
  case HeapOp::kBuildHeap:
    return heap_build_code();
  default:
    return heap_idle_code_panel();
  }
}

} // namespace viz
