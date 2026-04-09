#pragma once

#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/dom/elements.hpp"
#include "graph_snapshot.h"
#include "test_cases.h"

#include <functional>
#include <string>
#include <vector>

namespace viz {

struct GraphConfig {
  int graph_preset = 0;
  GraphOp op = GraphOp::kNone;
  int source = 0;
};

struct GraphConfigResult {
  GraphConfig config;
  std::string test_case_label;
};

struct GraphConfigPanel {
  std::vector<std::string> op_labels = {"BFS", "DFS", "Dijkstra's"};
  int op_selected = 0;

  std::vector<GraphPreset> presets;
  std::vector<std::string> preset_labels;
  int preset_selected = 0;

  std::vector<std::string> source_labels;
  int source_selected = 0;

  std::function<void(const GraphConfigResult&)> on_apply;
  std::function<void()> on_close;

  ftxui::Component component;

  void init(std::function<void(const GraphConfigResult&)> apply_cb,
            std::function<void()> close_cb) {
    on_apply = std::move(apply_cb);
    on_close = std::move(close_cb);

    presets = graph_presets();
    preset_labels.clear();
    for (const auto& p : presets)
      preset_labels.push_back(p.label);
    preset_selected = 0;

    update_source_labels();
    build_component();
  }

  void update_source_labels() {
    source_labels.clear();
    if (preset_selected >= 0 &&
        preset_selected < static_cast<int>(presets.size())) {
      for (const auto& l : presets[preset_selected].def.labels)
        source_labels.push_back(l);
    }
    if (source_selected >= static_cast<int>(source_labels.size()))
      source_selected = 0;
  }

  void build_component() {
    using namespace ftxui;

    auto op_radio = Radiobox(&op_labels, &op_selected);
    auto preset_radio = Radiobox(&preset_labels, &preset_selected);
    auto source_radio = Radiobox(&source_labels, &source_selected);

    auto inner =
        Container::Vertical({op_radio, preset_radio, source_radio});

    auto renderer = Renderer(inner, [=, this] {
      Elements content;
      content.push_back(text(" Configure: Graphs ") | bold | center);
      content.push_back(separator());

      Elements cols;
      cols.push_back(vbox({
                         text("Operation") | bold | underlined,
                         op_radio->Render(),
                     }) |
                     flex);
      cols.push_back(separator());
      cols.push_back(vbox({
                         text("Graph") | bold | underlined,
                         preset_radio->Render(),
                     }) |
                     flex);
      cols.push_back(separator());
      cols.push_back(vbox({
                         text("Source") | bold | underlined,
                         source_radio->Render(),
                     }) |
                     flex);
      content.push_back(hbox(std::move(cols)));

      if (preset_selected >= 0 &&
          preset_selected < static_cast<int>(presets.size())) {
        content.push_back(separator());
        content.push_back(text(presets[preset_selected].description) | dim);
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
        update_source_labels();
        GraphConfigResult result;
        result.config.graph_preset = preset_selected;
        static constexpr GraphOp ops[] = {GraphOp::kBfs, GraphOp::kDfs,
                                          GraphOp::kDijkstra};
        result.config.op = ops[op_selected];
        result.config.source = source_selected;
        result.test_case_label = preset_labels[preset_selected];
        if (on_apply)
          on_apply(result);
        return true;
      }
      return false;
    });
  }

  void sync_op(GraphOp op) {
    switch (op) {
    case GraphOp::kBfs:
      op_selected = 0;
      break;
    case GraphOp::kDfs:
      op_selected = 1;
      break;
    case GraphOp::kDijkstra:
      op_selected = 2;
      break;
    default:
      op_selected = 0;
      break;
    }
  }
};

} // namespace viz
