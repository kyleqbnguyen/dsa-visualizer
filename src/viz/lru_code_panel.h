#pragma once

#include "code_panel.h"
#include "lru_snapshot.h"

namespace viz {

inline auto lru_get_code() -> CodePanel {
  return {
      .title = "lru_cache.h — get",
      .lines =
          {
              "auto get(key) {",                     // 0
              "    if (!map.has(key)) return miss;",  // 1
              "    node = map[key];",                 // 2
              "    moveToHead(node);",                // 3
              "    return node.value;",               // 4
              "}",                                   // 5
          },
  };
}

inline auto lru_put_code() -> CodePanel {
  return {
      .title = "lru_cache.h — put",
      .lines =
          {
              "void put(key, value) {",             // 0
              "    if (map.has(key)) {",             // 1
              "        update + moveToHead;",        // 2
              "    } else if (size == capacity) {",  // 3
              "        evict tail (LRU);",           // 4
              "    }",                               // 5
              "    insertAtHead(key, value);",       // 6
              "}",                                   // 7
          },
  };
}

inline auto lru_idle_code_panel() -> CodePanel {
  return {
      .title = "No operation selected",
      .lines =
          {
              "Press [C] to configure an operation",
          },
  };
}

inline auto get_lru_code_panel(LruOp op) -> CodePanel {
  switch (op) {
  case LruOp::kGet:
    return lru_get_code();
  case LruOp::kPut:
    return lru_put_code();
  default:
    return lru_idle_code_panel();
  }
}

} // namespace viz
