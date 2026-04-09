#pragma once

#include <vector>

namespace dsa {

struct TreeNode {
  int value = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
};

class BinaryTree {
public:
  BinaryTree() = default;
  ~BinaryTree() {
    for (auto* node : owned_)
      delete node;
  }

  BinaryTree(const BinaryTree&) = delete;
  auto operator=(const BinaryTree&) -> BinaryTree& = delete;
  BinaryTree(BinaryTree&&) = default;
  auto operator=(BinaryTree&&) -> BinaryTree& = default;

  auto root() const -> TreeNode* { return root_; }

  auto makeNode(int value) -> TreeNode* {
    auto* node = new TreeNode{value};
    owned_.push_back(node);
    return node;
  }

  void setRoot(TreeNode* node) { root_ = node; }

  static auto fromLevelOrder(const std::vector<int>& values) -> BinaryTree {
    // TODO: implement (-1 represents a null node)
    (void)values;
    return BinaryTree{};
  }

private:
  TreeNode* root_ = nullptr;
  std::vector<TreeNode*> owned_;
};

inline void preorderTraversal(TreeNode* node, std::vector<int>& result) {
  // TODO: implement
  (void)node;
  (void)result;
}

inline void inorderTraversal(TreeNode* node, std::vector<int>& result) {
  // TODO: implement
  (void)node;
  (void)result;
}

inline void postorderTraversal(TreeNode* node, std::vector<int>& result) {
  // TODO: implement
  (void)node;
  (void)result;
}

} // namespace dsa
