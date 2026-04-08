#pragma once

#include "code_panel.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ring_buffer_code_panel.h"
#include "ring_buffer_config.h"
#include "ring_buffer_recorder.h"
#include "ring_buffer_snapshot.h"
#include "viz_controller.h"

#include <atomic>
#include <chrono>
#include <functional>
#include <string>
#include <thread>
#include <vector>

namespace viz {

namespace ring_colors {
inline constexpr auto kEmpty = ftxui::Color::GrayDark;
inline constexpr auto kOccupied = ftxui::Color::White;
inline constexpr auto kActive = ftxui::Color::Yellow;
inline constexpr auto kNew = ftxui::Color::Cyan;
inline constexpr auto kRemoved = ftxui::Color::Red;
inline constexpr auto kDone = ftxui::Color::Green;
} // namespace ring_colors

inline auto ring_slot_color(RingSlotState state) -> ftxui::Color {
  switch (state) {
  case RingSlotState::kEmpty:
    return ring_colors::kEmpty;
  case RingSlotState::kOccupied:
    return ring_colors::kOccupied;
  case RingSlotState::kActive:
    return ring_colors::kActive;
  case RingSlotState::kNew:
    return ring_colors::kNew;
  case RingSlotState::kRemoved:
    return ring_colors::kRemoved;
  case RingSlotState::kDone:
    return ring_colors::kDone;
  }
  return ring_colors::kOccupied;
}

inline auto render_ring_buffer(const RingBufferStepSnapshot& snap,
                               const std::string& title) -> ftxui::Element {
  using namespace ftxui;

  std::vector<Element> content;
  if (!title.empty()) {
    content.push_back(text(title) | bold | center);
    content.push_back(separator());
  }

  const int cap = static_cast<int>(snap.slots.size());

  std::string info = "capacity=" + std::to_string(cap) +
                     "  size=" + std::to_string(snap.size) +
                     "  head=" + std::to_string(snap.head) +
                     "  tail=" + std::to_string(snap.tail);
  content.push_back(text(info) | dim | center);
  content.push_back(separator());

  std::vector<Element> index_row;
  std::vector<Element> slot_row;
  std::vector<Element> label_row;

  for (int i = 0; i < cap; ++i) {
    const auto& slot = snap.slots[i];
    auto col = ring_slot_color(slot.state);

    std::string val_str =
        (slot.state == RingSlotState::kEmpty) ? "   " : " " + std::to_string(slot.value) + " ";

    auto box = vbox({text(val_str) | bold | color(col) | center}) |
               border | color(col);
    slot_row.push_back(box);

    index_row.push_back(
        text(" " + std::to_string(i) + " ") | dim | center);

    std::string lbl;
    if (i == snap.head && i == snap.tail && snap.size > 0) {
      lbl = "H/T";
    } else if (i == snap.head && snap.size > 0) {
      lbl = "hd ";
    } else if (i == snap.tail) {
      lbl = " tl";
    }

    if (!lbl.empty())
      label_row.push_back(text(" " + lbl + " ") | bold | color(Color::Cyan));
    else
      label_row.push_back(text("    "));
  }

  content.push_back(hbox(std::move(index_row)) | center);
  content.push_back(hbox(std::move(slot_row)) | center);
  content.push_back(hbox(std::move(label_row)) | center);

  if (!snap.status_text.empty()) {
    content.push_back(separator());
    content.push_back(text(snap.status_text) | center);
  }

  return vbox(std::move(content)) | border | flex;
}

inline auto render_ring_buffer_trace_panel(
    const std::vector<RingBufferStepSnapshot>& steps, int current_step,
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
      if (is_current) {
        rows.push_back(
            hbox({prefix, entry_text | bold | color(Color::Yellow)}));
      } else {
        rows.push_back(hbox({prefix, entry_text | color(Color::GrayLight)}));
      }
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

inline auto render_ring_buffer_state_panel(const RingBufferStepSnapshot& snap,
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

inline auto extract_ring_buffer_final_values(const RingBufferRecording& rec)
    -> std::pair<std::vector<int>, int> {
  if (rec.steps.empty())
    return {{}, 8};
  const auto& snap = rec.steps.back();
  std::vector<int> values;
  int head = snap.head;
  for (int i = 0; i < snap.size; ++i) {
    int idx = (head + i) % static_cast<int>(snap.slots.size());
    values.push_back(snap.slots[idx].value);
  }
  return {values, static_cast<int>(snap.slots.size())};
}

inline void run_ring_buffer_visualizer(RingBufferRecording recording,
                                       CodePanel code,
                                       RingBufferConfig current_config) {
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

  RingBufferConfigPanel config_panel;

  auto re_record = [](const RingBufferConfig& cfg) -> RingBufferRecording {
    switch (cfg.op) {
    case RingBufferOp::kEnqueue:
      return record_ring_buffer_enqueue(cfg.initial_values, cfg.capacity,
                                        cfg.value);
    case RingBufferOp::kDequeue:
      return record_ring_buffer_dequeue(cfg.initial_values, cfg.capacity);
    default:
      return record_ring_buffer_idle(cfg.initial_values, cfg.capacity);
    }
  };

  auto apply_config = [&](const RingBufferConfigResult& result) {
    auto [preserved_vals, preserved_cap] =
        extract_ring_buffer_final_values(recording);
    current_config = result.config;
    if (result.reset_initial_values) {
      current_config.initial_values = result.config.initial_values;
      current_config.capacity = result.config.capacity;
    } else {
      current_config.initial_values = preserved_vals;
    }
    recording = re_record(current_config);
    code = get_ring_buffer_code_panel(current_config.op);
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

    auto viz_pane = render_ring_buffer(snap, recording.title);

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
          render_ring_buffer_state_panel(snap, ctrl) | flex_shrink);

    if (trace_visible) {
      right_panels.push_back(render_ring_buffer_trace_panel(
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
      current_config.initial_values = {};
      recording = re_record(current_config);
      code = get_ring_buffer_code_panel(current_config.op);
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

inline void run_ring_buffer_viz() {
  RingBufferConfig config;
  config.capacity = 8;
  config.initial_values = {5, 3, 8};
  config.op = RingBufferOp::kNone;
  config.value = 0;

  auto recording =
      record_ring_buffer_idle(config.initial_values, config.capacity);
  auto code = get_ring_buffer_code_panel(config.op);
  run_ring_buffer_visualizer(std::move(recording), code, config);
}

} // namespace viz
