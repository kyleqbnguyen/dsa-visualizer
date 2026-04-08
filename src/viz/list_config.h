#pragma once

#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/dom/elements.hpp"
#include "list_snapshot.h"
#include "test_cases.h"

#include <algorithm>
#include <charconv>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

namespace viz {

struct ListConfig {
  std::vector<int> initial_values;
  ListOp op = ListOp::kAppend;
  int value = 0;
  int index = 0;
};

struct ListConfigResult {
  ListConfig config;
  std::string test_case_label;
  bool applied = false;
  bool reset_initial_values = false;
};

struct ListConfigPanel {
  std::string algo_name;

  std::vector<std::string> op_labels = {"Prepend", "Append", "Insert At",
                                        "Remove At", "Get"};
  int op_selected = 1;

  std::string value_str = "99";
  std::string index_str = "0";

  std::vector<ListTestCase> test_cases;
  std::vector<std::string> test_case_labels;
  int test_case_selected = 0;

  std::vector<std::string> tab_labels = {"Dataset", "Test Cases"};
  int tab_selected = 0;

  std::string validation_msg;

  std::function<void(const ListConfigResult&)> on_apply;
  std::function<void()> on_close;

  ftxui::Component component;

  void init(const std::string& algo,
            std::function<void(const ListConfigResult&)> apply_cb,
            std::function<void()> close_cb) {
    algo_name = algo;
    on_apply = std::move(apply_cb);
    on_close = std::move(close_cb);

    test_cases = get_list_test_cases(algo_name);
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

    auto index_opt = InputOption();
    index_opt.multiline = false;
    auto index_input = Input(&index_str, "index", index_opt);

    auto test_case_menu = Radiobox(&test_case_labels, &test_case_selected);
    auto tab_toggle = Toggle(&tab_labels, &tab_selected);

    auto dataset_container =
        Container::Vertical({op_radio, value_input, index_input});
    auto test_case_container = Container::Vertical({test_case_menu});
    auto tab_container =
        Container::Tab({dataset_container, test_case_container}, &tab_selected);
    auto inner = Container::Vertical({tab_toggle, tab_container});

    auto renderer = Renderer(inner, [=, this] {
      bool show_value =
          (op_selected == 0 || op_selected == 1 || op_selected == 2);
      bool show_index =
          (op_selected == 2 || op_selected == 3 || op_selected == 4);

      Elements content;
      content.push_back(text(" Configure: " + algo_name + " ") | bold | center);
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
        }
        if (show_index) {
          params.push_back(text("Index:"));
          params.push_back(index_input->Render() | border);
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
          auto& tc = test_cases[test_case_selected - 1];
          content.push_back(text(tc.description) | dim);
          std::string preview;
          for (int i = 0; i < static_cast<int>(tc.initial_values.size()); ++i) {
            if (i > 0)
              preview += ", ";
            preview += std::to_string(tc.initial_values[i]);
          }
          content.push_back(text("Data: [" + preview + "]") | dim);
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
    ListConfigResult result;

    if (tab_selected == 1) {
      if (test_case_selected > 0 &&
          test_case_selected <= static_cast<int>(test_cases.size())) {
        auto& tc = test_cases[test_case_selected - 1];
        result.config.initial_values = tc.initial_values;
        result.config.op = tc.op;
        result.config.value = tc.value;
        result.config.index = tc.index;
        result.test_case_label = tc.label;
        result.applied = true;
        result.reset_initial_values = true;
        validation_msg.clear();
        if (on_apply)
          on_apply(result);
      }
      return;
    }

    static constexpr ListOp ops[] = {ListOp::kPrepend, ListOp::kAppend,
                                     ListOp::kInsertAt, ListOp::kRemoveAt,
                                     ListOp::kGet};
    result.config.op = ops[op_selected];

    bool need_value =
        (op_selected == 0 || op_selected == 1 || op_selected == 2);
    bool need_index =
        (op_selected == 2 || op_selected == 3 || op_selected == 4);

    if (need_value) {
      auto parsed = generators::parse_csv_ints(value_str);
      if (parsed.empty()) {
        validation_msg = "Error: invalid value.";
        return;
      }
      result.config.value = parsed[0];
    }

    if (need_index) {
      auto parsed = generators::parse_csv_ints(index_str);
      if (parsed.empty()) {
        validation_msg = "Error: invalid index.";
        return;
      }
      result.config.index = parsed[0];
    }

    result.applied = true;
    validation_msg.clear();
    if (on_apply)
      on_apply(result);
  }
};

} // namespace viz
