#include "ring_buffer_recorder.h"

#include <gtest/gtest.h>

// ──────────────────────────────────────────
//  Enqueue
// ──────────────────────────────────────────

TEST(RingBufferViz, EnqueueHasSteps) {
  auto rec = viz::record_ring_buffer_enqueue({5, 3, 8}, 8, 10);
  EXPECT_FALSE(rec.steps.empty());
}

TEST(RingBufferViz, EnqueueFinalSnapshotHasCorrectSize) {
  auto rec = viz::record_ring_buffer_enqueue({5, 3, 8}, 8, 10);
  const auto& snap = rec.steps.back();
  EXPECT_EQ(snap.size, 4);
}

TEST(RingBufferViz, EnqueueFinalSlotHasDoneState) {
  auto rec = viz::record_ring_buffer_enqueue({5, 3, 8}, 8, 10);
  const auto& snap = rec.steps.back();
  EXPECT_EQ(snap.slots[3].state, viz::RingSlotState::kDone);
  EXPECT_EQ(snap.slots[3].value, 10);
}

TEST(RingBufferViz, EnqueueOntoEmpty) {
  auto rec = viz::record_ring_buffer_enqueue({}, 8, 42);
  EXPECT_FALSE(rec.steps.empty());
  const auto& snap = rec.steps.back();
  EXPECT_EQ(snap.size, 1);
  EXPECT_EQ(snap.slots[0].value, 42);
}

TEST(RingBufferViz, EnqueueOntoFullProducesErrorStep) {
  auto rec = viz::record_ring_buffer_enqueue({1, 2, 3, 4}, 4, 99);
  EXPECT_GE(rec.steps.size(), 2u);
  const auto& error_snap = rec.steps.back();
  EXPECT_EQ(error_snap.size, 4);
}

// ──────────────────────────────────────────
//  Dequeue
// ──────────────────────────────────────────

TEST(RingBufferViz, DequeueHasSteps) {
  auto rec = viz::record_ring_buffer_dequeue({5, 3, 8, 10}, 8);
  EXPECT_FALSE(rec.steps.empty());
}

TEST(RingBufferViz, DequeueFinalSnapshotHasCorrectSize) {
  auto rec = viz::record_ring_buffer_dequeue({5, 3, 8, 10}, 8);
  const auto& snap = rec.steps.back();
  EXPECT_EQ(snap.size, 3);
}

TEST(RingBufferViz, DequeueFinalHeadAdvanced) {
  auto rec = viz::record_ring_buffer_dequeue({5, 3, 8, 10}, 8);
  const auto& snap = rec.steps.back();
  EXPECT_EQ(snap.head, 1);
}

TEST(RingBufferViz, DequeueFinalSlot0IsEmpty) {
  auto rec = viz::record_ring_buffer_dequeue({5, 3, 8, 10}, 8);
  const auto& snap = rec.steps.back();
  EXPECT_EQ(snap.slots[0].state, viz::RingSlotState::kEmpty);
}

TEST(RingBufferViz, DequeueOnEmptyProducesErrorStep) {
  auto rec = viz::record_ring_buffer_dequeue({}, 8);
  EXPECT_GE(rec.steps.size(), 2u);
}

// ──────────────────────────────────────────
//  Idle
// ──────────────────────────────────────────

TEST(RingBufferViz, IdleHasOneStep) {
  auto rec = viz::record_ring_buffer_idle({1, 2, 3}, 8);
  EXPECT_EQ(rec.steps.size(), 1u);
}

TEST(RingBufferViz, IdleSnapshotHasCorrectCapacity) {
  auto rec = viz::record_ring_buffer_idle({1, 2, 3}, 8);
  EXPECT_EQ(rec.steps.front().slots.size(), 8u);
}

TEST(RingBufferViz, IdleSnapshotHasCorrectSize) {
  auto rec = viz::record_ring_buffer_idle({1, 2, 3}, 8);
  EXPECT_EQ(rec.steps.front().size, 3);
}
