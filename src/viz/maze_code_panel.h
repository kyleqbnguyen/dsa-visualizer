#pragma once

#include "code_panel.h"

namespace viz {

inline auto maze_solve_code() -> CodePanel {
  return {
      .title = "maze_solver.h",
      .lines =
          {
              "bool solve(maze, row, col) {",               // 0
              "    if (out of bounds) return false;",        // 1
              "    if (maze[row][col]==WALL) return false;", // 2
              "    if (seen[row][col]) return false;",       // 3
              "    if (row==endR && col==endC) return true;", // 4
              "    seen[row][col] = true;",                  // 5
              "    if (solve(row-1,col)) return true;",      // 6
              "    if (solve(row+1,col)) return true;",      // 7
              "    if (solve(row,col-1)) return true;",      // 8
              "    if (solve(row,col+1)) return true;",      // 9
              "    return false;  // backtrack",             // 10
              "}",                                          // 11
          },
  };
}

} // namespace viz
