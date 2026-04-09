#include "heap_recorder.h"

#include <gtest/gtest.h>

TEST(HeapViz, InsertHasSteps) {
  auto rec = viz::record_heap_insert({3, 5, 8, 10, 20}, 1);
  EXPECT_FALSE(rec.steps.empty());
}

TEST(HeapViz, InsertFinalSizeCorrect) {
  auto rec = viz::record_heap_insert({3, 5, 8, 10, 20}, 1);
  EXPECT_EQ(rec.steps.back().data.size(), 6u);
}

TEST(HeapViz, InsertBubbleUp) {
  auto rec = viz::record_heap_insert({3, 5, 8, 10, 20}, 1);
  const auto& final_data = rec.steps.back().data;
  EXPECT_EQ(final_data[0], 1);
}

TEST(HeapViz, InsertNoSwap) {
  auto rec = viz::record_heap_insert({3, 5, 8, 10, 20}, 25);
  const auto& final_data = rec.steps.back().data;
  EXPECT_EQ(final_data[5], 25);
}

TEST(HeapViz, DeleteMinHasSteps) {
  auto rec = viz::record_heap_delete_min({3, 5, 8, 10, 20});
  EXPECT_FALSE(rec.steps.empty());
}

TEST(HeapViz, DeleteMinFinalSizeCorrect) {
  auto rec = viz::record_heap_delete_min({3, 5, 8, 10, 20});
  EXPECT_EQ(rec.steps.back().data.size(), 4u);
}

TEST(HeapViz, DeleteMinRemovedSmallest) {
  auto rec = viz::record_heap_delete_min({3, 5, 8, 10, 20});
  const auto& final_data = rec.steps.back().data;
  for (int v : final_data)
    EXPECT_GE(v, 5);
}

TEST(HeapViz, DeleteMinFromEmpty) {
  auto rec = viz::record_heap_delete_min({});
  EXPECT_GE(rec.steps.size(), 1u);
}

TEST(HeapViz, BuildHeapHasSteps) {
  auto rec = viz::record_heap_build({20, 15, 10, 8, 5, 3, 1});
  EXPECT_FALSE(rec.steps.empty());
}

TEST(HeapViz, BuildHeapResultIsValidHeap) {
  auto rec = viz::record_heap_build({20, 15, 10, 8, 5, 3, 1});
  const auto& data = rec.steps.back().data;
  for (int i = 1; i < static_cast<int>(data.size()); ++i) {
    int parent = (i - 1) / 2;
    EXPECT_LE(data[parent], data[i]);
  }
}

TEST(HeapViz, IdleHasOneStep) {
  auto rec = viz::record_heap_idle({3, 5, 8});
  EXPECT_EQ(rec.steps.size(), 1u);
}
