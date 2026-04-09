#pragma once

#include <string>
#include <vector>

namespace dsa {

struct Maze {
  int rows = 0;
  int cols = 0;
  std::vector<bool> walls;

  auto isWall(int r, int c) const -> bool {
    if (r < 0 || r >= rows || c < 0 || c >= cols)
      return true;
    return walls[r * cols + c];
  }

  void setWall(int r, int c, bool val) {
    if (r >= 0 && r < rows && c >= 0 && c < cols)
      walls[r * cols + c] = val;
  }

  static auto fromStrings(const std::vector<std::string>& grid) -> Maze {
    // TODO: implement ('#' represents a wall)
    (void)grid;
    return Maze{};
  }
};

inline auto solveMaze(const Maze& maze, int sr, int sc, int er, int ec)
    -> std::vector<std::pair<int, int>> {
  // TODO: implement
  (void)maze;
  (void)sr;
  (void)sc;
  (void)er;
  (void)ec;
  return {};
}

} // namespace dsa
