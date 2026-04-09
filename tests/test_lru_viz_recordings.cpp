#include "lru_recorder.h"

#include <gtest/gtest.h>

static const std::vector<std::pair<int, int>> kInitial = {{1, 10}, {2, 20}, {3, 30}};

TEST(LruViz, GetHitHasSteps) {
  auto rec = viz::record_lru_get(kInitial, 4, 2);
  EXPECT_FALSE(rec.steps.empty());
}

TEST(LruViz, GetHitMovesToHead) {
  auto rec = viz::record_lru_get(kInitial, 4, 2);
  const auto& last = rec.steps.back();
  EXPECT_FALSE(last.list.empty());
  EXPECT_EQ(last.list[0].key, 2);
}

TEST(LruViz, GetMiss) {
  auto rec = viz::record_lru_get(kInitial, 4, 99);
  const auto& last = rec.steps.back();
  EXPECT_NE(last.status_text.find("miss"), std::string::npos);
}

TEST(LruViz, PutNewNoEviction) {
  auto rec = viz::record_lru_put(kInitial, 4, 4, 40);
  const auto& last = rec.steps.back();
  EXPECT_EQ(last.list.size(), 4u);
  EXPECT_EQ(last.list[0].key, 4);
}

TEST(LruViz, PutNewWithEviction) {
  auto rec = viz::record_lru_put(kInitial, 3, 4, 40);
  const auto& last = rec.steps.back();
  EXPECT_EQ(last.list.size(), 3u);
  EXPECT_EQ(last.list[0].key, 4);
}

TEST(LruViz, PutExistingUpdates) {
  auto rec = viz::record_lru_put(kInitial, 4, 2, 99);
  const auto& last = rec.steps.back();
  EXPECT_EQ(last.list[0].key, 2);
  EXPECT_EQ(last.list[0].value, 99);
}

TEST(LruViz, IdleHasOneStep) {
  auto rec = viz::record_lru_idle(kInitial, 4);
  EXPECT_EQ(rec.steps.size(), 1u);
}

TEST(LruViz, EvictionShowsEvictedState) {
  auto rec = viz::record_lru_put(kInitial, 3, 4, 40);
  bool found_evicted = false;
  for (const auto& step : rec.steps) {
    for (const auto& node : step.list) {
      if (node.state == viz::LruNodeState::kEvicted)
        found_evicted = true;
    }
  }
  EXPECT_TRUE(found_evicted);
}
