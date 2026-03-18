#pragma once

#include <algorithm>
#include <atomic>
#include <string>

namespace viz {

struct VizController {
  enum class Mode { CONTINUOUS, STEP, PAUSED };

  std::atomic<Mode> mode{Mode::PAUSED};

  std::atomic<int> current_step{0};
  std::atomic<int> total_steps{0};

  std::atomic<int> speed_ms{300};
  std::atomic<int> elapsed_since_last_step_ms{0};

  std::atomic<bool> show_trace{true};
  std::atomic<bool> show_state{true};
  std::atomic<bool> show_code{true};

  std::string test_case_label;

  bool is_finished() const {
    int step = current_step.load(std::memory_order_relaxed);
    int total = total_steps.load(std::memory_order_relaxed);
    return total <= 0 || step >= total - 1;
  }

  bool step_forward() {
    int step = current_step.load(std::memory_order_relaxed);
    int total = total_steps.load(std::memory_order_relaxed);
    if (step < total - 1) {
      current_step.store(step + 1, std::memory_order_relaxed);
      return true;
    }
    return false;
  }

  bool step_backward() {
    int step = current_step.load(std::memory_order_relaxed);
    if (step > 0) {
      current_step.store(step - 1, std::memory_order_relaxed);
      Mode m = mode.load(std::memory_order_relaxed);
      if (m == Mode::CONTINUOUS) {
        mode.store(Mode::PAUSED, std::memory_order_relaxed);
      }
      return true;
    }
    return false;
  }

  void reset() {
    current_step.store(0, std::memory_order_relaxed);
    mode.store(Mode::PAUSED, std::memory_order_relaxed);
    elapsed_since_last_step_ms.store(0, std::memory_order_relaxed);
  }

  void toggle_pause() {
    Mode m = mode.load(std::memory_order_relaxed);
    if (m == Mode::CONTINUOUS) {
      mode.store(Mode::PAUSED, std::memory_order_relaxed);
    } else {
      if (!is_finished()) {
        mode.store(Mode::CONTINUOUS, std::memory_order_relaxed);
        elapsed_since_last_step_ms.store(0, std::memory_order_relaxed);
      }
    }
  }

  void speed_up() {
    int s = speed_ms.load(std::memory_order_relaxed);
    speed_ms.store(std::max(50, s - 50), std::memory_order_relaxed);
  }

  void speed_down() {
    int s = speed_ms.load(std::memory_order_relaxed);
    speed_ms.store(std::min(1000, s + 50), std::memory_order_relaxed);
  }

  void toggle_trace() {
    show_trace.store(!show_trace.load(std::memory_order_relaxed),
                     std::memory_order_relaxed);
  }
  void toggle_state() {
    show_state.store(!show_state.load(std::memory_order_relaxed),
                     std::memory_order_relaxed);
  }
  void toggle_code() {
    show_code.store(!show_code.load(std::memory_order_relaxed),
                    std::memory_order_relaxed);
  }

  bool tick(int delta_ms) {
    Mode m = mode.load(std::memory_order_relaxed);
    if (m != Mode::CONTINUOUS || is_finished())
      return false;

    int elapsed =
        elapsed_since_last_step_ms.load(std::memory_order_relaxed) + delta_ms;
    int speed = speed_ms.load(std::memory_order_relaxed);

    if (elapsed >= speed) {
      elapsed_since_last_step_ms.store(0, std::memory_order_relaxed);
      return step_forward();
    }
    elapsed_since_last_step_ms.store(elapsed, std::memory_order_relaxed);
    return false;
  }
};

} // namespace viz
