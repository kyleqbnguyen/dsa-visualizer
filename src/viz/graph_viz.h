#pragma once

#include "code_panel.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/canvas.hpp"
#include "ftxui/dom/elements.hpp"
#include "graph_code_panel.h"
#include "graph_config.h"
#include "graph_recorder.h"
#include "graph_snapshot.h"
#include "viz_controller.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <climits>
#include <string>
#include <thread>
#include <vector>

namespace viz {

inline auto graph_node_color(GraphNodeState state) -> ftxui::Color {
  switch (state) {
  case GraphNodeState::kNormal:
    return ftxui::Color::White;
  case GraphNodeState::kFrontier:
    return ftxui::Color::Cyan;
  case GraphNodeState::kActive:
    return ftxui::Color::Yellow;
  case GraphNodeState::kVisited:
    return ftxui::Color::GreenLight;
  case GraphNodeState::kPath:
    return ftxui::Color::Green;
  case GraphNodeState::kSource:
    return ftxui::Color::CyanLight;
  }
  return ftxui::Color::White;
}

inline auto graph_edge_color(GraphEdgeState state) -> ftxui::Color {
  switch (state) {
  case GraphEdgeState::kNormal:
    return ftxui::Color::GrayDark;
  case GraphEdgeState::kActive:
    return ftxui::Color::Yellow;
  case GraphEdgeState::kTree:
    return ftxui::Color::Cyan;
  case GraphEdgeState::kPath:
    return ftxui::Color::Green;
  }
  return ftxui::Color::GrayDark;
}

inline auto render_graph(const GraphStepSnapshot& snap,
                         const std::string& title) -> ftxui::Element {
  using namespace ftxui;

  std::vector<Element> content;
  if (!title.empty()) {
    content.push_back(text(title) | bold | center);
    content.push_back(separator());
  }

  if (snap.nodes.empty()) {
    content.push_back(text("(empty graph)") | center | dim);
    return vbox(std::move(content)) | border | flex;
  }

  int max_x = 0, max_y = 0;
  for (const auto& n : snap.nodes) {
    if (n.x > max_x)
      max_x = n.x;
    if (n.y > max_y)
      max_y = n.y;
  }

  int canvas_w = (max_x + 10) * 2;
  int canvas_h = (max_y + 6) * 4;
  if (canvas_w < 60)
    canvas_w = 60;
  if (canvas_h < 30)
    canvas_h = 30;

  auto c = Canvas(canvas_w, canvas_h);

  for (const auto& e : snap.edges) {
    auto col = graph_edge_color(e.state);
    int x1 = snap.nodes[e.from].x;
    int y1 = snap.nodes[e.from].y;
    int x2 = snap.nodes[e.to].x;
    int y2 = snap.nodes[e.to].y;
    c.DrawPointLine(x1, y1, x2, y2, col);

    if (e.weight > 1) {
      int mx = (x1 + x2) / 2;
      int my = (y1 + y2) / 2;
      c.DrawText(mx, my, std::to_string(e.weight), col);
    }
  }

  for (const auto& n : snap.nodes) {
    auto col = graph_node_color(n.state);
    std::string label = n.label;
    if (n.distance != INT_MAX && n.distance > 0) {
      label += "(" + std::to_string(n.distance) + ")";
    }
    c.DrawText(n.x - static_cast<int>(label.size()) / 2, n.y, label, col);
  }

  content.push_back(canvas(std::move(c)) | flex);

  if (!snap.frontier.empty()) {
    std::string f_str;
    for (int id : snap.frontier) {
      if (!f_str.empty())
        f_str += ", ";
      if (id >= 0 && id < static_cast<int>(snap.nodes.size()))
        f_str += snap.nodes[id].label;
    }
    content.push_back(text("Frontier: [" + f_str + "]") | dim | center);
  }

  if (!snap.status_text.empty()) {
    content.push_back(separator());
    content.push_back(text(snap.status_text) | center);
  }

  return vbox(std::move(content)) | border | flex;
}

inline auto render_graph_trace_panel(
    const std::vector<GraphStepSnapshot>& steps, int current_step,
    int scroll_offset, int visible_lines = 8) -> ftxui::Element {
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

inline auto render_graph_state_panel(const GraphStepSnapshot& snap,
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

inline void run_graph_visualizer(GraphRecording recording, CodePanel code,
                                 GraphConfig current_config,
                                 std::vector<GraphPreset> all_presets) {
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

  GraphConfigPanel config_panel;

  auto re_record = [&](const GraphConfig& cfg) -> GraphRecording {
    const auto& gd = all_presets[cfg.graph_preset].def;
    switch (cfg.op) {
    case GraphOp::kBfs:
      return record_graph_bfs(gd, cfg.source);
    case GraphOp::kDfs:
      return record_graph_dfs(gd, cfg.source);
    case GraphOp::kDijkstra:
      return record_graph_dijkstra(gd, cfg.source);
    default:
      return record_graph_idle(gd);
    }
  };

  auto apply_config = [&](const GraphConfigResult& result) {
    current_config = result.config;
    recording = re_record(current_config);
    code = get_graph_code_panel(current_config.op);
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

    auto viz_pane = render_graph(snap, recording.title);

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
          render_graph_state_panel(snap, ctrl) | flex_shrink);

    if (trace_visible) {
      right_panels.push_back(
          render_graph_trace_panel(
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

inline void run_graph_viz() {
  auto presets = graph_presets();
  GraphConfig config;
  config.graph_preset = 0;
  config.op = GraphOp::kNone;
  config.source = 0;

  auto recording = record_graph_idle(presets[0].def);
  auto code = get_graph_code_panel(config.op);
  run_graph_visualizer(std::move(recording), code, config, presets);
}

} // namespace viz
