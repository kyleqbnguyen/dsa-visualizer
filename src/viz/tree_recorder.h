#pragma once

#include "tree_snapshot.h"

#include <algorithm>
#include <queue>
#include <stack>
#include <string>
#include <vector>

namespace viz {

namespace tree_detail {

struct FlatTree {
  std::vector<TreeNodeSnap> nodes;
  int root = 0;
};

inline auto buildFlat(const std::vector<int>& level_order) -> FlatTree {
  FlatTree ft;
  if (level_order.empty())
    return ft;

  for (int v : level_order) {
    TreeNodeSnap n;
    n.value = v;
    n.state = (v == -1) ? TreeNodeState::kNull : TreeNodeState::kNormal;
    ft.nodes.push_back(n);
  }

  for (int i = 0; i < static_cast<int>(ft.nodes.size()); ++i) {
    if (ft.nodes[i].state == TreeNodeState::kNull)
      continue;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    if (left < static_cast<int>(ft.nodes.size()) &&
        ft.nodes[left].state != TreeNodeState::kNull) {
      ft.nodes[i].left = left;
      ft.nodes[left].parent = i;
    }
    if (right < static_cast<int>(ft.nodes.size()) &&
        ft.nodes[right].state != TreeNodeState::kNull) {
      ft.nodes[i].right = right;
      ft.nodes[right].parent = i;
    }
  }
  ft.root = 0;
  return ft;
}

inline auto snap(const FlatTree& ft, const std::vector<int>& visited)
    -> TreeStepSnapshot {
  TreeStepSnapshot s;
  s.nodes = ft.nodes;
  s.root = ft.root;
  s.visit_order = visited;
  return s;
}

inline void setAllNormal(FlatTree& ft) {
  for (auto& n : ft.nodes) {
    if (n.state != TreeNodeState::kNull)
      n.state = TreeNodeState::kNormal;
  }
}

inline void markVisited(FlatTree& ft, const std::vector<int>& visited) {
  for (int idx : visited) {
    if (idx >= 0 && idx < static_cast<int>(ft.nodes.size()))
      ft.nodes[idx].state = TreeNodeState::kVisited;
  }
}

} // namespace tree_detail

inline auto record_tree_idle(const std::vector<int>& level_order)
    -> TreeRecording {
  TreeRecording rec;
  rec.title = "Binary Tree";
  rec.algorithm_name = "tree";
  rec.op = TreeOp::kNone;

  auto ft = tree_detail::buildFlat(level_order);
  auto s = tree_detail::snap(ft, {});
  s.status_text = "Press [C] to select an operation";
  rec.steps.push_back(std::move(s));
  return rec;
}

inline auto record_tree_preorder(const std::vector<int>& level_order)
    -> TreeRecording {
  TreeRecording rec;
  rec.title = "Pre-order Traversal";
  rec.algorithm_name = "tree";
  rec.op = TreeOp::kPreorder;

  auto ft = tree_detail::buildFlat(level_order);
  if (ft.nodes.empty()) {
    auto s = tree_detail::snap(ft, {});
    s.status_text = "Empty tree";
    rec.steps.push_back(std::move(s));
    return rec;
  }

  std::vector<int> visited;

  {
    auto s = tree_detail::snap(ft, visited);
    s.status_text = "Begin pre-order traversal (root->left->right)";
    s.trace_entry = "preorder(root)";
    s.current_line = 0;
    rec.steps.push_back(std::move(s));
  }

  struct Frame {
    int node;
    int phase; // 0=visit, 1=go left, 2=go right, 3=done
  };

  std::stack<Frame> stk;
  stk.push({ft.root, 0});

  while (!stk.empty()) {
    auto& frame = stk.top();
    int idx = frame.node;

    if (idx < 0 || idx >= static_cast<int>(ft.nodes.size()) ||
        ft.nodes[idx].state == TreeNodeState::kNull) {
      stk.pop();
      {
        tree_detail::setAllNormal(ft);
        tree_detail::markVisited(ft, visited);
        auto s = tree_detail::snap(ft, visited);
        s.status_text = "null — return";
        s.trace_entry = "return (null)";
        s.current_line = 1;
        rec.steps.push_back(std::move(s));
      }
      continue;
    }

    if (frame.phase == 0) {
      tree_detail::setAllNormal(ft);
      tree_detail::markVisited(ft, visited);
      ft.nodes[idx].state = TreeNodeState::kActive;
      visited.push_back(idx);

      auto s = tree_detail::snap(ft, visited);
      s.status_text = "Visit node " + std::to_string(ft.nodes[idx].value);
      s.trace_entry =
          "visit(" + std::to_string(ft.nodes[idx].value) + ")";
      s.current_line = 2;
      s.variables = {{"node", std::to_string(ft.nodes[idx].value)}};
      rec.steps.push_back(std::move(s));

      frame.phase = 1;
    } else if (frame.phase == 1) {
      frame.phase = 2;
      int left = ft.nodes[idx].left;
      if (left >= 0) {
        tree_detail::setAllNormal(ft);
        tree_detail::markVisited(ft, visited);
        ft.nodes[left].state = TreeNodeState::kVisiting;

        auto s = tree_detail::snap(ft, visited);
        s.status_text = "Go left to " +
                        std::to_string(ft.nodes[left].value);
        s.trace_entry = "preorder(left=" +
                        std::to_string(ft.nodes[left].value) + ")";
        s.current_line = 3;
        rec.steps.push_back(std::move(s));

        stk.push({left, 0});
      }
    } else if (frame.phase == 2) {
      frame.phase = 3;
      int right = ft.nodes[idx].right;
      if (right >= 0) {
        tree_detail::setAllNormal(ft);
        tree_detail::markVisited(ft, visited);
        ft.nodes[right].state = TreeNodeState::kVisiting;

        auto s = tree_detail::snap(ft, visited);
        s.status_text = "Go right to " +
                        std::to_string(ft.nodes[right].value);
        s.trace_entry = "preorder(right=" +
                        std::to_string(ft.nodes[right].value) + ")";
        s.current_line = 4;
        rec.steps.push_back(std::move(s));

        stk.push({right, 0});
      }
    } else {
      stk.pop();
    }
  }

  {
    tree_detail::setAllNormal(ft);
    tree_detail::markVisited(ft, visited);
    auto s = tree_detail::snap(ft, visited);
    s.status_text = "Pre-order traversal complete";
    s.trace_entry = "done";
    s.current_line = 5;
    rec.steps.push_back(std::move(s));
  }

  return rec;
}

inline auto record_tree_inorder(const std::vector<int>& level_order)
    -> TreeRecording {
  TreeRecording rec;
  rec.title = "In-order Traversal";
  rec.algorithm_name = "tree";
  rec.op = TreeOp::kInorder;

  auto ft = tree_detail::buildFlat(level_order);
  if (ft.nodes.empty()) {
    auto s = tree_detail::snap(ft, {});
    s.status_text = "Empty tree";
    rec.steps.push_back(std::move(s));
    return rec;
  }

  std::vector<int> visited;

  {
    auto s = tree_detail::snap(ft, visited);
    s.status_text = "Begin in-order traversal (left->root->right)";
    s.trace_entry = "inorder(root)";
    s.current_line = 0;
    rec.steps.push_back(std::move(s));
  }

  struct Frame {
    int node;
    int phase; // 0=go left, 1=visit, 2=go right, 3=done
  };

  std::stack<Frame> stk;
  stk.push({ft.root, 0});

  while (!stk.empty()) {
    auto& frame = stk.top();
    int idx = frame.node;

    if (idx < 0 || idx >= static_cast<int>(ft.nodes.size()) ||
        ft.nodes[idx].state == TreeNodeState::kNull) {
      stk.pop();
      continue;
    }

    if (frame.phase == 0) {
      frame.phase = 1;
      int left = ft.nodes[idx].left;
      if (left >= 0) {
        tree_detail::setAllNormal(ft);
        tree_detail::markVisited(ft, visited);
        ft.nodes[left].state = TreeNodeState::kVisiting;

        auto s = tree_detail::snap(ft, visited);
        s.status_text = "Go left to " +
                        std::to_string(ft.nodes[left].value);
        s.trace_entry = "inorder(left=" +
                        std::to_string(ft.nodes[left].value) + ")";
        s.current_line = 2;
        rec.steps.push_back(std::move(s));

        stk.push({left, 0});
      }
    } else if (frame.phase == 1) {
      tree_detail::setAllNormal(ft);
      tree_detail::markVisited(ft, visited);
      ft.nodes[idx].state = TreeNodeState::kActive;
      visited.push_back(idx);

      auto s = tree_detail::snap(ft, visited);
      s.status_text = "Visit node " + std::to_string(ft.nodes[idx].value);
      s.trace_entry =
          "visit(" + std::to_string(ft.nodes[idx].value) + ")";
      s.current_line = 3;
      s.variables = {{"node", std::to_string(ft.nodes[idx].value)}};
      rec.steps.push_back(std::move(s));

      frame.phase = 2;
    } else if (frame.phase == 2) {
      frame.phase = 3;
      int right = ft.nodes[idx].right;
      if (right >= 0) {
        tree_detail::setAllNormal(ft);
        tree_detail::markVisited(ft, visited);
        ft.nodes[right].state = TreeNodeState::kVisiting;

        auto s = tree_detail::snap(ft, visited);
        s.status_text = "Go right to " +
                        std::to_string(ft.nodes[right].value);
        s.trace_entry = "inorder(right=" +
                        std::to_string(ft.nodes[right].value) + ")";
        s.current_line = 4;
        rec.steps.push_back(std::move(s));

        stk.push({right, 0});
      }
    } else {
      stk.pop();
    }
  }

  {
    tree_detail::setAllNormal(ft);
    tree_detail::markVisited(ft, visited);
    auto s = tree_detail::snap(ft, visited);
    s.status_text = "In-order traversal complete";
    s.trace_entry = "done";
    s.current_line = 5;
    rec.steps.push_back(std::move(s));
  }

  return rec;
}

inline auto record_tree_postorder(const std::vector<int>& level_order)
    -> TreeRecording {
  TreeRecording rec;
  rec.title = "Post-order Traversal";
  rec.algorithm_name = "tree";
  rec.op = TreeOp::kPostorder;

  auto ft = tree_detail::buildFlat(level_order);
  if (ft.nodes.empty()) {
    auto s = tree_detail::snap(ft, {});
    s.status_text = "Empty tree";
    rec.steps.push_back(std::move(s));
    return rec;
  }

  std::vector<int> visited;

  {
    auto s = tree_detail::snap(ft, visited);
    s.status_text = "Begin post-order traversal (left->right->root)";
    s.trace_entry = "postorder(root)";
    s.current_line = 0;
    rec.steps.push_back(std::move(s));
  }

  struct Frame {
    int node;
    int phase; // 0=go left, 1=go right, 2=visit, 3=done
  };

  std::stack<Frame> stk;
  stk.push({ft.root, 0});

  while (!stk.empty()) {
    auto& frame = stk.top();
    int idx = frame.node;

    if (idx < 0 || idx >= static_cast<int>(ft.nodes.size()) ||
        ft.nodes[idx].state == TreeNodeState::kNull) {
      stk.pop();
      continue;
    }

    if (frame.phase == 0) {
      frame.phase = 1;
      int left = ft.nodes[idx].left;
      if (left >= 0) {
        tree_detail::setAllNormal(ft);
        tree_detail::markVisited(ft, visited);
        ft.nodes[left].state = TreeNodeState::kVisiting;

        auto s = tree_detail::snap(ft, visited);
        s.status_text = "Go left to " +
                        std::to_string(ft.nodes[left].value);
        s.trace_entry = "postorder(left=" +
                        std::to_string(ft.nodes[left].value) + ")";
        s.current_line = 2;
        rec.steps.push_back(std::move(s));

        stk.push({left, 0});
      }
    } else if (frame.phase == 1) {
      frame.phase = 2;
      int right = ft.nodes[idx].right;
      if (right >= 0) {
        tree_detail::setAllNormal(ft);
        tree_detail::markVisited(ft, visited);
        ft.nodes[right].state = TreeNodeState::kVisiting;

        auto s = tree_detail::snap(ft, visited);
        s.status_text = "Go right to " +
                        std::to_string(ft.nodes[right].value);
        s.trace_entry = "postorder(right=" +
                        std::to_string(ft.nodes[right].value) + ")";
        s.current_line = 3;
        rec.steps.push_back(std::move(s));

        stk.push({right, 0});
      }
    } else if (frame.phase == 2) {
      tree_detail::setAllNormal(ft);
      tree_detail::markVisited(ft, visited);
      ft.nodes[idx].state = TreeNodeState::kActive;
      visited.push_back(idx);

      auto s = tree_detail::snap(ft, visited);
      s.status_text = "Visit node " + std::to_string(ft.nodes[idx].value);
      s.trace_entry =
          "visit(" + std::to_string(ft.nodes[idx].value) + ")";
      s.current_line = 4;
      s.variables = {{"node", std::to_string(ft.nodes[idx].value)}};
      rec.steps.push_back(std::move(s));

      frame.phase = 3;
    } else {
      stk.pop();
    }
  }

  {
    tree_detail::setAllNormal(ft);
    tree_detail::markVisited(ft, visited);
    auto s = tree_detail::snap(ft, visited);
    s.status_text = "Post-order traversal complete";
    s.trace_entry = "done";
    s.current_line = 5;
    rec.steps.push_back(std::move(s));
  }

  return rec;
}

inline auto record_tree_bfs(const std::vector<int>& level_order)
    -> TreeRecording {
  TreeRecording rec;
  rec.title = "BFS (Level-order) Traversal";
  rec.algorithm_name = "tree";
  rec.op = TreeOp::kBfs;

  auto ft = tree_detail::buildFlat(level_order);
  if (ft.nodes.empty()) {
    auto s = tree_detail::snap(ft, {});
    s.status_text = "Empty tree";
    rec.steps.push_back(std::move(s));
    return rec;
  }

  std::vector<int> visited;
  std::queue<int> q;
  q.push(ft.root);

  {
    ft.nodes[ft.root].state = TreeNodeState::kVisiting;
    auto s = tree_detail::snap(ft, visited);
    s.status_text = "Begin BFS — enqueue root";
    s.trace_entry = "queue.push(root)";
    s.current_line = 0;
    s.variables = {{"queue_size", "1"}};
    rec.steps.push_back(std::move(s));
  }

  while (!q.empty()) {
    int idx = q.front();
    q.pop();

    tree_detail::setAllNormal(ft);
    tree_detail::markVisited(ft, visited);
    ft.nodes[idx].state = TreeNodeState::kActive;
    visited.push_back(idx);

    {
      auto s = tree_detail::snap(ft, visited);
      s.status_text = "Visit node " + std::to_string(ft.nodes[idx].value);
      s.trace_entry =
          "visit(" + std::to_string(ft.nodes[idx].value) + ")";
      s.current_line = 3;
      s.variables = {{"node", std::to_string(ft.nodes[idx].value)},
                     {"queue_size", std::to_string(q.size())}};
      rec.steps.push_back(std::move(s));
    }

    int left = ft.nodes[idx].left;
    int right = ft.nodes[idx].right;

    if (left >= 0) {
      q.push(left);
      tree_detail::setAllNormal(ft);
      tree_detail::markVisited(ft, visited);
      ft.nodes[left].state = TreeNodeState::kVisiting;

      auto s = tree_detail::snap(ft, visited);
      s.status_text = "Enqueue left child " +
                      std::to_string(ft.nodes[left].value);
      s.trace_entry = "queue.push(" +
                      std::to_string(ft.nodes[left].value) + ")";
      s.current_line = 5;
      s.variables = {{"queue_size", std::to_string(q.size())}};
      rec.steps.push_back(std::move(s));
    }

    if (right >= 0) {
      q.push(right);
      tree_detail::setAllNormal(ft);
      tree_detail::markVisited(ft, visited);
      ft.nodes[right].state = TreeNodeState::kVisiting;

      auto s = tree_detail::snap(ft, visited);
      s.status_text = "Enqueue right child " +
                      std::to_string(ft.nodes[right].value);
      s.trace_entry = "queue.push(" +
                      std::to_string(ft.nodes[right].value) + ")";
      s.current_line = 5;
      s.variables = {{"queue_size", std::to_string(q.size())}};
      rec.steps.push_back(std::move(s));
    }
  }

  {
    tree_detail::setAllNormal(ft);
    tree_detail::markVisited(ft, visited);
    auto s = tree_detail::snap(ft, visited);
    s.status_text = "BFS traversal complete";
    s.trace_entry = "done";
    s.current_line = 7;
    rec.steps.push_back(std::move(s));
  }

  return rec;
}

inline auto record_bst_find(const std::vector<int>& level_order, int target)
    -> TreeRecording {
  TreeRecording rec;
  rec.title = "BST Find — " + std::to_string(target);
  rec.algorithm_name = "bst";
  rec.op = TreeOp::kBstFind;

  auto ft = tree_detail::buildFlat(level_order);
  if (ft.nodes.empty()) {
    auto s = tree_detail::snap(ft, {});
    s.status_text = "Empty tree — not found";
    rec.steps.push_back(std::move(s));
    return rec;
  }

  {
    auto s = tree_detail::snap(ft, {});
    s.status_text = "Find " + std::to_string(target) + " in BST";
    s.trace_entry = "find(" + std::to_string(target) + ")";
    s.current_line = 0;
    s.variables = {{"target", std::to_string(target)}};
    rec.steps.push_back(std::move(s));
  }

  int idx = ft.root;
  while (idx >= 0 && idx < static_cast<int>(ft.nodes.size()) &&
         ft.nodes[idx].state != TreeNodeState::kNull) {
    tree_detail::setAllNormal(ft);
    ft.nodes[idx].state = TreeNodeState::kActive;

    int val = ft.nodes[idx].value;

    if (val == target) {
      ft.nodes[idx].state = TreeNodeState::kFound;
      auto s = tree_detail::snap(ft, {});
      s.status_text = "Found " + std::to_string(target) + " at node";
      s.trace_entry = "found! node.value=" + std::to_string(val);
      s.current_line = 2;
      s.variables = {{"target", std::to_string(target)},
                     {"node", std::to_string(val)}};
      rec.steps.push_back(std::move(s));
      return rec;
    }

    {
      auto s = tree_detail::snap(ft, {});
      std::string dir = (target < val) ? "left" : "right";
      s.status_text = std::to_string(target) + " " +
                      ((target < val) ? "<" : ">") + " " +
                      std::to_string(val) + " — go " + dir;
      s.trace_entry = "compare " + std::to_string(target) + " vs " +
                      std::to_string(val) + " → " + dir;
      s.current_line = (target < val) ? 3 : 4;
      s.variables = {{"target", std::to_string(target)},
                     {"node", std::to_string(val)}};
      rec.steps.push_back(std::move(s));
    }

    ft.nodes[idx].state = TreeNodeState::kVisited;
    idx = (target < val) ? ft.nodes[idx].left : ft.nodes[idx].right;
  }

  {
    tree_detail::setAllNormal(ft);
    auto s = tree_detail::snap(ft, {});
    s.status_text = std::to_string(target) + " not found in BST";
    s.trace_entry = "not found";
    s.current_line = 5;
    s.variables = {{"target", std::to_string(target)}};
    rec.steps.push_back(std::move(s));
  }

  return rec;
}

inline auto record_bst_insert(const std::vector<int>& level_order, int value)
    -> TreeRecording {
  TreeRecording rec;
  rec.title = "BST Insert — " + std::to_string(value);
  rec.algorithm_name = "bst";
  rec.op = TreeOp::kBstInsert;

  auto ft = tree_detail::buildFlat(level_order);

  if (ft.nodes.empty()) {
    TreeNodeSnap n;
    n.value = value;
    n.state = TreeNodeState::kInserted;
    ft.nodes.push_back(n);
    ft.root = 0;

    auto s = tree_detail::snap(ft, {});
    s.status_text = "Tree was empty — inserted " + std::to_string(value) +
                    " as root";
    s.trace_entry = "insert(" + std::to_string(value) + ") → new root";
    s.current_line = 1;
    rec.steps.push_back(std::move(s));
    return rec;
  }

  {
    auto s = tree_detail::snap(ft, {});
    s.status_text = "Insert " + std::to_string(value) + " into BST";
    s.trace_entry = "insert(" + std::to_string(value) + ")";
    s.current_line = 0;
    s.variables = {{"value", std::to_string(value)}};
    rec.steps.push_back(std::move(s));
  }

  int idx = ft.root;
  while (true) {
    int val = ft.nodes[idx].value;

    tree_detail::setAllNormal(ft);
    ft.nodes[idx].state = TreeNodeState::kActive;

    {
      std::string dir = (value <= val) ? "left" : "right";
      auto s = tree_detail::snap(ft, {});
      s.status_text = std::to_string(value) + " " +
                      ((value <= val) ? "<=" : ">") + " " +
                      std::to_string(val) + " — go " + dir;
      s.trace_entry = "at " + std::to_string(val) + " → " + dir;
      s.current_line = (value <= val) ? 3 : 5;
      s.variables = {{"value", std::to_string(value)},
                     {"node", std::to_string(val)}};
      rec.steps.push_back(std::move(s));
    }

    ft.nodes[idx].state = TreeNodeState::kVisited;

    int next = (value <= val) ? ft.nodes[idx].left : ft.nodes[idx].right;
    if (next < 0) {
      TreeNodeSnap n;
      n.value = value;
      n.state = TreeNodeState::kInserted;
      n.parent = idx;
      int new_idx = static_cast<int>(ft.nodes.size());
      ft.nodes.push_back(n);
      if (value <= val)
        ft.nodes[idx].left = new_idx;
      else
        ft.nodes[idx].right = new_idx;

      tree_detail::setAllNormal(ft);
      ft.nodes[new_idx].state = TreeNodeState::kInserted;

      auto s = tree_detail::snap(ft, {});
      s.status_text = "Inserted " + std::to_string(value) +
                      " as " + ((value <= val) ? "left" : "right") +
                      " child of " + std::to_string(val);
      s.trace_entry = "new node(" + std::to_string(value) + ")";
      s.current_line = 7;
      s.variables = {{"value", std::to_string(value)},
                     {"parent", std::to_string(val)}};
      rec.steps.push_back(std::move(s));
      return rec;
    }

    idx = next;
  }
}

inline auto record_bst_delete(const std::vector<int>& level_order, int target)
    -> TreeRecording {
  TreeRecording rec;
  rec.title = "BST Delete — " + std::to_string(target);
  rec.algorithm_name = "bst";
  rec.op = TreeOp::kBstDelete;

  auto ft = tree_detail::buildFlat(level_order);
  if (ft.nodes.empty()) {
    auto s = tree_detail::snap(ft, {});
    s.status_text = "Empty tree — nothing to delete";
    rec.steps.push_back(std::move(s));
    return rec;
  }

  {
    auto s = tree_detail::snap(ft, {});
    s.status_text = "Delete " + std::to_string(target) + " from BST";
    s.trace_entry = "delete(" + std::to_string(target) + ")";
    s.current_line = 0;
    s.variables = {{"target", std::to_string(target)}};
    rec.steps.push_back(std::move(s));
  }

  int idx = ft.root;
  int parent = -1;
  bool is_left = false;

  while (idx >= 0 && idx < static_cast<int>(ft.nodes.size()) &&
         ft.nodes[idx].state != TreeNodeState::kNull) {
    tree_detail::setAllNormal(ft);
    ft.nodes[idx].state = TreeNodeState::kActive;

    int val = ft.nodes[idx].value;

    if (val == target) {
      {
        auto s = tree_detail::snap(ft, {});
        s.status_text = "Found " + std::to_string(target);
        s.trace_entry = "found node " + std::to_string(val);
        s.current_line = 2;
        rec.steps.push_back(std::move(s));
      }

      bool has_left = ft.nodes[idx].left >= 0;
      bool has_right = ft.nodes[idx].right >= 0;

      if (!has_left && !has_right) {
        ft.nodes[idx].state = TreeNodeState::kDeleted;
        {
          auto s = tree_detail::snap(ft, {});
          s.status_text = "Leaf node — remove directly";
          s.trace_entry = "case: leaf → remove";
          s.current_line = 3;
          rec.steps.push_back(std::move(s));
        }

        ft.nodes[idx].state = TreeNodeState::kNull;
        if (parent >= 0) {
          if (is_left)
            ft.nodes[parent].left = -1;
          else
            ft.nodes[parent].right = -1;
        }

      } else if (has_left && has_right) {
        int succ = ft.nodes[idx].right;
        int succ_parent = idx;
        while (ft.nodes[succ].left >= 0) {
          succ_parent = succ;
          succ = ft.nodes[succ].left;
        }

        tree_detail::setAllNormal(ft);
        ft.nodes[idx].state = TreeNodeState::kActive;
        ft.nodes[succ].state = TreeNodeState::kVisiting;

        {
          auto s = tree_detail::snap(ft, {});
          s.status_text = "Two children — find in-order successor: " +
                          std::to_string(ft.nodes[succ].value);
          s.trace_entry = "successor=" +
                          std::to_string(ft.nodes[succ].value);
          s.current_line = 6;
          rec.steps.push_back(std::move(s));
        }

        ft.nodes[idx].value = ft.nodes[succ].value;

        {
          tree_detail::setAllNormal(ft);
          ft.nodes[idx].state = TreeNodeState::kActive;
          auto s = tree_detail::snap(ft, {});
          s.status_text = "Copy successor value " +
                          std::to_string(ft.nodes[idx].value) +
                          " into node";
          s.trace_entry = "copy " +
                          std::to_string(ft.nodes[idx].value);
          s.current_line = 7;
          rec.steps.push_back(std::move(s));
        }

        ft.nodes[succ].state = TreeNodeState::kDeleted;
        {
          auto s = tree_detail::snap(ft, {});
          s.status_text = "Remove successor node";
          s.trace_entry = "remove successor";
          s.current_line = 8;
          rec.steps.push_back(std::move(s));
        }

        int succ_child = ft.nodes[succ].right;
        if (succ_parent == idx)
          ft.nodes[succ_parent].right = succ_child;
        else
          ft.nodes[succ_parent].left = succ_child;
        ft.nodes[succ].state = TreeNodeState::kNull;

      } else {
        int child = has_left ? ft.nodes[idx].left : ft.nodes[idx].right;

        ft.nodes[idx].state = TreeNodeState::kDeleted;
        ft.nodes[child].state = TreeNodeState::kVisiting;
        {
          auto s = tree_detail::snap(ft, {});
          s.status_text = "One child — replace with child " +
                          std::to_string(ft.nodes[child].value);
          s.trace_entry = "replace with " +
                          std::to_string(ft.nodes[child].value);
          s.current_line = 4;
          rec.steps.push_back(std::move(s));
        }

        if (parent >= 0) {
          if (is_left)
            ft.nodes[parent].left = child;
          else
            ft.nodes[parent].right = child;
        } else {
          ft.root = child;
        }
        ft.nodes[idx].state = TreeNodeState::kNull;
      }

      tree_detail::setAllNormal(ft);
      {
        auto s = tree_detail::snap(ft, {});
        s.status_text = "Delete complete";
        s.trace_entry = "done";
        s.current_line = 9;
        rec.steps.push_back(std::move(s));
      }
      return rec;
    }

    {
      std::string dir = (target < val) ? "left" : "right";
      auto s = tree_detail::snap(ft, {});
      s.status_text = std::to_string(target) + " " +
                      ((target < val) ? "<" : ">") + " " +
                      std::to_string(val) + " — go " + dir;
      s.trace_entry = "search " + dir;
      s.current_line = 1;
      rec.steps.push_back(std::move(s));
    }

    ft.nodes[idx].state = TreeNodeState::kVisited;
    parent = idx;
    is_left = (target < ft.nodes[idx].value);
    idx = is_left ? ft.nodes[idx].left : ft.nodes[idx].right;
  }

  {
    tree_detail::setAllNormal(ft);
    auto s = tree_detail::snap(ft, {});
    s.status_text = std::to_string(target) + " not found — nothing to delete";
    s.trace_entry = "not found";
    s.current_line = 9;
    rec.steps.push_back(std::move(s));
  }

  return rec;
}

} // namespace viz
