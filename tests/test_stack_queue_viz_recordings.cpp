#include "stack_queue_recorder.h"
#include <gtest/gtest.h>

// ──────────────────────────────────────────
//  Stack — Push
// ──────────────────────────────────────────

TEST(StackViz, PushHasSteps) {
  auto rec = viz::record_stack_push({5, 3, 8}, 10);
  EXPECT_FALSE(rec.steps.empty());
}

TEST(StackViz, PushFinalSnapshotHasNPlusOneNodes) {
  auto rec = viz::record_stack_push({5, 3, 8}, 10);
  const auto &final_snap = rec.steps.back();
  EXPECT_EQ(final_snap.nodes.size(), 4u);
}

TEST(StackViz, PushFinalTopNodeIsDone) {
  auto rec = viz::record_stack_push({5, 3, 8}, 10);
  const auto &final_snap = rec.steps.back();
  EXPECT_EQ(final_snap.nodes.back().state, viz::ListNodeState::kDone);
  EXPECT_EQ(final_snap.nodes.back().value, 10);
}

TEST(StackViz, PushOntoEmptyStack) {
  auto rec = viz::record_stack_push({}, 42);
  EXPECT_FALSE(rec.steps.empty());
  const auto &final_snap = rec.steps.back();
  EXPECT_EQ(final_snap.nodes.size(), 1u);
  EXPECT_EQ(final_snap.nodes.back().value, 42);
}

// ──────────────────────────────────────────
//  Stack — Pop
// ──────────────────────────────────────────

TEST(StackViz, PopHasSteps) {
  auto rec = viz::record_stack_pop({5, 3, 8, 10});
  EXPECT_FALSE(rec.steps.empty());
}

TEST(StackViz, PopFinalSnapshotHasNMinusOneNodes) {
  auto rec = viz::record_stack_pop({5, 3, 8, 10});
  const auto &final_snap = rec.steps.back();
  EXPECT_EQ(final_snap.nodes.size(), 3u);
}

TEST(StackViz, PopFinalSnapshotHasNoRemovedNodes) {
  auto rec = viz::record_stack_pop({5, 3, 8, 10});
  const auto &final_snap = rec.steps.back();
  for (const auto &node : final_snap.nodes) {
    EXPECT_NE(node.state, viz::ListNodeState::kRemoved);
  }
}

TEST(StackViz, PopEmptyStackHasTwoSteps) {
  auto rec = viz::record_stack_pop({});
  // Initial + empty-error step
  EXPECT_GE(rec.steps.size(), 2u);
}

// ──────────────────────────────────────────
//  Queue — Enqueue
// ──────────────────────────────────────────

TEST(QueueViz, EnqueueHasSteps) {
  auto rec = viz::record_queue_enqueue({5, 3, 8}, 10);
  EXPECT_FALSE(rec.steps.empty());
}

TEST(QueueViz, EnqueueFinalSnapshotHasNPlusOneNodes) {
  auto rec = viz::record_queue_enqueue({5, 3, 8}, 10);
  const auto &final_snap = rec.steps.back();
  EXPECT_EQ(final_snap.nodes.size(), 4u);
}

TEST(QueueViz, EnqueueFinalRearNodeIsDone) {
  auto rec = viz::record_queue_enqueue({5, 3, 8}, 10);
  const auto &final_snap = rec.steps.back();
  EXPECT_EQ(final_snap.nodes.back().state, viz::ListNodeState::kDone);
  EXPECT_EQ(final_snap.nodes.back().value, 10);
}

TEST(QueueViz, EnqueueOntoEmptyQueue) {
  auto rec = viz::record_queue_enqueue({}, 42);
  EXPECT_FALSE(rec.steps.empty());
  const auto &final_snap = rec.steps.back();
  EXPECT_EQ(final_snap.nodes.size(), 1u);
  EXPECT_EQ(final_snap.nodes.back().value, 42);
}

// ──────────────────────────────────────────
//  Queue — Dequeue
// ──────────────────────────────────────────

TEST(QueueViz, DequeueHasSteps) {
  auto rec = viz::record_queue_dequeue({5, 3, 8, 10});
  EXPECT_FALSE(rec.steps.empty());
}

TEST(QueueViz, DequeueFinalSnapshotHasNMinusOneNodes) {
  auto rec = viz::record_queue_dequeue({5, 3, 8, 10});
  const auto &final_snap = rec.steps.back();
  EXPECT_EQ(final_snap.nodes.size(), 3u);
}

TEST(QueueViz, DequeueFinalSnapshotFrontIsCorrect) {
  auto rec = viz::record_queue_dequeue({5, 3, 8, 10});
  const auto &final_snap = rec.steps.back();
  // 5 was dequeued, so new front should be 3
  EXPECT_EQ(final_snap.nodes.front().value, 3);
}

TEST(QueueViz, DequeueEmptyQueueHasTwoSteps) {
  auto rec = viz::record_queue_dequeue({});
  EXPECT_GE(rec.steps.size(), 2u);
}
