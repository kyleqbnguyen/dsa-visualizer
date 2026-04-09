#pragma once

#include "maze_snapshot.h"

#include <string>
#include <vector>

namespace viz {

namespace maze_detail {

inline auto makeGrid(int rows, int cols, const std::vector<bool>& walls,
                     int sr, int sc, int er, int ec)
    -> std::vector<MazeCell> {
  std::vector<MazeCell> cells(rows * cols);
  for (int i = 0; i < rows * cols; ++i)
    cells[i].state = walls[i] ? MazeCellState::kWall : MazeCellState::kOpen;
  cells[sr * cols + sc].state = MazeCellState::kStart;
  cells[er * cols + ec].state = MazeCellState::kEnd;
  return cells;
}

} // namespace maze_detail

inline auto record_maze_solve(const std::vector<std::string>& grid_strings,
                              int sr, int sc, int er, int ec)
    -> MazeRecording {
  MazeRecording rec;
  rec.title = "Maze Solver — DFS";

  int rows = static_cast<int>(grid_strings.size());
  int cols = rows > 0 ? static_cast<int>(grid_strings[0].size()) : 0;

  std::vector<bool> walls(rows * cols, false);
  for (int r = 0; r < rows; ++r)
    for (int c = 0; c < cols && c < static_cast<int>(grid_strings[r].size());
         ++c)
      walls[r * cols + c] = (grid_strings[r][c] == '#');

  auto base_cells = maze_detail::makeGrid(rows, cols, walls, sr, sc, er, ec);

  auto current_cells = base_cells;

  {
    MazeStepSnapshot snap;
    snap.rows = rows;
    snap.cols = cols;
    snap.cells = current_cells;
    snap.current_row = sr;
    snap.current_col = sc;
    snap.depth = 0;
    snap.status_text = "Start at (" + std::to_string(sr) + "," +
                       std::to_string(sc) + ") → target (" +
                       std::to_string(er) + "," + std::to_string(ec) + ")";
    snap.trace_entry = "solve(start)";
    snap.current_line = -1;
    rec.steps.push_back(std::move(snap));
  }

  std::vector<bool> seen(rows * cols, false);
  seen[sr * cols + sc] = true;

  struct Frame {
    int r, c, dir;
  };

  std::vector<Frame> stack;
  stack.push_back({sr, sc, 0});
  current_cells[sr * cols + sc].state = MazeCellState::kOnStack;

  constexpr int dr[] = {-1, 1, 0, 0};
  constexpr int dc[] = {0, 0, -1, 1};
  constexpr const char* dir_names[] = {"up", "down", "left", "right"};

  bool solved = false;

  while (!stack.empty() && !solved) {
    auto& frame = stack.back();

    if (frame.r == er && frame.c == ec) {
      for (auto& f : stack)
        current_cells[f.r * cols + f.c].state = MazeCellState::kPath;
      current_cells[sr * cols + sc].state = MazeCellState::kStart;
      current_cells[er * cols + ec].state = MazeCellState::kEnd;

      MazeStepSnapshot snap;
      snap.rows = rows;
      snap.cols = cols;
      snap.cells = current_cells;
      snap.current_row = er;
      snap.current_col = ec;
      snap.depth = static_cast<int>(stack.size());
      snap.status_text = "Reached end! Path found (length " +
                         std::to_string(stack.size()) + ")";
      snap.trace_entry = "FOUND path";
      snap.current_line = 3;
      rec.steps.push_back(std::move(snap));
      solved = true;
      break;
    }

    if (frame.dir >= 4) {
      current_cells[frame.r * cols + frame.c].state = MazeCellState::kDeadEnd;
      if (frame.r == sr && frame.c == sc)
        current_cells[frame.r * cols + frame.c].state = MazeCellState::kStart;

      {
        MazeStepSnapshot snap;
        snap.rows = rows;
        snap.cols = cols;
        snap.cells = current_cells;
        snap.current_row = frame.r;
        snap.current_col = frame.c;
        snap.depth = static_cast<int>(stack.size());
        snap.status_text = "Backtrack from (" + std::to_string(frame.r) + "," +
                           std::to_string(frame.c) + ")";
        snap.trace_entry = "backtrack (" + std::to_string(frame.r) + "," +
                           std::to_string(frame.c) + ")";
        snap.current_line = 8;
        snap.variables = {{"row", std::to_string(frame.r)},
                          {"col", std::to_string(frame.c)},
                          {"depth", std::to_string(stack.size())}};
        rec.steps.push_back(std::move(snap));
      }

      stack.pop_back();
      continue;
    }

    int nr = frame.r + dr[frame.dir];
    int nc = frame.c + dc[frame.dir];
    const char* dname = dir_names[frame.dir];
    ++frame.dir;

    if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
      continue;
    }
    if (walls[nr * cols + nc]) {
      continue;
    }
    if (seen[nr * cols + nc]) {
      continue;
    }

    seen[nr * cols + nc] = true;
    current_cells[nr * cols + nc].state = MazeCellState::kCurrent;

    {
      MazeStepSnapshot snap;
      snap.rows = rows;
      snap.cols = cols;
      snap.cells = current_cells;
      snap.current_row = nr;
      snap.current_col = nc;
      snap.depth = static_cast<int>(stack.size()) + 1;
      snap.status_text = "Explore (" + std::to_string(nr) + "," +
                         std::to_string(nc) + ") — " + dname;
      snap.trace_entry = "visit (" + std::to_string(nr) + "," +
                         std::to_string(nc) + ") " + dname;
      snap.current_line = 5;
      snap.variables = {{"row", std::to_string(nr)},
                        {"col", std::to_string(nc)},
                        {"depth", std::to_string(stack.size() + 1)},
                        {"dir", dname}};
      rec.steps.push_back(std::move(snap));
    }

    current_cells[nr * cols + nc].state = MazeCellState::kOnStack;
    stack.push_back({nr, nc, 0});
  }

  if (!solved) {
    MazeStepSnapshot snap;
    snap.rows = rows;
    snap.cols = cols;
    snap.cells = current_cells;
    snap.current_row = sr;
    snap.current_col = sc;
    snap.depth = 0;
    snap.status_text = "No path found — maze is unsolvable";
    snap.trace_entry = "UNSOLVABLE";
    snap.current_line = 9;
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

} // namespace viz
