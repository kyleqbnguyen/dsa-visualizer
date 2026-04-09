#pragma once

#include "code_panel.h"
#include "tree_snapshot.h"

namespace viz {

inline auto preorder_code() -> CodePanel {
  return {
      .title = "preorder.h",
      .lines =
          {
              "void preorder(node) {",          // 0
              "    if (node == null) return;",   // 1
              "    visit(node);",                // 2
              "    preorder(node->left);",       // 3
              "    preorder(node->right);",      // 4
              "}",                               // 5
          },
  };
}

inline auto inorder_code() -> CodePanel {
  return {
      .title = "inorder.h",
      .lines =
          {
              "void inorder(node) {",            // 0
              "    if (node == null) return;",    // 1
              "    inorder(node->left);",         // 2
              "    visit(node);",                 // 3
              "    inorder(node->right);",        // 4
              "}",                                // 5
          },
  };
}

inline auto postorder_code() -> CodePanel {
  return {
      .title = "postorder.h",
      .lines =
          {
              "void postorder(node) {",          // 0
              "    if (node == null) return;",    // 1
              "    postorder(node->left);",       // 2
              "    postorder(node->right);",      // 3
              "    visit(node);",                 // 4
              "}",                                // 5
          },
  };
}

inline auto bfs_tree_code() -> CodePanel {
  return {
      .title = "bfs_tree.h",
      .lines =
          {
              "queue.push(root);",                        // 0
              "while (!queue.empty()) {",                 // 1
              "    node = queue.front(); queue.pop();",   // 2
              "    visit(node);",                         // 3
              "    if (node->left)",                      // 4
              "        queue.push(node->left);",          // 5
              "    if (node->right)",                     // 6
              "        queue.push(node->right);",         // 7
              "}",                                        // 8
          },
  };
}

inline auto bst_find_code() -> CodePanel {
  return {
      .title = "bst.h — find",
      .lines =
          {
              "node = root;",                          // 0
              "while (node != null) {",                // 1
              "    if (node.val == target) return;",   // 2
              "    if (target < node.val)",             // 3
              "        node = node->left;",            // 4
              "    else node = node->right;",          // 5
              "}",                                     // 6
              "return NOT_FOUND;",                     // 7
          },
  };
}

inline auto bst_insert_code() -> CodePanel {
  return {
      .title = "bst.h — insert",
      .lines =
          {
              "node = root;",                             // 0
              "if (root == null) { root = new; return; }", // 1
              "while (true) {",                           // 2
              "    if (value <= node.val) {",              // 3
              "        if (!node.left) { add; return; }", // 4
              "    } else {",                             // 5
              "        if (!node.right) { add; return; }", // 6
              "    }",                                    // 7
              "}",                                        // 8
          },
  };
}

inline auto bst_delete_code() -> CodePanel {
  return {
      .title = "bst.h — delete",
      .lines =
          {
              "node = find(target);",                  // 0
              "// walk tree to find target",           // 1
              "if (found) {",                          // 2
              "    if (leaf) remove;",                 // 3
              "    else if (one child) replace;",      // 4
              "    else {",                            // 5
              "        succ = inorder_successor;",     // 6
              "        node.val = succ.val;",          // 7
              "        delete succ;",                  // 8
              "    }",                                 // 9
              "}",                                     // 10
          },
  };
}

inline auto tree_idle_code_panel() -> CodePanel {
  return {
      .title = "No operation selected",
      .lines =
          {
              "Press [C] to configure an operation",
          },
  };
}

inline auto get_tree_code_panel(TreeOp op) -> CodePanel {
  switch (op) {
  case TreeOp::kPreorder:
    return preorder_code();
  case TreeOp::kInorder:
    return inorder_code();
  case TreeOp::kPostorder:
    return postorder_code();
  case TreeOp::kBfs:
    return bfs_tree_code();
  case TreeOp::kBstFind:
    return bst_find_code();
  case TreeOp::kBstInsert:
    return bst_insert_code();
  case TreeOp::kBstDelete:
    return bst_delete_code();
  default:
    return tree_idle_code_panel();
  }
}

} // namespace viz
