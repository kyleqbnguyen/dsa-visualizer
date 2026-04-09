#pragma once

#include "code_panel.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "lru_code_panel.h"
#include "lru_config.h"
#include "lru_recorder.h"
#include "lru_snapshot.h"
#include "viz_controller.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

namespace viz {

namespace lru_colors {
inline constexpr auto kNormal = ftxui::Color::White;
inline constexpr auto kActive = ftxui::Color::Yellow;
inline constexpr auto kNew = ftxui::Color::Cyan;
inline constexpr auto kEvicted = ftxui::Color::Red;
} // namespace lru_colors

inline auto lru_node_color(LruNodeState state) -> ftxui::Color {
  switch (state) {
  case LruNodeState::kNormal:
    return lru_colors::kNormal;
  case LruNodeState::kActive:
    return lru_colors::kActive;
  case LruNodeState::kNew:
    return lru_colors::kNew;
  case LruNodeState::kEvicted:
    return lru_colors::kEvicted;
  }
  return lru_colors::kNormal;
}

inline auto render_lru(const LruStepSnapshot& snap, const std::string& title)
    -> ftxui::Element {
  using namespace ftxui;

  std::vector<Element> content;
  if (!title.empty()) {
    content.push_back(text(title) | bold | center);
    content.push_back(separator());
  }

  content.push_back(text("Doubly Linked List  (MRU → LRU)") | bold | center);

  if (snap.list.empty()) {
    content.push_back(text("(empty)") | center | dim);
  } else {
    std::vector<Element> columns;
    for (int i = 0; i < static_cast<int>(snap.list.size()); ++i) {
      const auto& node = snap.list[i];
      auto col = lru_node_color(node.state);

      auto box = vbox({
                     text(" " + std::to_string(node.key) + ":" +
                          std::to_string(node.value) + " ") |
                         bold | color(col) | center,
                 }) |
                 border | color(col);

      std::string label;
      if (i == 0)
        label = "MRU";
      if (i == static_cast<int>(snap.list.size()) - 1) {
        if (label.empty())
          label = "LRU";
        else
          label += "/LRU";
      }
      if (node.state == LruNodeState::kActive)
        label = "hit";
      else if (node.state == LruNodeState::kNew)
        label = "new";
      else if (node.state == LruNodeState::kEvicted)
        label = "evict";

      Element label_el = label.empty()
                             ? text("") | center
                             : text(label) | bold | color(col) | center;

      columns.push_back(vbox({box, label_el}));

      if (i < static_cast<int>(snap.list.size()) - 1)
        columns.push_back(
            vbox({text(" <-> ") | dim | vcenter, text("")}));
      else
        columns.push_back(
            vbox({text(" -> NULL") | dim | vcenter, text("")}));
    }
    content.push_back(hbox(std::move(columns)) | center);
  }

  content.push_back(separator());
  content.push_back(text("Hash Map") | bold | center);

  if (snap.map.empty()) {
    content.push_back(text("(empty)") | center | dim);
  } else {
    std::vector<Element> slots;
    for (const auto& slot : snap.map) {
      auto col = slot.active ? ftxui::Color::Yellow : ftxui::Color::White;
      auto box_text = std::to_string(slot.key) + " → [" +
                      std::to_string(slot.list_index) + "]";
      slots.push_back(text(" " + box_text + " ") | border | color(col));
      slots.push_back(text(" "));
    }
    content.push_back(hbox(std::move(slots)) | center);
  }

  content.push_back(separator());
  content.push_back(
      text("capacity=" + std::to_string(snap.capacity) +
           "  size=" + std::to_string(snap.list.size())) |
      dim | center);

  if (!snap.status_text.empty()) {
    content.push_back(separator());
    content.push_back(text(snap.status_text) | center);
  }

  return vbox(std::move(content)) | border | flex;
}

inline auto render_lru_trace_panel(const std::vector<LruStepSnapshot>& steps,
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

inline auto render_lru_state_panel(const LruStepSnapshot& snap,
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

inline void run_lru_visualizer(LruRecording recording, CodePanel code,
                               LruConfig current_config) {
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

  LruConfigPanel config_panel;

  auto re_record = [](const LruConfig& cfg) -> LruRecording {
    switch (cfg.op) {
    case LruOp::kGet:
      return record_lru_get(cfg.initial_entries, cfg.capacity, cfg.key);
    case LruOp::kPut:
      return record_lru_put(cfg.initial_entries, cfg.capacity, cfg.key,
                            cfg.value);
    default:
      return record_lru_idle(cfg.initial_entries, cfg.capacity);
    }
  };

  auto apply_config = [&](const LruConfigResult& result) {
    current_config = result.config;
    if (result.reset_initial_values)
      current_config.initial_entries = result.config.initial_entries;
    else if (!recording.steps.empty()) {
      const auto& last = recording.steps.back();
      current_config.initial_entries.clear();
      for (const auto& n : last.list)
        current_config.initial_entries.push_back({n.key, n.value});
    }
    recording = re_record(current_config);
    code = get_lru_code_panel(current_config.op);
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

    auto viz_pane = render_lru(snap, recording.title);

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
          render_lru_state_panel(snap, ctrl) | flex_shrink);

    if (trace_visible) {
      right_panels.push_back(render_lru_trace_panel(
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

inline void run_lru_viz() {
  LruConfig config;
  config.initial_entries = {{1, 10}, {2, 20}, {3, 30}};
  config.capacity = 4;
  config.op = LruOp::kNone;

  auto recording = record_lru_idle(config.initial_entries, config.capacity);
  auto code = get_lru_code_panel(config.op);
  run_lru_visualizer(std::move(recording), code, config);
}

} // namespace viz
