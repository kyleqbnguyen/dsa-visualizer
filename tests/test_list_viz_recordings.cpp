#include <vector>

#include <gtest/gtest.h>

#include "list_recorder.h"

TEST(ListRecordings, SinglyAppendHasCorrectFinalState) {
  auto rec = viz::record_singly_append({3, 7, 12}, 99);
  ASSERT_FALSE(rec.steps.empty());
  EXPECT_EQ(rec.algorithm_name, "singly_linked_list");
  EXPECT_EQ(rec.op, viz::ListOp::kAppend);
  auto &last = rec.steps.back();
  ASSERT_EQ(last.nodes.size(), 4u);
  EXPECT_EQ(last.nodes.back().state, viz::ListNodeState::kDone);
  EXPECT_EQ(last.nodes.back().value, 99);
}

TEST(ListRecordings, SinglyPrependHasCorrectFinalState) {
  auto rec = viz::record_singly_prepend({3, 7, 12}, 99);
  ASSERT_FALSE(rec.steps.empty());
  EXPECT_EQ(rec.op, viz::ListOp::kPrepend);
  auto &last = rec.steps.back();
  ASSERT_EQ(last.nodes.size(), 4u);
  EXPECT_EQ(last.nodes.front().state, viz::ListNodeState::kDone);
  EXPECT_EQ(last.nodes.front().value, 99);
}

TEST(ListRecordings, SinglyInsertAtMiddle) {
  auto rec = viz::record_singly_insert_at({3, 7, 12, 5, 9}, 2, 99);
  ASSERT_FALSE(rec.steps.empty());
  EXPECT_EQ(rec.op, viz::ListOp::kInsertAt);
  auto &last = rec.steps.back();
  ASSERT_EQ(last.nodes.size(), 6u);
  EXPECT_EQ(last.nodes[2].value, 99);
  EXPECT_EQ(last.nodes[2].state, viz::ListNodeState::kDone);
}

TEST(ListRecordings, SinglyRemoveAtMiddle) {
  auto rec = viz::record_singly_remove_at({3, 7, 12, 5, 9}, 2);
  ASSERT_FALSE(rec.steps.empty());
  EXPECT_EQ(rec.op, viz::ListOp::kRemoveAt);
  auto &last = rec.steps.back();
  ASSERT_EQ(last.nodes.size(), 4u);
}

TEST(ListRecordings, SinglyGetFindsValue) {
  auto rec = viz::record_singly_get({3, 7, 12, 5, 9}, 3);
  ASSERT_FALSE(rec.steps.empty());
  EXPECT_EQ(rec.op, viz::ListOp::kGet);
  auto &last = rec.steps.back();
  EXPECT_EQ(last.nodes[3].state, viz::ListNodeState::kFound);
  EXPECT_EQ(last.nodes[3].value, 5);
}

TEST(ListRecordings, DoublyAppendHasCorrectFinalState) {
  auto rec = viz::record_doubly_append({3, 7, 12}, 99);
  ASSERT_FALSE(rec.steps.empty());
  EXPECT_EQ(rec.algorithm_name, "doubly_linked_list");
  EXPECT_EQ(rec.op, viz::ListOp::kAppend);
  auto &last = rec.steps.back();
  ASSERT_EQ(last.nodes.size(), 4u);
  EXPECT_EQ(last.nodes.back().state, viz::ListNodeState::kDone);
  EXPECT_EQ(last.nodes.back().value, 99);
}

TEST(ListRecordings, DoublyPrependHasCorrectFinalState) {
  auto rec = viz::record_doubly_prepend({3, 7, 12}, 99);
  ASSERT_FALSE(rec.steps.empty());
  auto &last = rec.steps.back();
  ASSERT_EQ(last.nodes.size(), 4u);
  EXPECT_EQ(last.nodes.front().value, 99);
  EXPECT_EQ(last.nodes.front().state, viz::ListNodeState::kDone);
}

TEST(ListRecordings, DoublyRemoveAtMiddle) {
  auto rec = viz::record_doubly_remove_at({3, 7, 12, 5, 9}, 2);
  ASSERT_FALSE(rec.steps.empty());
  auto &last = rec.steps.back();
  ASSERT_EQ(last.nodes.size(), 4u);
}

TEST(ListRecordings, DoublyGetFindsValue) {
  auto rec = viz::record_doubly_get({3, 7, 12, 5, 9}, 3);
  ASSERT_FALSE(rec.steps.empty());
  auto &last = rec.steps.back();
  EXPECT_EQ(last.nodes[3].state, viz::ListNodeState::kFound);
  EXPECT_EQ(last.nodes[3].value, 5);
}
