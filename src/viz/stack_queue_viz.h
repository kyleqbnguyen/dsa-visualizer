#pragma once

#include "code_panel.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "list_snapshot.h"
#include "list_viz.h"
#include "stack_queue_code_panel.h"
#include "stack_queue_config.h"
#include "stack_queue_recorder.h"
#include "viz_controller.h"

#include <atomic>
#include <chrono>
#include <functional>
#include <string>
#include <thread>
#include <vector>

namespace viz {

using StackQueueRecorderFunc =
    std::function<ListAlgorithmRecording(const StackQueueConfig&)>;

inline auto render_stack(const ListStepSnapshot& snap, const std::string& title)
    -> ftxui::Element {
  using namespace ftxui;

  std::vector<Element> content;
  if (!title.empty()) {
    content.push_back(text(title) | bold | center);
    content.push_back(separator());
  }

  if (snap.nodes.empty()) {
    content.push_back(text("(empty stack)") | center | dim);
    if (!snap.status_text.empty()) {
      content.push_back(separator());
      content.push_back(text(snap.status_text) | center);
    }
    return vbox(std::move(content)) | border | flex;
  }

  std::vector<Element> label_row;
  label_row.push_back(text("BOTTOM") | bold | dim);
  for (std::size_t i = 0; i + 1 < snap.nodes.size(); ++i) {
    label_row.push_back(text("        "));
  }
  label_row.push_back(text("  TOP") | bold | color(Color::Cyan));

  std::vector<Element> node_row;
  std::vector<Element> cursor_row;

  for (int i = 0; i < static_cast<int>(snap.nodes.size()); ++i) {
    const auto& node = snap.nodes[i];
    auto col = node_color(node.state);

    auto box = vbox({
                   text(" " + std::to_string(node.value) + " ") | bold |
                       color(col) | center,
               }) |
               border | color(col);
    node_row.push_back(box);

    if (i < static_cast<int>(snap.nodes.size()) - 1) {
      node_row.push_back(text(" --> ") | dim);
    }

    std::string label;
    if (node.state == ListNodeState::kActive)
      label = "top";
    else if (node.state == ListNodeState::kNew)
      label = "new";
    else if (node.state == ListNodeState::kFound)
      label = "top";
    else if (node.state == ListNodeState::kRemoved)
      label = "del";

    if (!label.empty())
      cursor_row.push_back(text("  " + label + "  ") | bold | color(col));
    else
      cursor_row.push_back(text("       "));

    if (i < static_cast<int>(snap.nodes.size()) - 1)
      cursor_row.push_back(text("     "));
  }

  content.push_back(hbox(std::move(label_row)) | center);
  content.push_back(hbox(std::move(node_row)) | center);
  content.push_back(hbox(std::move(cursor_row)) | center);

  if (!snap.status_text.empty()) {
    content.push_back(separator());
    content.push_back(text(snap.status_text) | center);
  }

  return vbox(std::move(content)) | border | flex;
}
inline auto render_queue(const ListStepSnapshot& snap, const std::string& title)
    -> ftxui::Element {
  using namespace ftxui;

  std::vector<Element> content;
  if (!title.empty()) {
    content.push_back(text(title) | bold | center);
    content.push_back(separator());
  }

  if (snap.nodes.empty()) {
    content.push_back(text("(empty queue)") | center | dim);
    if (!snap.status_text.empty()) {
      content.push_back(separator());
      content.push_back(text(snap.status_text) | center);
    }
    return vbox(std::move(content)) | border | flex;
  }

  std::vector<Element> label_row;
  label_row.push_back(text("FRONT") | bold | color(Color::Green));
  for (std::size_t i = 0; i + 1 < snap.nodes.size(); ++i) {
    label_row.push_back(text("        "));
  }
  label_row.push_back(text("  REAR") | bold | color(Color::Cyan));

  std::vector<Element> node_row;
  std::vector<Element> cursor_row;

  for (int i = 0; i < static_cast<int>(snap.nodes.size()); ++i) {
    const auto& node = snap.nodes[i];
    auto col = node_color(node.state);

    auto box = vbox({
                   text(" " + std::to_string(node.value) + " ") | bold |
                       color(col) | center,
               }) |
               border | color(col);
    node_row.push_back(box);

    if (i < static_cast<int>(snap.nodes.size()) - 1) {
      node_row.push_back(text(" --> ") | dim);
    }

    std::string label;
    if (node.state == ListNodeState::kActive)
      label = "front";
    else if (node.state == ListNodeState::kNew)
      label = "new";
    else if (node.state == ListNodeState::kFound)
      label = "rear";
    else if (node.state == ListNodeState::kRemoved)
      label = "del";

    if (!label.empty())
      cursor_row.push_back(text("  " + label + "  ") | bold | color(col));
    else
      cursor_row.push_back(text("       "));

    if (i < static_cast<int>(snap.nodes.size()) - 1)
      cursor_row.push_back(text("     "));
  }

  content.push_back(hbox(std::move(label_row)) | center);
  content.push_back(hbox(std::move(node_row)) | center);
  content.push_back(hbox(std::move(cursor_row)) | center);

  if (!snap.status_text.empty()) {
    content.push_back(separator());
    content.push_back(text(snap.status_text) | center);
  }

  return vbox(std::move(content)) | border | flex;
}

inline void run_stack_queue_visualizer(ListAlgorithmRecording recording,
                                       CodePanel code,
                                       StackQueueConfig current_config,
                                       StackQueueRecorderFunc re_record,
                                       bool is_stack) {
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

  StackQueueConfigPanel config_panel;

  auto apply_config = [&](const StackQueueConfigResult& result) {
    auto preserved = extract_final_values(recording);
    current_config = result.config;
    if (result.reset_initial_values) {
      current_config.initial_values = result.config.initial_values;
    } else {
      current_config.initial_values = preserved;
    }
    recording = re_record(current_config);
    code = is_stack ? get_stack_code_panel(current_config.op)
                    : get_queue_code_panel(current_config.op);
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

  config_panel.init(is_stack, apply_config, close_config);

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

    auto viz_pane = is_stack ? render_stack(snap, recording.title)
                             : render_queue(snap, recording.title);

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
      right_panels.push_back(render_list_state_panel(snap, ctrl) | flex_shrink);

    if (trace_visible) {
      right_panels.push_back(render_list_trace_panel(
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
      code = is_stack ? get_stack_code_panel(current_config.op)
                      : get_queue_code_panel(current_config.op);
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

inline void run_stack_viz() {
  StackQueueConfig config;
  config.initial_values = {5, 3, 8, 1, 6};
  config.op = ListOp::kNone;
  config.value = 0;

  auto re_record = [](const StackQueueConfig& cfg) -> ListAlgorithmRecording {
    switch (cfg.op) {
    case ListOp::kPush:
      return record_stack_push(cfg.initial_values, cfg.value);
    case ListOp::kPop:
      return record_stack_pop(cfg.initial_values);
    default:
      return record_stack_queue_idle(cfg.initial_values, "Stack");
    }
  };

  auto recording = re_record(config);
  auto code = get_stack_code_panel(config.op);
  run_stack_queue_visualizer(std::move(recording), code, config, re_record,
                             /*is_stack=*/true);
}

inline void run_queue_viz() {
  StackQueueConfig config;
  config.initial_values = {5, 3, 8, 1, 6};
  config.op = ListOp::kNone;
  config.value = 0;

  auto re_record = [](const StackQueueConfig& cfg) -> ListAlgorithmRecording {
    switch (cfg.op) {
    case ListOp::kEnqueue:
      return record_queue_enqueue(cfg.initial_values, cfg.value);
    case ListOp::kDequeue:
      return record_queue_dequeue(cfg.initial_values);
    default:
      return record_stack_queue_idle(cfg.initial_values, "Queue");
    }
  };

  auto recording = re_record(config);
  auto code = get_queue_code_panel(config.op);
  run_stack_queue_visualizer(std::move(recording), code, config, re_record,
                             /*is_stack=*/false);
}

} // namespace viz
