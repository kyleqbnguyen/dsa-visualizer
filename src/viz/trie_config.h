#pragma once

#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/dom/elements.hpp"
#include "test_cases.h"
#include "trie_snapshot.h"

#include <functional>
#include <string>
#include <vector>

namespace viz {

struct TrieConfig {
  std::vector<std::string> dictionary = {"app", "apple", "ape", "bat", "ball"};
  TrieVizOp op = TrieVizOp::kNone;
  std::string target_word;
};

struct TrieConfigResult {
  TrieConfig config;
  std::string test_case_label;
  bool reset_initial_values = false;
};

struct TrieConfigPanel {
  std::vector<std::string> op_labels = {"Insert", "Search", "Delete"};
  int op_selected = 0;

  std::string word_str = "bad";

  std::vector<TrieTestCase> test_cases;
  std::vector<std::string> test_case_labels;
  int test_case_selected = 0;

  std::vector<std::string> tab_labels = {"Dataset", "Test Cases"};
  int tab_selected = 0;

  std::string validation_msg;

  std::function<void(const TrieConfigResult&)> on_apply;
  std::function<void()> on_close;

  ftxui::Component component;

  void init(std::function<void(const TrieConfigResult&)> apply_cb,
            std::function<void()> close_cb) {
    on_apply = std::move(apply_cb);
    on_close = std::move(close_cb);

    test_cases = trie_test_cases();
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

    auto word_opt = InputOption();
    word_opt.multiline = false;
    auto word_input = Input(&word_str, "word", word_opt);

    auto test_case_menu = Radiobox(&test_case_labels, &test_case_selected);
    auto tab_toggle = Toggle(&tab_labels, &tab_selected);

    auto dataset_container = Container::Vertical({op_radio, word_input});
    auto test_case_container = Container::Vertical({test_case_menu});
    auto tab_container =
        Container::Tab({dataset_container, test_case_container}, &tab_selected);
    auto inner = Container::Vertical({tab_toggle, tab_container});

    auto renderer = Renderer(inner, [=, this] {
      Elements content;
      content.push_back(text(" Configure: Trie ") | bold | center);
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
        params.push_back(text("Word:"));
        params.push_back(word_input->Render() | border);
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
    TrieConfigResult result;

    if (tab_selected == 1) {
      if (test_case_selected > 0 &&
          test_case_selected <= static_cast<int>(test_cases.size())) {
        const auto& tc = test_cases[test_case_selected - 1];
        result.config.dictionary = tc.dictionary;
        result.config.op = tc.op;
        result.config.target_word = tc.target_word;
        result.test_case_label = tc.label;
        result.reset_initial_values = true;
        validation_msg.clear();
        if (on_apply)
          on_apply(result);
      }
      return;
    }

    static constexpr TrieVizOp ops[] = {TrieVizOp::kInsert, TrieVizOp::kSearch,
                                        TrieVizOp::kDelete};
    result.config.op = ops[op_selected];
    result.config.target_word = word_str;

    if (word_str.empty()) {
      validation_msg = "Error: word cannot be empty.";
      return;
    }

    result.reset_initial_values = false;
    validation_msg.clear();
    if (on_apply)
      on_apply(result);
  }

  void sync_op(TrieVizOp op) {
    switch (op) {
    case TrieVizOp::kInsert:
      op_selected = 0;
      break;
    case TrieVizOp::kSearch:
      op_selected = 1;
      break;
    case TrieVizOp::kDelete:
      op_selected = 2;
      break;
    default:
      op_selected = 0;
      break;
    }
  }
};

} // namespace viz
