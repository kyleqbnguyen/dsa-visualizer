#pragma once

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/color.hpp"
#include "snapshot.h"

#include <algorithm>
#include <string>
#include <vector>

namespace viz {

inline auto render_trace_panel(const std::vector<StepSnapshot>& steps,
                               int current_step, int scroll_offset,
                               int visible_lines = 8) -> ftxui::Element {
  using namespace ftxui;

  std::vector<Element> rows;

  int total_entries = 0;
  for (int i = 0; i <= current_step && i < static_cast<int>(steps.size());
       ++i) {
    if (!steps[i].trace_entry.empty()) {
      total_entries++;
    }
  }

  int effective_offset = scroll_offset;
  if (effective_offset < 0) {
    effective_offset = std::max(0, total_entries - visible_lines);
  }

  int entry_idx = 0;
  for (int i = 0; i <= current_step && i < static_cast<int>(steps.size());
       ++i) {
    if (steps[i].trace_entry.empty())
      continue;

    if (entry_idx >= effective_offset &&
        entry_idx < effective_offset + visible_lines) {
      bool is_current = (i == current_step);
      auto prefix = text(std::to_string(i) + ": ") | dim;
      auto entry_text = text(steps[i].trace_entry);

      if (is_current) {
        rows.push_back(
            hbox({prefix, entry_text | bold | color(Color::Yellow)}));
      } else {
        rows.push_back(hbox({prefix, entry_text | color(Color::GrayLight)}));
      }
    }
    entry_idx++;
  }

  while (static_cast<int>(rows.size()) < visible_lines) {
    rows.push_back(text("") | dim);
  }

  std::string scroll_info = std::to_string(total_entries) + " entries";
  if (total_entries > visible_lines) {
    int page = effective_offset / visible_lines + 1;
    int total_pages = (total_entries + visible_lines - 1) / visible_lines;
    scroll_info += "  (page " + std::to_string(page) + "/" +
                   std::to_string(total_pages) + ")";
  }

  return vbox({
             text(" Trace ") | bold | center,
             separator(),
             vbox(std::move(rows)) | vscroll_indicator | frame,
             separator(),
             text(scroll_info) | dim | center,
         }) |
         border;
}

} // namespace viz
