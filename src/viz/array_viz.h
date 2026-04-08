#pragma once

#include "algo_recorder.h"
#include "code_panel.h"
#include "common.h"
#include "config_overlay.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "snapshot.h"
#include "state_panel.h"
#include "trace_panel.h"
#include "viz_controller.h"

#include <atomic>
#include <chrono>
#include <functional>
#include <string>
#include <thread>
#include <vector>

namespace viz {

using RecorderFunc =
    std::function<AlgorithmRecording(const std::vector<int>& data, int target)>;

inline void run_visualizer(AlgorithmRecording recording, const CodePanel& code,
                           bool is_search, int target, RecorderFunc re_record) {
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

  ConfigPanel config_panel;

  auto apply_config = [&](const ConfigResult& result) {
    target = result.target;
    recording = re_record(result.data, result.target);
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

  config_panel.init(recording.algorithm_name, is_search, target, apply_config,
                    close_config);

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

    auto viz_pane = render_array(snap, recording.title, target, is_search);

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
      right_panels.push_back(code.render(snap.current_line, false) | flex_grow);
    } else {
      right_panels.push_back(code.render(snap.current_line, true));
    }

    if (state_visible) {
      right_panels.push_back(render_state_panel(snap, ctrl) | flex_shrink);
    }

    if (trace_visible) {
      right_panels.push_back(
          render_trace_panel(recording.steps, step,
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
             std::to_string(ctrl.speed_ms.load(std::memory_order_relaxed)) +
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
      ctrl.mode.store(VizController::Mode::PAUSED, std::memory_order_relaxed);
      config_open = true;
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

inline void run_linear_search_viz(std::vector<int> input, int target) {
  auto code = linear_search_code();

  auto recorder = [](const std::vector<int>& data,
                     int tgt) -> AlgorithmRecording {
    return record_linear_search(data, tgt);
  };

  auto recording = recorder(input, target);
  run_visualizer(std::move(recording), code, true, target, recorder);
}

inline void run_binary_search_viz(std::vector<int> input, int target) {
  auto code = binary_search_code();

  auto recorder = [](const std::vector<int>& data,
                     int tgt) -> AlgorithmRecording {
    return record_binary_search(data, tgt);
  };

  auto recording = recorder(input, target);
  run_visualizer(std::move(recording), code, true, target, recorder);
}

inline void run_bubble_sort_viz(std::vector<int> input) {
  auto code = bubble_sort_code();

  auto recorder = [](const std::vector<int>& data,
                     int /*tgt*/) -> AlgorithmRecording {
    return record_bubble_sort(data);
  };

  auto recording = recorder(input, 0);
  run_visualizer(std::move(recording), code, false, 0, recorder);
}

inline void run_two_crystal_balls_viz(std::vector<int> input) {
  auto code = two_crystal_balls_code();

  auto recorder = [](const std::vector<int>& data,
                     int /*tgt*/) -> AlgorithmRecording {
    return record_two_crystal_balls(data);
  };

  auto recording = recorder(input, 0);
  run_visualizer(std::move(recording), code, false, 0, recorder);
}

} // namespace viz
