#pragma once

#include <string>
#include <vector>

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/color.hpp"

namespace viz {

struct CodePanel {
  std::string title;
  std::vector<std::string> lines;

  auto render(int current_line, bool collapsed = false) const
      -> ftxui::Element {
    if (collapsed) {
      return ftxui::vbox({
                 ftxui::text(" " + title + " [hidden] ") | ftxui::dim |
                     ftxui::center,
             }) |
             ftxui::border;
    }

    std::vector<ftxui::Element> rows;

    for (int idx = 0; idx < static_cast<int>(lines.size()); ++idx) {
      auto line_num =
          ftxui::text(std::to_string(idx + 1) + " ") | ftxui::dim;
      auto line_text = ftxui::text(lines[idx]);

      ftxui::Element row;
      if (idx == current_line) {
        row = ftxui::hbox({
            line_num | ftxui::bold | ftxui::color(ftxui::Color::Yellow),
            line_text | ftxui::bold |
                ftxui::bgcolor(ftxui::Color::GrayDark) |
                ftxui::color(ftxui::Color::Yellow),
        });
      } else {
        row = ftxui::hbox({line_num, line_text});
      }
      rows.push_back(row);
    }

    return ftxui::vbox({
               ftxui::text(" " + title + " ") | ftxui::bold | ftxui::center,
               ftxui::separator(),
               ftxui::vbox(std::move(rows)),
           }) |
           ftxui::border;
  }
};

inline auto linear_search_code() -> CodePanel {
  return {
      .title = "linear_search.h",
      .lines =
          {
              "for (size_t i = 0; i < data.size(); ++i) {",
              "    bool found = (data[i] == target);",
              "    if (found) {",
              "        return i;",
              "    }",
              "}",
              "return std::nullopt;",
          },
  };
}

inline auto linear_search_line(bool found, bool finished) -> int {
  if (finished)
    return 6;
  if (found)
    return 3;
  return 1;
}

inline auto binary_search_code() -> CodePanel {
  return {
      .title = "binary_search.h",
      .lines =
          {
              "if (data.empty()) return std::nullopt;",
              "size_t low = 0;",
              "size_t high = data.size();",
              "",
              "while (low < high) {",
              "    size_t mid = low + (high - low) / 2;",
              "    bool found = (data[mid] == target);",
              "",
              "    if (found) {",
              "        return mid;",
              "    } else if (data[mid] < target) {",
              "        low = mid + 1;",
              "    } else {",
              "        high = mid;",
              "    }",
              "}",
              "return std::nullopt;",
          },
  };
}

inline auto binary_search_line(bool found, bool val_less_than_target,
                               bool finished) -> int {
  if (finished)
    return 16;
  if (found)
    return 9;
  if (val_less_than_target)
    return 11;
  return 13;
}

inline auto bubble_sort_code() -> CodePanel {
  return {
      .title = "bubble_sort.h",
      .lines =
          {
              "for (size_t i = 0; i < data.size(); ++i) {",
              "    for (size_t j = 0; j < data.size()-1-i; ++j) {",
              "        bool will_swap = data[j] > data[j+1];",
              "        if (will_swap) {",
              "            std::swap(data[j], data[j+1]);",
              "        }",
              "    }",
              "    if (!swapped) break;",
              "}",
          },
  };
}

inline auto bubble_sort_line(bool will_swap, bool finished) -> int {
  if (finished)
    return 8;
  if (will_swap)
    return 4;
  return 2;
}

} // namespace viz
