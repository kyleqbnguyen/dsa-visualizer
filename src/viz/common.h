#pragma once

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/color.hpp"
#include "snapshot.h"

#include <algorithm>
#include <string>
#include <vector>

namespace viz {

namespace colors {
inline constexpr auto kDefault = ftxui::Color::GrayLight;
inline constexpr auto kActive = ftxui::Color::Yellow;
inline constexpr auto kCompare = ftxui::Color::Cyan;
inline constexpr auto kSwap = ftxui::Color::Red;
inline constexpr auto kFound = ftxui::Color::Green;
inline constexpr auto kSorted = ftxui::Color::Blue;
inline constexpr auto kPivot = ftxui::Color::Magenta;
inline constexpr auto kOutOfRange = ftxui::Color::GrayDark;
} // namespace colors

inline auto make_bar(int value, int max_value, int max_height,
                     ftxui::Color color) -> ftxui::Element {
  int height = (max_value > 0) ? (value * max_height / max_value) : 0;
  if (height < 1)
    height = 1;

  std::vector<ftxui::Element> rows;
  for (int row = max_height; row > 0; --row) {
    if (row <= height)
      rows.push_back(ftxui::text("██") | ftxui::color(color));
    else
      rows.push_back(ftxui::text("  "));
  }
  rows.push_back(ftxui::text(std::to_string(value)) | ftxui::center);
  return ftxui::vbox(std::move(rows)) | ftxui::center;
}

inline auto render_array(const StepSnapshot& snap,
                         const std::string& title = {}, int target = -1,
                         bool is_search = false) -> ftxui::Element {

  std::vector<ftxui::Element> content;

  if (!title.empty()) {
    content.push_back(ftxui::text(title) | ftxui::bold | ftxui::center);
    content.push_back(ftxui::separator());
  }

  if (snap.data.empty()) {
    content.push_back(ftxui::text("No data") | ftxui::center);
    return ftxui::vbox(std::move(content)) | ftxui::border | ftxui::flex;
  }

  int max_val = *std::max_element(snap.data.begin(), snap.data.end());
  constexpr int kMaxBarHeight = 15;

  std::vector<ftxui::Element> bars;
  for (int i = 0; i < static_cast<int>(snap.data.size()); ++i) {
    ftxui::Color bar_color = colors::kDefault;

    if (i == snap.found_index) {
      bar_color = colors::kFound;
    } else if (i == snap.highlight_a || i == snap.highlight_b) {
      bar_color = colors::kActive;
    } else if (snap.low >= 0 && snap.high >= 0) {
      if (i < snap.low || i >= snap.high)
        bar_color = colors::kOutOfRange;
    } else if (snap.sorted_boundary >= 0) {
      if (i >= snap.sorted_boundary)
        bar_color = colors::kSorted;
    }

    bars.push_back(make_bar(snap.data[i], max_val, kMaxBarHeight, bar_color));
    bars.push_back(ftxui::text(" "));
  }

  content.push_back(ftxui::hbox(std::move(bars)) | ftxui::center | ftxui::flex);

  std::vector<ftxui::Element> info_parts;

  if (is_search) {
    info_parts.push_back(ftxui::text("Target: " + std::to_string(target)) |
                         ftxui::bold | ftxui::color(ftxui::Color::Cyan));
    info_parts.push_back(ftxui::text("  "));
  }

  if (snap.low >= 0 && snap.high >= 0) {
    info_parts.push_back(ftxui::text("Window: [" + std::to_string(snap.low) +
                                     ", " + std::to_string(snap.high) + ")") |
                         ftxui::dim);
    info_parts.push_back(ftxui::text("  "));
  }

  if (snap.sorted_boundary >= 0 &&
      snap.sorted_boundary < static_cast<int>(snap.data.size())) {
    info_parts.push_back(
        ftxui::text("Sorted: [" + std::to_string(snap.sorted_boundary) + ".." +
                    std::to_string(snap.data.size() - 1) + "]") |
        ftxui::color(colors::kSorted));
    info_parts.push_back(ftxui::text("  "));
  }

  if (!info_parts.empty()) {
    content.push_back(ftxui::separator());
    content.push_back(ftxui::hbox(std::move(info_parts)) | ftxui::center);
  }

  if (!snap.status_text.empty()) {
    content.push_back(ftxui::separator());
    content.push_back(ftxui::text(snap.status_text) | ftxui::center);
  }

  return ftxui::vbox(std::move(content)) | ftxui::border | ftxui::flex;
}

} // namespace viz
