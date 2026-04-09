#pragma once

#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/dom/elements.hpp"
#include "test_cases.h"

#include <functional>
#include <string>
#include <vector>

namespace viz {

struct MazeConfig {
  std::vector<std::string> grid;
  int start_row = 0;
  int start_col = 0;
  int end_row = 0;
  int end_col = 0;
};

struct MazeConfigResult {
  MazeConfig config;
  std::string test_case_label;
};

struct MazeConfigPanel {
  std::vector<MazeTestCase> test_cases;
  std::vector<std::string> test_case_labels;
  int test_case_selected = 0;

  std::function<void(const MazeConfigResult&)> on_apply;
  std::function<void()> on_close;

  ftxui::Component component;

  void init(std::function<void(const MazeConfigResult&)> apply_cb,
            std::function<void()> close_cb) {
    on_apply = std::move(apply_cb);
    on_close = std::move(close_cb);

    test_cases = maze_test_cases();
    test_case_labels.clear();
    for (const auto& tc : test_cases)
      test_case_labels.push_back(tc.label);
    test_case_selected = 0;

    build_component();
  }

  void build_component() {
    using namespace ftxui;

    auto menu = Radiobox(&test_case_labels, &test_case_selected);
    auto inner = Container::Vertical({menu});

    auto renderer = Renderer(inner, [=, this] {
      Elements content;
      content.push_back(text(" Configure: Maze Solver ") | bold | center);
      content.push_back(separator());
      content.push_back(text("Select a maze:") | bold);
      content.push_back(menu->Render());

      if (test_case_selected >= 0 &&
          test_case_selected < static_cast<int>(test_cases.size())) {
        content.push_back(separator());
        const auto& tc = test_cases[test_case_selected];
        content.push_back(text(tc.description) | dim);
        content.push_back(text("Size: " +
                               std::to_string(tc.grid.size()) + "x" +
                               std::to_string(tc.grid.empty()
                                                  ? 0
                                                  : tc.grid[0].size())) |
                          dim);
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
        if (test_case_selected >= 0 &&
            test_case_selected < static_cast<int>(test_cases.size())) {
          const auto& tc = test_cases[test_case_selected];
          MazeConfigResult result;
          result.config.grid = tc.grid;
          result.config.start_row = tc.start_row;
          result.config.start_col = tc.start_col;
          result.config.end_row = tc.end_row;
          result.config.end_col = tc.end_col;
          result.test_case_label = tc.label;
          if (on_apply)
            on_apply(result);
        }
        return true;
      }
      return false;
    });
  }
};

} // namespace viz
