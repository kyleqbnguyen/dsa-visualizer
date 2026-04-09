#include "maze_recorder.h"

#include <gtest/gtest.h>

static const std::vector<std::string> kSimpleMaze = {
    "S...",
    ".#..",
    "..#.",
    "...E",
};

static const std::vector<std::string> kUnsolvable = {
    "S.#.",
    "..#.",
    "####",
    "...E",
};

static const std::vector<std::string> kCorridor = {
    "S...E",
};

TEST(MazeViz, SolveHasSteps) {
  auto rec = viz::record_maze_solve(kSimpleMaze, 0, 0, 3, 3);
  EXPECT_FALSE(rec.steps.empty());
}

TEST(MazeViz, SolveFindsPath) {
  auto rec = viz::record_maze_solve(kSimpleMaze, 0, 0, 3, 3);
  const auto& last = rec.steps.back();
  EXPECT_NE(last.status_text.find("Path found"), std::string::npos);
}

TEST(MazeViz, UnsolvableMaze) {
  auto rec = viz::record_maze_solve(kUnsolvable, 0, 0, 3, 3);
  const auto& last = rec.steps.back();
  EXPECT_NE(last.status_text.find("unsolvable"), std::string::npos);
}

TEST(MazeViz, CorridorMaze) {
  auto rec = viz::record_maze_solve(kCorridor, 0, 0, 0, 4);
  const auto& last = rec.steps.back();
  EXPECT_NE(last.status_text.find("Path found"), std::string::npos);
}

TEST(MazeViz, GridDimensionsPreserved) {
  auto rec = viz::record_maze_solve(kSimpleMaze, 0, 0, 3, 3);
  EXPECT_EQ(rec.steps[0].rows, 4);
  EXPECT_EQ(rec.steps[0].cols, 4);
  EXPECT_EQ(rec.steps[0].cells.size(), 16u);
}
