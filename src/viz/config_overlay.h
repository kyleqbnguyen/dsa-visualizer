#pragma once

#include <algorithm>
#include <charconv>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/dom/elements.hpp"

#include "test_cases.h"

namespace viz {

namespace generators {

inline auto random_array(int size, int min_val = 1, int max_val = 50)
    -> std::vector<int> {
  std::vector<int> data(size);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(min_val, max_val);
  std::generate(data.begin(), data.end(), [&]() { return dist(gen); });
  return data;
}

inline auto sorted_asc(int size) -> std::vector<int> {
  auto data = random_array(size);
  std::sort(data.begin(), data.end());
  return data;
}

inline auto sorted_desc(int size) -> std::vector<int> {
  auto data = random_array(size);
  std::sort(data.begin(), data.end(), std::greater<>());
  return data;
}

inline auto nearly_sorted(int size) -> std::vector<int> {
  auto data = sorted_asc(size);
  std::random_device rd;
  std::mt19937 gen(rd());
  int swaps = std::max(1, size / 5);
  std::uniform_int_distribution<int> idx_dist(0, size - 1);
  for (int s = 0; s < swaps; ++s) {
    int a = idx_dist(gen);
    int b = idx_dist(gen);
    std::swap(data[a], data[b]);
  }
  return data;
}

inline auto few_unique(int size) -> std::vector<int> {
  std::vector<int> values = {5, 15, 30, 45};
  std::vector<int> data(size);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(0,
                                          static_cast<int>(values.size()) - 1);
  std::generate(data.begin(), data.end(),
                [&]() { return values[dist(gen)]; });
  return data;
}

inline auto clustered(int size) -> std::vector<int> {
  std::vector<int> data;
  data.reserve(size);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::vector<int> centers = {10, 30, 50};
  std::uniform_int_distribution<int> center_pick(
      0, static_cast<int>(centers.size()) - 1);
  std::normal_distribution<double> spread(0.0, 3.0);
  for (int i = 0; i < size; ++i) {
    int c = centers[center_pick(gen)];
    int val = c + static_cast<int>(spread(gen));
    val = std::clamp(val, 1, 60);
    data.push_back(val);
  }
  return data;
}

inline auto pick_target(const std::vector<int> &data) -> int {
  if (data.empty())
    return 0;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> coin(0, 4);
  if (coin(gen) > 0) {
    std::uniform_int_distribution<int> idx_dist(
        0, static_cast<int>(data.size()) - 1);
    return data[idx_dist(gen)];
  }
  return 99;
}

inline auto parse_csv_ints(const std::string &input) -> std::vector<int> {
  std::vector<int> result;
  std::istringstream stream(input);
  std::string token;
  while (std::getline(stream, token, ',')) {
    auto start = token.find_first_not_of(" \t");
    auto end = token.find_last_not_of(" \t");
    if (start == std::string::npos)
      continue;
    token = token.substr(start, end - start + 1);
    if (token.empty())
      continue;
    int val = 0;
    auto [ptr, ec] =
        std::from_chars(token.data(), token.data() + token.size(), val);
    if (ec == std::errc{} && ptr == token.data() + token.size()) {
      result.push_back(val);
    }
  }
  return result;
}

} // namespace generators

struct ConfigResult {
  std::vector<int> data;
  int target = 0;
  std::string test_case_label;
  bool applied = false;
};

struct ConfigPanel {
  std::string algo_name;
  bool needs_target = false;

  std::vector<std::string> size_labels = {"Small (8)", "Medium (15)",
                                          "Large (25)"};
  int size_selected = 1;

  std::vector<std::string> dist_labels = {"Random", "Nearly sorted",
                                          "Reversed", "Few unique",
                                          "Clustered"};
  int dist_selected = 0;

  std::vector<std::string> target_labels = {"Automatic", "Manual input"};
  int target_selected = 0;
  std::string manual_target_str = "7";

  std::vector<TestCase> test_cases;
  std::vector<std::string> test_case_labels;
  int test_case_selected = 0;

  std::vector<std::string> tab_labels = {"Dataset", "Test Cases"};
  int tab_selected = 0;

  std::string validation_msg;

  std::function<void(const ConfigResult &)> on_apply;
  std::function<void()> on_close;

  ftxui::Component component;

  void init(const std::string &algo, bool target_needed, int current_target,
            std::function<void(const ConfigResult &)> apply_cb,
            std::function<void()> close_cb) {
    algo_name = algo;
    needs_target = target_needed;
    manual_target_str = std::to_string(current_target);
    on_apply = std::move(apply_cb);
    on_close = std::move(close_cb);

    test_cases = get_test_cases(algo_name);
    test_case_labels.clear();
    test_case_labels.push_back("(None)");
    for (const auto &tc : test_cases) {
      test_case_labels.push_back(tc.label);
    }
    test_case_selected = 0;
    validation_msg.clear();

    build_component();
  }

  void build_component() {
    using namespace ftxui;

    auto size_radio = Radiobox(&size_labels, &size_selected);
    auto dist_radio = Radiobox(&dist_labels, &dist_selected);
    auto target_radio = Radiobox(&target_labels, &target_selected);

    auto manual_target_opt = InputOption();
    manual_target_opt.multiline = false;
    auto manual_target_input =
        Input(&manual_target_str, "target value", manual_target_opt);

    auto test_case_menu = Radiobox(&test_case_labels, &test_case_selected);
    auto tab_toggle = Toggle(&tab_labels, &tab_selected);

    std::vector<Component> dataset_parts;
    dataset_parts.push_back(size_radio);
    dataset_parts.push_back(dist_radio);
    if (needs_target) {
      dataset_parts.push_back(target_radio);
      dataset_parts.push_back(manual_target_input);
    }
    auto dataset_container = Container::Vertical(std::move(dataset_parts));

    auto test_case_container = Container::Vertical({test_case_menu});

    auto tab_container =
        Container::Tab({dataset_container, test_case_container}, &tab_selected);

    auto inner = Container::Vertical({tab_toggle, tab_container});

    auto renderer = Renderer(inner, [=, this] {
      bool show_manual = needs_target && (target_selected == 1);

      Elements content;
      content.push_back(
          text(" Configure: " + algo_name + " ") | bold | center);
      content.push_back(separator());
      content.push_back(tab_toggle->Render() | center);
      content.push_back(separator());

      if (tab_selected == 0) {
        auto size_col = vbox({
            text("Size") | bold | underlined,
            size_radio->Render(),
        });

        auto dist_col = vbox({
            text("Distribution") | bold | underlined,
            dist_radio->Render(),
        });

        Elements cols;
        cols.push_back(size_col | flex);
        cols.push_back(separator());
        cols.push_back(dist_col | flex);

        if (needs_target) {
          Elements target_els;
          target_els.push_back(text("Target") | bold | underlined);
          target_els.push_back(target_radio->Render());
          if (show_manual) {
            target_els.push_back(manual_target_input->Render() | border);
          }
          cols.push_back(separator());
          cols.push_back(vbox(std::move(target_els)) | flex);
        }

        content.push_back(hbox(std::move(cols)));
      } else {
        content.push_back(text("Predefined test cases:") | bold);
        content.push_back(test_case_menu->Render());
        if (test_case_selected > 0 &&
            test_case_selected <= static_cast<int>(test_cases.size())) {
          content.push_back(separator());
          auto &tc = test_cases[test_case_selected - 1];
          content.push_back(text(tc.description) | dim);
          std::string preview;
          for (int i = 0; i < static_cast<int>(tc.data.size()); ++i) {
            if (i > 0)
              preview += ", ";
            preview += std::to_string(tc.data[i]);
          }
          content.push_back(text("Data: [" + preview + "]") | dim);
          if (needs_target) {
            content.push_back(
                text("Target: " + std::to_string(tc.target)) | dim);
          }
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
          text("  "),
          text(" [Tab] Switch section ") | dim,
      }));

      return vbox(std::move(content)) | border;
    });

    component = CatchEvent(renderer, [this](Event event) -> bool {
      if (event == Event::Escape ||
          event == Event::Character('c') ||
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
    ConfigResult result;

    if (tab_selected == 1) {
      if (test_case_selected > 0 &&
          test_case_selected <= static_cast<int>(test_cases.size())) {
        auto &tc = test_cases[test_case_selected - 1];
        result.data = tc.data;
        result.target = tc.target;
        result.test_case_label = tc.label;
        result.applied = true;
        validation_msg.clear();
        if (on_apply)
          on_apply(result);
      }
      return;
    }

    int sz = 15;
    switch (size_selected) {
    case 0:
      sz = 8;
      break;
    case 1:
      sz = 15;
      break;
    case 2:
      sz = 25;
      break;
    }

    std::vector<int> data;
    switch (dist_selected) {
    case 0:
      data = generators::random_array(sz);
      break;
    case 1:
      data = generators::nearly_sorted(sz);
      break;
    case 2:
      data = generators::sorted_desc(sz);
      break;
    case 3:
      data = generators::few_unique(sz);
      break;
    case 4:
      data = generators::clustered(sz);
      break;
    }

    if (data.empty()) {
      validation_msg = "Error: could not generate dataset.";
      return;
    }

    result.data = data;

    if (needs_target) {
      if (target_selected == 1) {
        auto targets = generators::parse_csv_ints(manual_target_str);
        if (targets.empty()) {
          validation_msg = "Error: invalid target value.";
          return;
        }
        result.target = targets[0];
      } else {
        result.target = generators::pick_target(data);
      }
    }

    result.test_case_label = "";
    result.applied = true;
    validation_msg.clear();
    if (on_apply)
      on_apply(result);
  }
};

} // namespace viz
