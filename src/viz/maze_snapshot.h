#pragma once

#include <string>
#include <vector>

namespace viz {

enum class MazeCellState {
  kWall,
  kOpen,
  kVisited,
  kOnStack,
  kDeadEnd,
  kPath,
  kStart,
  kEnd,
  kCurrent,
};

struct MazeCell {
  MazeCellState state = MazeCellState::kOpen;
};

struct MazeStepSnapshot {
  int rows = 0;
  int cols = 0;
  std::vector<MazeCell> cells;

  int current_row = -1;
  int current_col = -1;
  int depth = 0;

  std::string status_text;
  std::string trace_entry;
  int current_line = -1;
  std::vector<std::pair<std::string, std::string>> variables;
};

struct MazeRecording {
  std::string title;
  std::string algorithm_name = "maze_solver";
  std::vector<MazeStepSnapshot> steps;
};

} // namespace viz
