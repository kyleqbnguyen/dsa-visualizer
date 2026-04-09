#pragma once

#include "code_panel.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/canvas.hpp"
#include "ftxui/dom/elements.hpp"
#include "trie_code_panel.h"
#include "trie_config.h"
#include "trie_recorder.h"
#include "trie_snapshot.h"
#include "viz_controller.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

namespace viz {

inline auto trie_node_color(TrieNodeState state) -> ftxui::Color {
  switch (state) {
  case TrieNodeState::kNormal:
    return ftxui::Color::White;
  case TrieNodeState::kActive:
    return ftxui::Color::Yellow;
  case TrieNodeState::kNew:
    return ftxui::Color::Green;
  case TrieNodeState::kFound:
    return ftxui::Color::Green;
  case TrieNodeState::kNotFound:
    return ftxui::Color::Red;
  case TrieNodeState::kDeleted:
    return ftxui::Color::Red;
  case TrieNodeState::kEndOfWord:
    return ftxui::Color::Cyan;
  }
  return ftxui::Color::White;
}

namespace trie_layout {

struct NodePos {
  int index;
  float x, y;
};

inline void computePositions(const std::vector<TrieNodeSnap>& nodes,
                             int node, std::vector<NodePos>& out,
                             float x, float y, float spread) {
  out.push_back({node, x, y});

  int num_children = static_cast<int>(nodes[node].children.size());
  if (num_children == 0)
    return;

  float total_width = (num_children - 1) * spread;
  float start_x = x - total_width / 2.0f;

  float child_spread = spread * 0.6f;
  if (child_spread < 5.0f)
    child_spread = 5.0f;

  for (int i = 0; i < num_children; ++i) {
    float cx = start_x + i * spread;
    computePositions(nodes, nodes[node].children[i], out, cx, y + 4,
                     child_spread);
  }
}

} // namespace trie_layout

inline auto render_trie(const TrieStepSnapshot& snap, const std::string& title)
    -> ftxui::Element {
  using namespace ftxui;

  std::vector<Element> content;
  if (!title.empty()) {
    content.push_back(text(title) | bold | center);
    content.push_back(separator());
  }

  if (snap.nodes.empty()) {
    content.push_back(text("(empty trie)") | center | dim);
    if (!snap.status_text.empty()) {
      content.push_back(separator());
      content.push_back(text(snap.status_text) | center);
    }
    return vbox(std::move(content)) | border | flex;
  }

  std::vector<trie_layout::NodePos> positions;
  trie_layout::computePositions(snap.nodes, 0, positions, 50, 1, 15.0f);

  float min_x = positions[0].x, max_x = positions[0].x, max_y = positions[0].y;
  for (const auto& p : positions) {
    if (p.x < min_x) min_x = p.x;
    if (p.x > max_x) max_x = p.x;
    if (p.y > max_y) max_y = p.y;
  }

  float offset_x = -min_x + 4;
  int canvas_w = static_cast<int>(max_x - min_x + 12) * 2;
  int canvas_h = static_cast<int>(max_y + 4) * 4;
  if (canvas_w < 40) canvas_w = 40;
  if (canvas_h < 20) canvas_h = 20;

  auto c = Canvas(canvas_w, canvas_h);

  auto findPos = [&](int idx) -> trie_layout::NodePos* {
    for (auto& p : positions) {
      if (p.index == idx) return &p;
    }
    return nullptr;
  };

  for (const auto& p : positions) {
    const auto& node = snap.nodes[p.index];
    int px = static_cast<int>(p.x + offset_x);
    int py = static_cast<int>(p.y);

    for (int child : node.children) {
      auto* cp = findPos(child);
      if (cp) {
        int cx = static_cast<int>(cp->x + offset_x);
        int cy = static_cast<int>(cp->y);
        c.DrawPointLine(px, py + 1, cx, cy - 1, Color::GrayDark);

        int mx = (px + cx) / 2;
        int my = (py + 1 + cy - 1) / 2;
        std::string edge_label(1, snap.nodes[child].edge_char);
        c.DrawText(mx, my, edge_label, Color::White);
      }
    }
  }

  for (const auto& p : positions) {
    const auto& node = snap.nodes[p.index];
    auto col = trie_node_color(node.state);
    int px = static_cast<int>(p.x + offset_x);
    int py = static_cast<int>(p.y);

    std::string label;
    if (p.index == 0) {
      label = "*";
    } else {
      label = std::string(1, node.edge_char);
    }
    if (node.is_end)
      label += "*";

    c.DrawText(px - static_cast<int>(label.size()) / 2, py, label, col);
  }

  content.push_back(canvas(std::move(c)) | flex);

  if (!snap.current_word.empty()) {
    content.push_back(
        text("Word: \"" + snap.current_word + "\"") | dim | center);
  }

  if (!snap.status_text.empty()) {
    content.push_back(separator());
    content.push_back(text(snap.status_text) | center);
  }

  return vbox(std::move(content)) | border | flex;
}

inline auto render_trie_trace_panel(
    const std::vector<TrieStepSnapshot>& steps, int current_step,
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

inline auto render_trie_state_panel(const TrieStepSnapshot& snap,
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

inline void run_trie_visualizer(TrieRecording recording, CodePanel code,
                                TrieConfig current_config) {
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

  TrieConfigPanel config_panel;

  auto re_record = [](const TrieConfig& cfg) -> TrieRecording {
    switch (cfg.op) {
    case TrieVizOp::kInsert:
      return record_trie_insert(cfg.dictionary, cfg.target_word);
    case TrieVizOp::kSearch:
      return record_trie_search(cfg.dictionary, cfg.target_word);
    case TrieVizOp::kDelete:
      return record_trie_delete(cfg.dictionary, cfg.target_word);
    default:
      return record_trie_idle(cfg.dictionary);
    }
  };

  auto apply_config = [&](const TrieConfigResult& result) {
    current_config = result.config;
    if (result.reset_initial_values)
      current_config.dictionary = result.config.dictionary;
    recording = re_record(current_config);
    code = get_trie_code_panel(current_config.op);
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

    auto viz_pane = render_trie(snap, recording.title);

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
          render_trie_state_panel(snap, ctrl) | flex_shrink);

    if (trace_visible) {
      right_panels.push_back(
          render_trie_trace_panel(
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

inline void run_trie_viz() {
  TrieConfig config;
  config.dictionary = {"app", "apple", "ape", "bat", "ball", "bad"};
  config.op = TrieVizOp::kNone;

  auto recording = record_trie_idle(config.dictionary);
  auto code = get_trie_code_panel(config.op);
  run_trie_visualizer(std::move(recording), code, config);
}

} // namespace viz
