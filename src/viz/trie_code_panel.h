#pragma once

#include "code_panel.h"
#include "trie_snapshot.h"

namespace viz {

inline auto trie_insert_code() -> CodePanel {
  return {
      .title = "trie.h — insert",
      .lines =
          {
              "void insert(word) {",                    // 0
              "    node = root;",                       // 1
              "    for (char ch : word) {",             // 2
              "        if (!node.has(ch))",             // 3
              "            node.children[ch] = new;",   // 4
              "        node = node.children[ch];",     // 5
              "    }",                                  // 6
              "    node.isEnd = true;",                // 7
              "}",                                     // 8
          },
  };
}

inline auto trie_search_code() -> CodePanel {
  return {
      .title = "trie.h — search",
      .lines =
          {
              "bool search(word) {",                   // 0
              "    node = root;",                      // 1
              "    for (char ch : word) {",            // 2
              "        if (!node.has(ch)) return false;", // 3
              "        node = node.children[ch];",     // 4
              "    }",                                 // 5
              "    return node.isEnd;",                // 6
              "}",                                     // 7
          },
  };
}

inline auto trie_delete_code() -> CodePanel {
  return {
      .title = "trie.h — delete",
      .lines =
          {
              "void delete(word) {",                   // 0
              "    walk to end node;",                 // 1
              "    clear isEnd;",                      // 2
              "    while (node has no children",        // 3
              "           && !node.isEnd) {",          // 4
              "        remove node; go to parent;",    // 5
              "    }",                                 // 6
              "}",                                     // 7
          },
  };
}

inline auto trie_idle_code_panel() -> CodePanel {
  return {
      .title = "No operation selected",
      .lines =
          {
              "Press [C] to configure an operation",
          },
  };
}

inline auto get_trie_code_panel(TrieVizOp op) -> CodePanel {
  switch (op) {
  case TrieVizOp::kInsert:
    return trie_insert_code();
  case TrieVizOp::kSearch:
    return trie_search_code();
  case TrieVizOp::kDelete:
    return trie_delete_code();
  default:
    return trie_idle_code_panel();
  }
}

} // namespace viz
