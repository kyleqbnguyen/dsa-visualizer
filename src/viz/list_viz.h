#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <string>
#include <thread>
#include <vector>

#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

#include "code_panel.h"
#include "list_code_panel.h"
#include "list_config.h"
#include "list_recorder.h"
#include "list_snapshot.h"
#include "viz_controller.h"

namespace viz {

namespace list_colors {
inline constexpr auto kNormal = ftxui::Color::White;
inline constexpr auto kActive = ftxui::Color::Yellow;
inline constexpr auto kNew = ftxui::Color::Cyan;
inline constexpr auto kTarget = ftxui::Color::Cyan;
inline constexpr auto kFound = ftxui::Color::Green;
inline constexpr auto kRemoved = ftxui::Color::Red;
inline constexpr auto kDone = ftxui::Color::White;
} // namespace list_colors

inline auto node_color(ListNodeState state) -> ftxui::Color {
  switch (state) {
  case ListNodeState::kNormal:
    return list_colors::kNormal;
  case ListNodeState::kActive:
    return list_colors::kActive;
  case ListNodeState::kNew:
    return list_colors::kNew;
  case ListNodeState::kTarget:
    return list_colors::kTarget;
  case ListNodeState::kFound:
    return list_colors::kFound;
  case ListNodeState::kRemoved:
    return list_colors::kRemoved;
  case ListNodeState::kDone:
    return list_colors::kDone;
  }
  return list_colors::kNormal;
}

inline auto render_list(const ListStepSnapshot &snap,
                        const std::string &title, bool is_doubly)
    -> ftxui::Element {
  using namespace ftxui;

  std::vector<Element> content;

  if (!title.empty()) {
    content.push_back(text(title) | bold | center);
    content.push_back(separator());
  }

  if (snap.nodes.empty()) {
    content.push_back(text("(empty list)") | center | dim);
    if (!snap.status_text.empty()) {
      content.push_back(separator());
      content.push_back(text(snap.status_text) | center);
    }
    return vbox(std::move(content)) | border | flex;
  }

  std::vector<Element> columns;

  for (int i = 0; i < static_cast<int>(snap.nodes.size()); ++i) {
    const auto &node = snap.nodes[i];
    auto col = node_color(node.state);

    auto box = vbox({
                   text(" " + std::to_string(node.value) + " ") | bold |
                       color(col) | center,
               }) |
               border | color(col);

    std::string label;
    if (node.state == ListNodeState::kActive) {
      label = "curr";
    } else if (node.state == ListNodeState::kNew) {
      label = "new";
    } else if (node.state == ListNodeState::kFound) {
      label = "found";
    } else if (node.state == ListNodeState::kRemoved) {
      label = "del";
    }

    Element cursor_el;
    if (!label.empty()) {
      cursor_el = text(label) | bold | color(col) | center;
    } else {
      cursor_el = text("") | center;
    }

    auto index_el = text("[" + std::to_string(i) + "]") | dim | center;

    columns.push_back(vbox({box, cursor_el, index_el}));

    if (i < static_cast<int>(snap.nodes.size()) - 1) {
      Element arrow;
      if (node.has_next) {
        if (is_doubly) {
          arrow = text(" <-> ") | dim;
        } else {
          arrow = text(" --> ") | dim;
        }
      } else {
        arrow = text("  ×  ") | dim | color(Color::Red);
      }
      columns.push_back(vbox({arrow | vcenter, text(""), text("")}));
    } else {
      Element tail;
      if (node.has_next) {
        tail = text(" --> ...") | dim;
      } else {
        tail = text(" --> NULL") | dim;
      }
      columns.push_back(vbox({tail | vcenter, text(""), text("")}));
    }
  }

  content.push_back(text("HEAD") | bold | dim | center);
  content.push_back(hbox(std::move(columns)) | center);

  if (!snap.status_text.empty()) {
    content.push_back(separator());
    content.push_back(text(snap.status_text) | center);
  }

  return vbox(std::move(content)) | border | flex;
}

inline auto render_list_trace_panel(const std::vector<ListStepSnapshot> &steps,
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
      if (is_current) {
        rows.push_back(
            hbox({prefix, entry_text | bold | color(Color::Yellow)}));
      } else {
        rows.push_back(
            hbox({prefix, entry_text | color(Color::GrayLight)}));
      }
    }
    entry_idx++;
  }

  while (static_cast<int>(rows.size()) < visible_lines)
    rows.push_back(text("") | dim);

  std::string scroll_info = std::to_string(total_entries) + " entries";

  return vbox({
             text(" Trace ") | bold | center,
             separator(),
             vbox(std::move(rows)) | vscroll_indicator | frame,
             separator(),
             text(scroll_info) | dim | center,
         }) |
         border;
}

inline auto render_list_state_panel(const ListStepSnapshot &snap,
                                    const VizController &ctrl)
    -> ftxui::Element {
  using namespace ftxui;

  std::vector<Element> rows;

  int step = ctrl.current_step.load(std::memory_order_relaxed);
  int total = ctrl.total_steps.load(std::memory_order_relaxed);
  int last_step = (total > 0) ? total - 1 : 0;
  rows.push_back(text("Step " + std::to_string(step) + " / " +
                       std::to_string(last_step)) |
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
  rows.push_back(text("Speed: " +
                       std::to_string(
                           ctrl.speed_ms.load(std::memory_order_relaxed)) +
                       "ms") |
                 dim);

  if (!ctrl.test_case_label.empty()) {
    rows.push_back(separator());
    rows.push_back(
        text("Case: " + ctrl.test_case_label) | bold | color(Color::Cyan));
  }

  rows.push_back(separator());
  rows.push_back(text("Variables") | bold | underlined);
  if (snap.variables.empty()) {
    rows.push_back(text("  (none)") | dim);
  } else {
    for (const auto &[name, value] : snap.variables) {
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

inline auto extract_final_values(const ListAlgorithmRecording &rec)
    -> std::vector<int> {
  if (rec.steps.empty())
    return {};
  const auto &nodes = rec.steps.back().nodes;
  std::vector<int> values;
  values.reserve(nodes.size());
  for (const auto &n : nodes)
    values.push_back(n.value);
  return values;
}

using ListRecorderFunc =
    std::function<ListAlgorithmRecording(const ListConfig &)>;

inline void run_list_visualizer(ListAlgorithmRecording recording,
                                CodePanel code,
                                ListConfig current_config,
                                ListRecorderFunc re_record,
                                bool is_doubly) {
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

  ListConfigPanel config_panel;

  auto apply_config = [&](const ListConfigResult &result) {
    auto preserved = extract_final_values(recording);
    current_config = result.config;
    if (result.reset_initial_values) {
      current_config.initial_values = result.config.initial_values;
    } else {
      current_config.initial_values = preserved;
    }
    recording = re_record(current_config);
    code = is_doubly ? get_doubly_code_panel(current_config.op)
                     : get_singly_code_panel(current_config.op);
    ctrl.total_steps.store(static_cast<int>(recording.steps.size()),
                           std::memory_order_relaxed);
    ctrl.reset();
    ctrl.test_case_label = result.test_case_label;
    trace_scroll.store(-1, std::memory_order_relaxed);
    static constexpr ListOp kOps[] = {ListOp::kPrepend, ListOp::kAppend,
                                      ListOp::kInsertAt, ListOp::kRemoveAt,
                                      ListOp::kGet};
    for (int i = 0; i < 5; ++i) {
      if (kOps[i] == current_config.op) {
        config_panel.op_selected = i;
        break;
      }
    }
    config_open = false;
    screen.Post(Event::Custom);
  };

  auto close_config = [&]() {
    config_open = false;
    screen.Post(Event::Custom);
  };

  config_panel.init(recording.algorithm_name, apply_config, close_config);

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
    step =
        std::clamp(step, 0, static_cast<int>(recording.steps.size()) - 1);
    const auto &snap = recording.steps[step];

    auto viz_pane = render_list(snap, recording.title, is_doubly);

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

    if (code_visible) {
      right_panels.push_back(
          code.render(snap.current_line, false) | flex_grow);
    } else {
      right_panels.push_back(code.render(snap.current_line, true));
    }

    if (state_visible) {
      right_panels.push_back(
          render_list_state_panel(snap, ctrl) | flex_shrink);
    }

    if (trace_visible) {
      right_panels.push_back(
          render_list_trace_panel(
              recording.steps, step,
              trace_scroll.load(std::memory_order_relaxed)) |
          flex);
    }

    auto right_pane = vbox(std::move(right_panels)) | flex;

    auto controls = hbox({
        text(" [Space]") | bold,
        text(" Run ") | dim,
        text("[N]") | bold,
        text(" Fwd ") | dim,
        text("[B]") | bold,
        text(" Back ") | dim,
        text("[R]") | bold,
        text(" Reset ") | dim,
        text("[+/-]") | bold,
        text(" Speed ") | dim,
        text("[C]") | bold,
        text(" Config ") | dim,
        text("[X]") | bold,
        text(" Clear ") | dim,
        text("[T]") | bold,
        text(" Trace ") | dim,
        text("[V]") | bold,
        text(" State ") | dim,
        text("[D]") | bold,
        text(" Code ") | dim,
        text("[Q]") | bold,
        text(" Quit ") | dim,
        separator(),
        text(" " +
             std::to_string(
                 ctrl.speed_ms.load(std::memory_order_relaxed)) +
             "ms ") |
            dim,
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
      ctrl.mode.store(VizController::Mode::PAUSED,
                      std::memory_order_relaxed);
      config_open = true;
      screen.Post(Event::Custom);
      return true;
    }
    if (event == Event::Character('x') || event == Event::Character('X')) {
      current_config.initial_values = {};
      recording = re_record(current_config);
      code = is_doubly ? get_doubly_code_panel(current_config.op)
                       : get_singly_code_panel(current_config.op);
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
  if (tick_thread.joinable()) {
    tick_thread.join();
  }
}

inline void run_singly_linked_list_viz(std::vector<int> initial) {
  ListConfig config;
  config.initial_values = initial;
  config.op = ListOp::kNone;
  config.value = 0;
  config.index = 0;

  auto recorder = [](const ListConfig &cfg) -> ListAlgorithmRecording {
    switch (cfg.op) {
    case ListOp::kPrepend:
      return record_singly_prepend(cfg.initial_values, cfg.value);
    case ListOp::kAppend:
      return record_singly_append(cfg.initial_values, cfg.value);
    case ListOp::kInsertAt:
      return record_singly_insert_at(cfg.initial_values,
                                     static_cast<size_t>(cfg.index),
                                     cfg.value);
    case ListOp::kRemoveAt:
      return record_singly_remove_at(cfg.initial_values,
                                     static_cast<size_t>(cfg.index));
    case ListOp::kGet:
      return record_singly_get(cfg.initial_values,
                               static_cast<size_t>(cfg.index));
    default:
      return record_idle(cfg.initial_values, "Singly Linked List", false);
    }
  };

  auto recording = recorder(config);
  auto code = get_singly_code_panel(config.op);
  run_list_visualizer(std::move(recording), code, config, recorder, false);
}

inline void run_doubly_linked_list_viz(std::vector<int> initial) {
  ListConfig config;
  config.initial_values = initial;
  config.op = ListOp::kNone;
  config.value = 0;
  config.index = 0;

  auto recorder = [](const ListConfig &cfg) -> ListAlgorithmRecording {
    switch (cfg.op) {
    case ListOp::kPrepend:
      return record_doubly_prepend(cfg.initial_values, cfg.value);
    case ListOp::kAppend:
      return record_doubly_append(cfg.initial_values, cfg.value);
    case ListOp::kInsertAt:
      return record_doubly_insert_at(cfg.initial_values,
                                     static_cast<size_t>(cfg.index),
                                     cfg.value);
    case ListOp::kRemoveAt:
      return record_doubly_remove_at(cfg.initial_values,
                                     static_cast<size_t>(cfg.index));
    case ListOp::kGet:
      return record_doubly_get(cfg.initial_values,
                               static_cast<size_t>(cfg.index));
    default:
      return record_idle(cfg.initial_values, "Doubly Linked List", true);
    }
  };

  auto recording = recorder(config);
  auto code = get_doubly_code_panel(config.op);
  run_list_visualizer(std::move(recording), code, config, recorder, true);
}

} // namespace viz
