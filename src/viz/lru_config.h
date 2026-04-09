#pragma once

#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/dom/elements.hpp"
#include "lru_snapshot.h"
#include "test_cases.h"

#include <charconv>
#include <functional>
#include <string>
#include <vector>

namespace viz {

struct LruConfig {
  std::vector<std::pair<int, int>> initial_entries = {{1, 10}, {2, 20}, {3, 30}};
  int capacity = 4;
  LruOp op = LruOp::kNone;
  int key = 0;
  int value = 0;
};

struct LruConfigResult {
  LruConfig config;
  std::string test_case_label;
  bool reset_initial_values = false;
};

struct LruConfigPanel {
  std::vector<std::string> op_labels = {"Get", "Put"};
  int op_selected = 0;

  std::string key_str = "2";
  std::string value_str = "99";

  std::vector<LruTestCase> test_cases;
  std::vector<std::string> test_case_labels;
  int test_case_selected = 0;

  std::vector<std::string> tab_labels = {"Dataset", "Test Cases"};
  int tab_selected = 0;

  std::string validation_msg;

  std::function<void(const LruConfigResult&)> on_apply;
  std::function<void()> on_close;

  ftxui::Component component;

  void init(std::function<void(const LruConfigResult&)> apply_cb,
            std::function<void()> close_cb) {
    on_apply = std::move(apply_cb);
    on_close = std::move(close_cb);

    test_cases = lru_test_cases();
    test_case_labels.clear();
    test_case_labels.push_back("(None)");
    for (const auto& tc : test_cases)
      test_case_labels.push_back(tc.label);
    test_case_selected = 0;
    validation_msg.clear();

    build_component();
  }

  void build_component() {
    using namespace ftxui;

    auto op_radio = Radiobox(&op_labels, &op_selected);

    auto key_opt = InputOption();
    key_opt.multiline = false;
    auto key_input = Input(&key_str, "key", key_opt);

    auto val_opt = InputOption();
    val_opt.multiline = false;
    auto val_input = Input(&value_str, "value", val_opt);

    auto test_case_menu = Radiobox(&test_case_labels, &test_case_selected);
    auto tab_toggle = Toggle(&tab_labels, &tab_selected);

    auto dataset_container =
        Container::Vertical({op_radio, key_input, val_input});
    auto test_case_container = Container::Vertical({test_case_menu});
    auto tab_container =
        Container::Tab({dataset_container, test_case_container}, &tab_selected);
    auto inner = Container::Vertical({tab_toggle, tab_container});

    auto renderer = Renderer(inner, [=, this] {
      bool show_value = (op_selected == 1);

      Elements content;
      content.push_back(text(" Configure: LRU Cache ") | bold | center);
      content.push_back(separator());
      content.push_back(tab_toggle->Render() | center);
      content.push_back(separator());

      if (tab_selected == 0) {
        Elements cols;
        cols.push_back(vbox({
                           text("Operation") | bold | underlined,
                           op_radio->Render(),
                       }) |
                       flex);

        Elements params;
        params.push_back(text("Parameters") | bold | underlined);
        params.push_back(text("Key:"));
        params.push_back(key_input->Render() | border);
        if (show_value) {
          params.push_back(text("Value:"));
          params.push_back(val_input->Render() | border);
        }
        cols.push_back(separator());
        cols.push_back(vbox(std::move(params)) | flex);
        content.push_back(hbox(std::move(cols)));
      } else {
        content.push_back(text("Predefined test cases:") | bold);
        content.push_back(test_case_menu->Render());
        if (test_case_selected > 0 &&
            test_case_selected <= static_cast<int>(test_cases.size())) {
          content.push_back(separator());
          const auto& tc = test_cases[test_case_selected - 1];
          content.push_back(text(tc.description) | dim);
        }
      }

      if (!validation_msg.empty()) {
        content.push_back(separator());
        content.push_back(text(validation_msg) | color(Color::Red));
      }

      content.push_back(separator());
      content.push_back(hbox({
          text(" [Enter] Apply ") | bold,
          text("  "),
          text(" [Esc/C] Close ") | dim,
      }));

      return vbox(std::move(content)) | border;
    });

    component = CatchEvent(renderer, [this](Event event) -> bool {
      if (event == Event::Escape || event == Event::Character('c') ||
          event == Event::Character('C')) {
        if (on_close)
          on_close();
        return true;
      }
      if (event == Event::Return) {
        try_apply();
        return true;
      }
      return false;
    });
  }

  void try_apply() {
    LruConfigResult result;

    if (tab_selected == 1) {
      if (test_case_selected > 0 &&
          test_case_selected <= static_cast<int>(test_cases.size())) {
        const auto& tc = test_cases[test_case_selected - 1];
        result.config.initial_entries = tc.initial_entries;
        result.config.capacity = tc.capacity;
        result.config.op = tc.op;
        result.config.key = tc.key;
        result.config.value = tc.value;
        result.test_case_label = tc.label;
        result.reset_initial_values = true;
        validation_msg.clear();
        if (on_apply)
          on_apply(result);
      }
      return;
    }

    result.config.op =
        (op_selected == 0) ? LruOp::kGet : LruOp::kPut;

    int parsed_key = 0;
    auto [kp, ke] = std::from_chars(
        key_str.data(), key_str.data() + key_str.size(), parsed_key);
    if (ke != std::errc{}) {
      validation_msg = "Error: invalid key.";
      return;
    }
    result.config.key = parsed_key;

    if (op_selected == 1) {
      int parsed_val = 0;
      auto [vp, ve] = std::from_chars(
          value_str.data(), value_str.data() + value_str.size(), parsed_val);
      if (ve != std::errc{}) {
        validation_msg = "Error: invalid value.";
        return;
      }
      result.config.value = parsed_val;
    }

    result.reset_initial_values = false;
    validation_msg.clear();
    if (on_apply)
      on_apply(result);
  }

  void sync_op(LruOp op) {
    op_selected = (op == LruOp::kGet) ? 0 : 1;
  }
};

} // namespace viz
