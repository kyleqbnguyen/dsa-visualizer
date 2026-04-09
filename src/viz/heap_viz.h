#pragma once

#include "code_panel.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/canvas.hpp"
#include "ftxui/dom/elements.hpp"
#include "heap_code_panel.h"
#include "heap_config.h"
#include "heap_recorder.h"
#include "heap_snapshot.h"
#include "viz_controller.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <functional>
#include <string>
#include <thread>
#include <vector>

namespace viz {

namespace heap_colors {
inline constexpr auto kNormal = ftxui::Color::White;
inline constexpr auto kActive = ftxui::Color::Yellow;
inline constexpr auto kCompare = ftxui::Color::Cyan;
} // namespace heap_colors

inline auto render_heap_tree(const HeapStepSnapshot& snap) -> ftxui::Element {
  using namespace ftxui;

  int n = static_cast<int>(snap.data.size());
  if (n == 0)
    return text("(empty heap)") | center | dim;

  int depth = 0;
  {
    int tmp = n;
    while (tmp > 0) {
      ++depth;
      tmp = (tmp - 1) / 2;
    }
  }
  depth = static_cast<int>(std::log2(n)) + 1;

  constexpr int kNodeWidth = 6;
  int max_nodes_bottom = 1 << (depth - 1);
  int canvas_width = max_nodes_bottom * kNodeWidth * 2;
  if (canvas_width < 40)
    canvas_width = 40;
  int canvas_height = depth * 4 + 2;

  auto c = Canvas(canvas_width * 2, canvas_height * 4);

  struct NodePos {
    int x = 0;
    int y = 0;
  };
  std::vector<NodePos> positions(n);

  for (int i = 0; i < n; ++i) {
    int level = static_cast<int>(std::log2(i + 1));
    int pos_in_level = i - (1 << level) + 1;
    int nodes_in_level = 1 << level;

    int section_width = canvas_width / nodes_in_level;
    int x = section_width * pos_in_level + section_width / 2;
    int y = level * 4 + 1;

    positions[i] = {x, y};
  }

  for (int i = 0; i < n; ++i) {
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n) {
      c.DrawPointLine(positions[i].x, positions[i].y + 1, positions[left].x,
                      positions[left].y - 1, Color::GrayDark);
    }
    if (right < n) {
      c.DrawPointLine(positions[i].x, positions[i].y + 1, positions[right].x,
                      positions[right].y - 1, Color::GrayDark);
    }
  }

  for (int i = 0; i < n; ++i) {
    Color col = heap_colors::kNormal;
    if (i == snap.highlight_a)
      col = heap_colors::kActive;
    else if (i == snap.highlight_b)
      col = heap_colors::kCompare;

    std::string label = std::to_string(snap.data[i]);
    c.DrawText(positions[i].x - static_cast<int>(label.size()) / 2,
               positions[i].y, label, col);
  }

  return canvas(std::move(c));
}

inline auto render_heap_array(const HeapStepSnapshot& snap) -> ftxui::Element {
  using namespace ftxui;

  if (snap.data.empty())
    return text("(empty)") | center | dim;

  int max_val = *std::max_element(snap.data.begin(), snap.data.end());
  constexpr int kMaxBarHeight = 8;

  std::vector<Element> bars;
  for (int i = 0; i < static_cast<int>(snap.data.size()); ++i) {
    Color bar_color = heap_colors::kNormal;
    if (i == snap.highlight_a)
      bar_color = heap_colors::kActive;
    else if (i == snap.highlight_b)
      bar_color = heap_colors::kCompare;

    int height = (max_val > 0) ? (snap.data[i] * kMaxBarHeight / max_val) : 0;
    if (height < 1)
      height = 1;

    std::vector<Element> rows;
    for (int row = kMaxBarHeight; row > 0; --row) {
      if (row <= height)
        rows.push_back(text("██") | color(bar_color));
      else
        rows.push_back(text("  "));
    }
    rows.push_back(text(std::to_string(snap.data[i])) | center);
    bars.push_back(vbox(std::move(rows)) | center);
    bars.push_back(text(" "));
  }

  return hbox(std::move(bars)) | center;
}

inline auto render_heap(const HeapStepSnapshot& snap, const std::string& title)
    -> ftxui::Element {
  using namespace ftxui;

  std::vector<Element> content;
  if (!title.empty()) {
    content.push_back(text(title) | bold | center);
    content.push_back(separator());
  }

  content.push_back(render_heap_tree(snap) | flex);
  content.push_back(separator());
  content.push_back(render_heap_array(snap));

  if (!snap.status_text.empty()) {
    content.push_back(separator());
    content.push_back(text(snap.status_text) | center);
  }

  return vbox(std::move(content)) | border | flex;
}

inline auto render_heap_trace_panel(const std::vector<HeapStepSnapshot>& steps,
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

inline auto render_heap_state_panel(const HeapStepSnapshot& snap,
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

inline void run_heap_visualizer(HeapRecording recording, CodePanel code,
                                HeapConfig current_config) {
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

  HeapConfigPanel config_panel;

  auto re_record = [](const HeapConfig& cfg) -> HeapRecording {
    switch (cfg.op) {
    case HeapOp::kInsert:
      return record_heap_insert(cfg.initial_values, cfg.value);
    case HeapOp::kDeleteMin:
      return record_heap_delete_min(cfg.initial_values);
    case HeapOp::kBuildHeap:
      return record_heap_build(cfg.initial_values);
    default:
      return record_heap_idle(cfg.initial_values);
    }
  };

  auto apply_config = [&](const HeapConfigResult& result) {
    current_config = result.config;
    if (result.reset_initial_values) {
      current_config.initial_values = result.config.initial_values;
    } else if (!recording.steps.empty()) {
      current_config.initial_values = recording.steps.back().data;
    }
    recording = re_record(current_config);
    code = get_heap_code_panel(current_config.op);
    ctrl.total_steps.store(static_cast<int>(recording.steps.size()),
                           std::memory_order_relaxed);
    ctrl.reset();
    ctrl.test_case_label = result.test_case_label;
    trace_scroll.store(-1, std::memory_order_relaxed);
    config_panel.sync_op(current_config.op);
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

    auto viz_pane = render_heap(snap, recording.title);

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
          render_heap_state_panel(snap, ctrl) | flex_shrink);

    if (trace_visible) {
      right_panels.push_back(render_heap_trace_panel(
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
        text("[X]") | bold,      text(" Clear ") | dim, text("[T]") | bold,
        text(" Trace ") | dim,   text("[V]") | bold,    text(" State ") | dim,
        text("[D]") | bold,      text(" Code ") | dim,  text("[Q]") | bold,
        text(" Quit ") | dim,
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
    if (event == Event::Character('x') || event == Event::Character('X')) {
      current_config.initial_values = {10, 5, 20, 3, 8};
      recording = re_record(current_config);
      code = get_heap_code_panel(current_config.op);
      ctrl.total_steps.store(static_cast<int>(recording.steps.size()),
                             std::memory_order_relaxed);
      ctrl.reset();
      ctrl.test_case_label = "";
      trace_scroll.store(-1, std::memory_order_relaxed);
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

inline void run_heap_viz() {
  HeapConfig config;
  config.initial_values = {3, 5, 8, 10, 20};
  config.op = HeapOp::kNone;
  config.value = 0;

  auto recording = record_heap_idle(config.initial_values);
  auto code = get_heap_code_panel(config.op);
  run_heap_visualizer(std::move(recording), code, config);
}

} // namespace viz
