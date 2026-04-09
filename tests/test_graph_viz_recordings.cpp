#include "graph_recorder.h"
#include "test_cases.h"

#include <gtest/gtest.h>

static auto getTestGraph() -> viz::GraphDef {
  auto presets = viz::graph_presets();
  return presets[0].def;
}

TEST(GraphViz, BfsHasSteps) {
  auto rec = viz::record_graph_bfs(getTestGraph(), 0);
  EXPECT_FALSE(rec.steps.empty());
}

TEST(GraphViz, BfsVisitsNodes) {
  auto rec = viz::record_graph_bfs(getTestGraph(), 0);
  EXPECT_GE(rec.steps.size(), 3u);
}

TEST(GraphViz, DfsHasSteps) {
  auto rec = viz::record_graph_dfs(getTestGraph(), 0);
  EXPECT_FALSE(rec.steps.empty());
}

TEST(GraphViz, DfsVisitsNodes) {
  auto rec = viz::record_graph_dfs(getTestGraph(), 0);
  EXPECT_GE(rec.steps.size(), 3u);
}

TEST(GraphViz, DijkstraHasSteps) {
  auto presets = viz::graph_presets();
  auto& weighted = presets.size() > 2 ? presets[2].def : presets[0].def;
  auto rec = viz::record_graph_dijkstra(weighted, 0);
  EXPECT_FALSE(rec.steps.empty());
}

TEST(GraphViz, DijkstraSourceDistZero) {
  auto presets = viz::graph_presets();
  auto& weighted = presets.size() > 2 ? presets[2].def : presets[0].def;
  auto rec = viz::record_graph_dijkstra(weighted, 0);
  const auto& last = rec.steps.back();
  EXPECT_EQ(last.nodes[0].distance, 0);
}

TEST(GraphViz, IdleHasOneStep) {
  auto rec = viz::record_graph_idle(getTestGraph());
  EXPECT_EQ(rec.steps.size(), 1u);
}
