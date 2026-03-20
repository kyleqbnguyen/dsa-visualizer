#include <vector>

#include <gtest/gtest.h>

#include "two_crystal_balls.h"

TEST(TwoCrystalBalls, FindsMiddleBreak) {
  std::vector<int> breaks = {0, 0, 0, 0, 1, 1, 1, 1};
  int result = dsa::two_crystal_balls(breaks);
  EXPECT_EQ(result, 4);
}

TEST(TwoCrystalBalls, FindsImmediateBreak) {
  std::vector<int> breaks = {1, 1, 1, 1};
  int result = dsa::two_crystal_balls(breaks);
  EXPECT_EQ(result, 0);
}

TEST(TwoCrystalBalls, FindsLateBreak) {
  std::vector<int> breaks = {0, 0, 0, 0, 0, 0, 0, 1};
  int result = dsa::two_crystal_balls(breaks);
  EXPECT_EQ(result, 7);
}

TEST(TwoCrystalBalls, ReturnsMinusOneWhenNeverBreaks) {
  std::vector<int> breaks = {0, 0, 0, 0, 0, 0};
  int result = dsa::two_crystal_balls(breaks);
  EXPECT_EQ(result, -1);
}

TEST(TwoCrystalBalls, EmptyArray) {
  std::vector<int> breaks = {};
  int result = dsa::two_crystal_balls(breaks);
  EXPECT_EQ(result, -1);
}

TEST(TwoCrystalBalls, SingleElementSafe) {
  std::vector<int> breaks = {0};
  int result = dsa::two_crystal_balls(breaks);
  EXPECT_EQ(result, -1);
}

TEST(TwoCrystalBalls, SingleElementBreaks) {
  std::vector<int> breaks = {1};
  int result = dsa::two_crystal_balls(breaks);
  EXPECT_EQ(result, 0);
}
