#include "tree_recorder.h"

#include <gtest/gtest.h>

static const std::vector<int> kBalancedBst = {8, 4, 12, 2, 6, 10, 14};

TEST(TreeViz, PreorderHasSteps) {
  auto rec = viz::record_tree_preorder(kBalancedBst);
  EXPECT_FALSE(rec.steps.empty());
}

TEST(TreeViz, PreorderVisitsAllNodes) {
  auto rec = viz::record_tree_preorder(kBalancedBst);
  const auto& last = rec.steps.back();
  EXPECT_EQ(last.visit_order.size(), 7u);
}

TEST(TreeViz, PreorderFirstVisitIsRoot) {
  auto rec = viz::record_tree_preorder(kBalancedBst);
  const auto& last = rec.steps.back();
  EXPECT_FALSE(last.visit_order.empty());
  EXPECT_EQ(last.nodes[last.visit_order[0]].value, 8);
}

TEST(TreeViz, InorderHasSteps) {
  auto rec = viz::record_tree_inorder(kBalancedBst);
  EXPECT_FALSE(rec.steps.empty());
}

TEST(TreeViz, InorderVisitsAllNodes) {
  auto rec = viz::record_tree_inorder(kBalancedBst);
  const auto& last = rec.steps.back();
  EXPECT_EQ(last.visit_order.size(), 7u);
}

TEST(TreeViz, InorderProducesSortedOrder) {
  auto rec = viz::record_tree_inorder(kBalancedBst);
  const auto& last = rec.steps.back();
  std::vector<int> values;
  for (int idx : last.visit_order)
    values.push_back(last.nodes[idx].value);
  EXPECT_TRUE(std::is_sorted(values.begin(), values.end()));
}

TEST(TreeViz, PostorderHasSteps) {
  auto rec = viz::record_tree_postorder(kBalancedBst);
  EXPECT_FALSE(rec.steps.empty());
}

TEST(TreeViz, PostorderLastVisitIsRoot) {
  auto rec = viz::record_tree_postorder(kBalancedBst);
  const auto& last = rec.steps.back();
  EXPECT_FALSE(last.visit_order.empty());
  EXPECT_EQ(last.nodes[last.visit_order.back()].value, 8);
}

TEST(TreeViz, BfsHasSteps) {
  auto rec = viz::record_tree_bfs(kBalancedBst);
  EXPECT_FALSE(rec.steps.empty());
}

TEST(TreeViz, BfsVisitsAllNodes) {
  auto rec = viz::record_tree_bfs(kBalancedBst);
  const auto& last = rec.steps.back();
  EXPECT_EQ(last.visit_order.size(), 7u);
}

TEST(TreeViz, BfsVisitsLevelOrder) {
  auto rec = viz::record_tree_bfs(kBalancedBst);
  const auto& last = rec.steps.back();
  std::vector<int> values;
  for (int idx : last.visit_order)
    values.push_back(last.nodes[idx].value);
  EXPECT_EQ(values[0], 8);
  EXPECT_EQ(values[1], 4);
  EXPECT_EQ(values[2], 12);
}

TEST(TreeViz, BstFindExisting) {
  auto rec = viz::record_bst_find(kBalancedBst, 6);
  EXPECT_FALSE(rec.steps.empty());
  bool found = false;
  for (const auto& s : rec.steps) {
    for (const auto& n : s.nodes) {
      if (n.state == viz::TreeNodeState::kFound && n.value == 6)
        found = true;
    }
  }
  EXPECT_TRUE(found);
}

TEST(TreeViz, BstFindMissing) {
  auto rec = viz::record_bst_find(kBalancedBst, 99);
  EXPECT_FALSE(rec.steps.empty());
  const auto& last = rec.steps.back();
  EXPECT_NE(last.status_text.find("not found"), std::string::npos);
}

TEST(TreeViz, BstInsertAddsNode) {
  auto rec = viz::record_bst_insert(kBalancedBst, 5);
  EXPECT_FALSE(rec.steps.empty());
  const auto& last = rec.steps.back();
  bool has_new = false;
  for (const auto& n : last.nodes) {
    if (n.value == 5 && n.state == viz::TreeNodeState::kInserted)
      has_new = true;
  }
  EXPECT_TRUE(has_new);
}

TEST(TreeViz, BstInsertIntoEmpty) {
  auto rec = viz::record_bst_insert({}, 42);
  EXPECT_FALSE(rec.steps.empty());
  EXPECT_EQ(rec.steps.back().nodes.size(), 1u);
}

TEST(TreeViz, BstDeleteLeaf) {
  auto rec = viz::record_bst_delete(kBalancedBst, 2);
  EXPECT_FALSE(rec.steps.empty());
  EXPECT_NE(rec.steps.back().status_text.find("complete"), std::string::npos);
}

TEST(TreeViz, BstDeleteNotFound) {
  auto rec = viz::record_bst_delete(kBalancedBst, 99);
  EXPECT_FALSE(rec.steps.empty());
  EXPECT_NE(rec.steps.back().status_text.find("not found"), std::string::npos);
}

TEST(TreeViz, IdleHasOneStep) {
  auto rec = viz::record_tree_idle(kBalancedBst);
  EXPECT_EQ(rec.steps.size(), 1u);
}

TEST(TreeViz, EmptyTreePreorder) {
  auto rec = viz::record_tree_preorder({});
  EXPECT_FALSE(rec.steps.empty());
}
