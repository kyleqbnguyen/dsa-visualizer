#pragma once

#include "code_panel.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "maze_code_panel.h"
#include "maze_config.h"
#include "maze_recorder.h"
#include "maze_snapshot.h"
#include "viz_controller.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

namespace viz {

inline auto maze_cell_color(MazeCellState state) -> ftxui::Color {
  switch (state) {
  case MazeCellState::kWall:
    return ftxui::Color::GrayDark;
  case MazeCellState::kOpen:
    return ftxui::Color::GrayLight;
  case MazeCellState::kVisited:
    return ftxui::Color::GrayDark;
  case MazeCellState::kOnStack:
    return ftxui::Color::Cyan;
  case MazeCellState::kDeadEnd:
    return ftxui::Color::DarkRed;
  case MazeCellState::kPath:
    return ftxui::Color::Green;
  case MazeCellState::kStart:
    return ftxui::Color::CyanLight;
  case MazeCellState::kEnd:
    return ftxui::Color::MagentaLight;
  case MazeCellState::kCurrent:
    return ftxui::Color::Yellow;
  }
  return ftxui::Color::White;
}

inline auto maze_cell_char(MazeCellState state) -> std::string {
  switch (state) {
  case MazeCellState::kWall:
    return "##";
  case MazeCellState::kOpen:
    return "  ";
  case MazeCellState::kVisited:
    return "..";
  case MazeCellState::kOnStack:
    return "**";
  case MazeCellState::kDeadEnd:
    return "xx";
  case MazeCellState::kPath:
    return "**";
  case MazeCellState::kStart:
    return "S ";
  case MazeCellState::kEnd:
    return " E";
  case MazeCellState::kCurrent:
    return "@@";
  }
  return "  ";
}

inline auto render_maze(const MazeStepSnapshot& snap, const std::string& title)
    -> ftxui::Element {
  using namespace ftxui;

  std::vector<Element> content;
  if (!title.empty()) {
    content.push_back(text(title) | bold | center);
    content.push_back(separator());
  }

  if (snap.cells.empty()) {
    content.push_back(text("(no maze)") | center | dim);
    return vbox(std::move(content)) | border | flex;
  }

  for (int r = 0; r < snap.rows; ++r) {
    std::vector<Element> row_elems;
    for (int c = 0; c < snap.cols; ++c) {
      const auto& cell = snap.cells[r * snap.cols + c];
      auto col = maze_cell_color(cell.state);
      auto ch = maze_cell_char(cell.state);
      row_elems.push_back(text(ch) | color(col));
    }
    content.push_back(hbox(std::move(row_elems)));
  }

  if (!snap.status_text.empty()) {
    content.push_back(separator());
    content.push_back(text(snap.status_text) | center);
  }

  return vbox(std::move(content)) | border | flex;
}

inline auto render_maze_trace_panel(const std::vector<MazeStepSnapshot>& steps,
                                    int current_step, int scroll_offset,
                                    int visible_lines = 8) -> ftxui::Element {
  using namespace ftxui;

  std::vector<Element> rows;
  int total_entries = 0;
  for (int i = 0; i <= current_step && i < static_cast<int>(steps.size());
       ++i) {
    if (!steps[i].trace_entry.empty())
      total_entries++;
  }

  int effective_offset = scroll_offset;
  if (effective_offset < 0)
    effective_offset = std::max(0, total_entries - visible_lines);

  int entry_idx = 0;
  for (int i = 0; i <= current_step && i < static_cast<int>(steps.size());
       ++i) {
    if (steps[i].trace_entry.empty())
      continue;
    if (entry_idx >= effective_offset &&
        entry_idx < effective_offset + visible_lines) {
      bool is_current = (i == current_step);
      auto prefix = text(std::to_string(i) + ": ") | dim;
      auto entry_text = text(steps[i].trace_entry);
      if (is_current)
        rows.push_back(
            hbox({prefix, entry_text | bold | color(Color::Yellow)}));
      else
        rows.push_back(hbox({prefix, entry_text | color(Color::GrayLight)}));
    }
    entry_idx++;
  }

  while (static_cast<int>(rows.size()) < visible_lines)
    rows.push_back(text("") | dim);

  return vbox({
             text(" Trace ") | bold | center,
             separator(),
             vbox(std::move(rows)) | vscroll_indicator | frame,
             separator(),
             text(std::to_string(total_entries) + " entries") | dim | center,
         }) |
         border;
}

inline auto render_maze_state_panel(const MazeStepSnapshot& snap,
                                    const VizController& ctrl)
    -> ftxui::Element {
  using namespace ftxui;

  std::vector<Element> rows;

  int step = ctrl.current_step.load(std::memory_order_relaxed);
  int total = ctrl.total_steps.load(std::memory_order_relaxed);
  int last_step = (total > 0) ? total - 1 : 0;
  rows.push_back(
      text("Step " + std::to_string(step) + " / " + std::to_string(last_step)) |
      bold);

  std::string mode_str;
  Color mode_color;
  if (ctrl.is_finished()) {
    mode_str = "DONE";
    mode_color = Color::Green;
  } else {
    auto mode = ctrl.mode.load(std::memory_order_relaxed);
    switch (mode) {
    case VizController::Mode::CONTINUOUS:
      mode_str = "RUNNING";
      mode_color = Color::Green;
      break;
    case VizController::Mode::STEP:
      mode_str = "STEP";
      mode_color = Color::Yellow;
      break;
    case VizController::Mode::PAUSED:
      mode_str = "PAUSED";
      mode_color = Color::Red;
      break;
    }
  }
  rows.push_back(text("State: " + mode_str) | color(mode_color));
  rows.push_back(
      text("Speed: " +
           std::to_string(ctrl.speed_ms.load(std::memory_order_relaxed)) +
           "ms") |
      dim);

  if (!ctrl.test_case_label.empty()) {
    rows.push_back(separator());
    rows.push_back(text("Case: " + ctrl.test_case_label) | bold |
                   color(Color::Cyan));
  }

  rows.push_back(separator());
  rows.push_back(text("Variables") | bold | underlined);
  if (snap.variables.empty()) {
    rows.push_back(text("  (none)") | dim);
  } else {
    for (const auto& [name, value] : snap.variables) {
      rows.push_back(hbox({
          text("  " + name + " = ") | dim,
          text(value) | bold | color(Color::Yellow),
      }));
    }
  }

  return vbox({
             text(" State ") | bold | center,
             separator(),
             vbox(std::move(rows)),
         }) |
         border;
}

inline void run_maze_visualizer(MazeRecording recording, CodePanel code,
                                MazeConfig current_config) {
  using namespace ftxui;

  if (recording.steps.empty())
    return;

  auto screen = ScreenInteractive::Fullscreen();

  VizController ctrl;
  ctrl.total_steps.store(static_cast<int>(recording.steps.size()),
                         std::memory_order_relaxed);
  ctrl.current_step.store(0, std::memory_order_relaxed);
  ctrl.mode.store(VizController::Mode::PAUSED, std::memory_order_relaxed);

  std::atomic<int> trace_scroll{-1};
  bool config_open = false;

  MazeConfigPanel config_panel;

  auto re_record = [](const MazeConfig& cfg) -> MazeRecording {
    return record_maze_solve(cfg.grid, cfg.start_row, cfg.start_col,
                             cfg.end_row, cfg.end_col);
  };

  auto apply_config = [&](const MazeConfigResult& result) {
    current_config = result.config;
    recording = re_record(current_config);
    ctrl.total_steps.store(static_cast<int>(recording.steps.size()),
                           std::memory_order_relaxed);
    ctrl.reset();
    ctrl.test_case_label = result.test_case_label;
    trace_scroll.store(-1, std::memory_order_relaxed);
    config_open = false;
    screen.Post(Event::Custom);
  };

  auto close_config = [&]() {
    config_open = false;
    screen.Post(Event::Custom);
  };

  config_panel.init(apply_config, close_config);

  constexpr int kTickMs = 50;
  std::atomic<bool> running{true};

  auto tick_thread = std::thread([&] {
    while (running.load()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(kTickMs));
      if (!running.load())
        break;
      if (config_open)
        continue;
      bool advanced = ctrl.tick(kTickMs);
      if (advanced) {
        trace_scroll.store(-1, std::memory_order_relaxed);
        screen.Post(Event::Custom);
      }
    }
  });

  auto config_component = Maybe(config_panel.component, &config_open);
  auto main_container = Container::Vertical({config_component});

  auto renderer = Renderer(main_container, [&]() -> Element {
    int step = ctrl.current_step.load(std::memory_order_relaxed);
    step = std::clamp(step, 0, static_cast<int>(recording.steps.size()) - 1);
    const auto& snap = recording.steps[step];

    auto viz_pane = render_maze(snap, recording.title);

    Element left_pane;
    if (config_open) {
      left_pane = vbox({
          viz_pane,
          config_component->Render(),
      });
    } else {
      left_pane = viz_pane;
    }

    bool code_visible = ctrl.show_code.load(std::memory_order_relaxed);
    bool state_visible = ctrl.show_state.load(std::memory_order_relaxed);
    bool trace_visible = ctrl.show_trace.load(std::memory_order_relaxed);

    Elements right_panels;
    if (code_visible)
      right_panels.push_back(code.render(snap.current_line, false) | flex_grow);
    else
      right_panels.push_back(code.render(snap.current_line, true));

    if (state_visible)
      right_panels.push_back(
          render_maze_state_panel(snap, ctrl) | flex_shrink);

    if (trace_visible) {
      right_panels.push_back(render_maze_trace_panel(
                                 recording.steps, step,
                                 trace_scroll.load(std::memory_order_relaxed)) |
                             flex);
    }

    auto right_pane = vbox(std::move(right_panels)) | flex;

    auto controls = hbox({
        text(" [Space]") | bold, text(" Run ") | dim,   text("[N]") | bold,
        text(" Fwd ") | dim,     text("[B]") | bold,    text(" Back ") | dim,
        text("[R]") | bold,      text(" Reset ") | dim, text("[+/-]") | bold,
        text(" Speed ") | dim,   text("[C]") | bold,    text(" Config ") | dim,
        text("[T]") | bold,      text(" Trace ") | dim, text("[V]") | bold,
        text(" State ") | dim,   text("[D]") | bold,    text(" Code ") | dim,
        text("[Q]") | bold,      text(" Quit ") | dim,
    });

    return vbox({
        hbox({
            left_pane | flex,
            separator(),
            right_pane | size(WIDTH, EQUAL, 44),
        }) | flex,
        controls | border,
    });
  });

  auto component = CatchEvent(renderer, [&](Event event) -> bool {
    if (config_open) {
      if (event == Event::Character('q')) {
        config_open = false;
        running.store(false);
        screen.Exit();
        return true;
      }
      return false;
    }

    if (event == Event::Character('q') || event == Event::Escape) {
      running.store(false);
      screen.Exit();
      return true;
    }
    if (event == Event::Character(' ')) {
      ctrl.toggle_pause();
      screen.Post(Event::Custom);
      return true;
    }
    if (event == Event::Character('n') || event == Event::Character('N')) {
      ctrl.mode.store(VizController::Mode::STEP, std::memory_order_relaxed);
      ctrl.step_forward();
      trace_scroll.store(-1, std::memory_order_relaxed);
      screen.Post(Event::Custom);
      return true;
    }
    if (event == Event::Character('b') || event == Event::Character('B')) {
      ctrl.step_backward();
      trace_scroll.store(-1, std::memory_order_relaxed);
      screen.Post(Event::Custom);
      return true;
    }
    if (event == Event::Character('r') || event == Event::Character('R')) {
      ctrl.reset();
      trace_scroll.store(-1, std::memory_order_relaxed);
      screen.Post(Event::Custom);
      return true;
    }
    if (event == Event::Character('+') || event == Event::Character('=')) {
      ctrl.speed_up();
      screen.Post(Event::Custom);
      return true;
    }
    if (event == Event::Character('-') || event == Event::Character('_')) {
      ctrl.speed_down();
      screen.Post(Event::Custom);
      return true;
    }
    if (event == Event::Character('t') || event == Event::Character('T')) {
      ctrl.toggle_trace();
      screen.Post(Event::Custom);
      return true;
    }
    if (event == Event::Character('v') || event == Event::Character('V')) {
      ctrl.toggle_state();
      screen.Post(Event::Custom);
      return true;
    }
    if (event == Event::Character('d') || event == Event::Character('D')) {
      ctrl.toggle_code();
      screen.Post(Event::Custom);
      return true;
    }
    if (event == Event::Character('c') || event == Event::Character('C')) {
      ctrl.mode.store(VizController::Mode::PAUSED, std::memory_order_relaxed);
      config_open = true;
      screen.Post(Event::Custom);
      return true;
    }
    return false;
  });

  screen.Loop(component);

  running.store(false);
  if (tick_thread.joinable())
    tick_thread.join();
}

inline void run_maze_viz() {
  std::vector<std::string> grid = {
      "S.....#...",
      ".#.#..#...",
      ".#....#...",
      ".###.##...",
      "......#...",
      ".#.####...",
      ".#........",
      ".####.###.",
      "........#E",
  };

  MazeConfig config;
  config.grid = grid;
  config.start_row = 0;
  config.start_col = 0;
  config.end_row = 8;
  config.end_col = 9;

  auto recording = record_maze_solve(config.grid, config.start_row,
                                     config.start_col, config.end_row,
                                     config.end_col);
  auto code = maze_solve_code();
  run_maze_visualizer(std::move(recording), code, config);
}

} // namespace viz
