#pragma once

#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/dom/elements.hpp"
#include "test_cases.h"
#include "tree_snapshot.h"

#include <charconv>
#include <functional>
#include <string>
#include <vector>

namespace viz {

struct TreeConfig {
  std::vector<int> tree_data = {8, 4, 12, 2, 6, 10, 14};
  TreeOp op = TreeOp::kNone;
  int value = 0;
};

struct TreeConfigResult {
  TreeConfig config;
  std::string test_case_label;
  bool reset_initial_values = false;
};

struct TreeConfigPanel {
  std::vector<std::string> op_labels = {
      "Pre-order", "In-order", "Post-order", "BFS",
      "BST Find",  "BST Insert", "BST Delete",
  };
  int op_selected = 0;

  std::string value_str = "5";

  std::vector<TreeTestCase> test_cases;
  std::vector<std::string> test_case_labels;
  int test_case_selected = 0;

  std::vector<std::string> tab_labels = {"Dataset", "Test Cases"};
  int tab_selected = 0;

  std::string validation_msg;

  std::function<void(const TreeConfigResult&)> on_apply;
  std::function<void()> on_close;

  ftxui::Component component;

  void init(std::function<void(const TreeConfigResult&)> apply_cb,
            std::function<void()> close_cb) {
    on_apply = std::move(apply_cb);
    on_close = std::move(close_cb);

    test_cases = tree_test_cases();
    test_case_labels.clear();
    test_case_labels.push_back("(None)");
    for (const auto& tc : test_cases) {
      test_case_labels.push_back(tc.label);
    }
    test_case_selected = 0;
    validation_msg.clear();

    build_component();
  }

  void build_component() {
    using namespace ftxui;

    auto op_radio = Radiobox(&op_labels, &op_selected);

    auto value_opt = InputOption();
    value_opt.multiline = false;
    auto value_input = Input(&value_str, "value", value_opt);

    auto test_case_menu = Radiobox(&test_case_labels, &test_case_selected);
    auto tab_toggle = Toggle(&tab_labels, &tab_selected);

    auto dataset_container = Container::Vertical({op_radio, value_input});
    auto test_case_container = Container::Vertical({test_case_menu});
    auto tab_container =
        Container::Tab({dataset_container, test_case_container}, &tab_selected);
    auto inner = Container::Vertical({tab_toggle, tab_container});

    auto renderer = Renderer(inner, [=, this] {
      bool show_value = (op_selected >= 4);

      Elements content;
      content.push_back(text(" Configure: Trees ") | bold | center);
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
        if (show_value) {
          params.push_back(text("Value:"));
          params.push_back(value_input->Render() | border);
        } else {
          params.push_back(text("(no value parameter)") | dim);
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
    TreeConfigResult result;

    if (tab_selected == 1) {
      if (test_case_selected > 0 &&
          test_case_selected <= static_cast<int>(test_cases.size())) {
        const auto& tc = test_cases[test_case_selected - 1];
        result.config.tree_data = tc.tree_data;
        result.config.op = tc.op;
        result.config.value = tc.value;
        result.test_case_label = tc.label;
        result.reset_initial_values = true;
        validation_msg.clear();
        if (on_apply)
          on_apply(result);
      }
      return;
    }

    static constexpr TreeOp ops[] = {
        TreeOp::kPreorder,  TreeOp::kInorder,    TreeOp::kPostorder,
        TreeOp::kBfs,       TreeOp::kBstFind,    TreeOp::kBstInsert,
        TreeOp::kBstDelete,
    };
    result.config.op = ops[op_selected];

    if (op_selected >= 4) {
      int parsed_val = 0;
      auto [ptr, ec] = std::from_chars(
          value_str.data(), value_str.data() + value_str.size(), parsed_val);
      if (ec != std::errc{}) {
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

  void sync_op(TreeOp op) {
    static constexpr TreeOp ops[] = {
        TreeOp::kPreorder,  TreeOp::kInorder,    TreeOp::kPostorder,
        TreeOp::kBfs,       TreeOp::kBstFind,    TreeOp::kBstInsert,
        TreeOp::kBstDelete,
    };
    for (int i = 0; i < 7; ++i) {
      if (ops[i] == op) {
        op_selected = i;
        return;
      }
    }
    op_selected = 0;
  }
};

} // namespace viz
