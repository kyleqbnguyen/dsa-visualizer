#include <algorithm>
#include <string>
#include <vector>

#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

#include "array_viz.h"
#include "config_overlay.h"
#include "list_viz.h"

namespace {

struct MenuEntry {
  std::string label;
  bool is_category = false;
  int algo_id = -1;
};

constexpr int kLinearSearch = 0;
constexpr int kBinarySearch = 1;
constexpr int kBubbleSort = 2;
constexpr int kTwoCrystalBalls = 3;
constexpr int kSinglyLinkedList = 4;
constexpr int kDoublyLinkedList = 5;

auto build_menu_entries() -> std::vector<MenuEntry> {
  return {
      {.label = "Arrays", .is_category = true},
      {.label = "  Linear Search", .is_category = false, .algo_id = kLinearSearch},
      {.label = "  Binary Search", .is_category = false, .algo_id = kBinarySearch},
      {.label = "  Bubble Sort", .is_category = false, .algo_id = kBubbleSort},
      {.label = "  Two Crystal Balls",
       .is_category = false,
       .algo_id = kTwoCrystalBalls},
      {.label = "Linked Lists", .is_category = true},
      {.label = "  Singly Linked List", .is_category = false, .algo_id = kSinglyLinkedList},
      {.label = "  Doubly Linked List", .is_category = false, .algo_id = kDoublyLinkedList},
      {.label = "Trees", .is_category = true},
      {.label = "Graphs", .is_category = true},
  };
}

int show_menu() {
  using namespace ftxui;

  auto screen = ScreenInteractive::Fullscreen();

  auto entries = build_menu_entries();
  int selected = 1;
  bool quit = false;
  bool launch = false;

  auto move_selection = [&](int dir) {
    int next = selected + dir;
    while (next >= 0 && next < static_cast<int>(entries.size())) {
      if (!entries[next].is_category) {
        selected = next;
        return;
      }
      next += dir;
    }
  };

  auto renderer = Renderer([&] {
    std::vector<Element> menu_rows;
    for (int i = 0; i < static_cast<int>(entries.size()); ++i) {
      const auto &entry = entries[i];
      if (entry.is_category) {
        menu_rows.push_back(
            text(entry.label) | bold | color(Color::Cyan));
      } else {
        if (i == selected) {
          menu_rows.push_back(
              text("> " + entry.label) | bold | inverted);
        } else {
          menu_rows.push_back(text("  " + entry.label));
        }
      }
    }

    return vbox({
        text("DSA Visualizer") | bold | center,
        text("ThePrimeagen's Algorithm Course - C++20 Edition") | dim |
            center,
        separator(),
        hbox({
            vbox({
                text("Select an algorithm:") | bold,
                separator(),
                vbox(std::move(menu_rows)) | flex,
            }) | border |
                size(WIDTH, EQUAL, 44),
            separator(),
            vbox({
                text("Controls") | bold | center,
                separator(),
                text("  Enter      - Open visualizer"),
                text("  j/k        - Navigate"),
                text("  q/Esc      - Quit"),
                text(""),
                text("Inside a visualizer:") | bold,
                text("  Space      - Run / Pause"),
                text("  N          - Step forward"),
                text("  B          - Step backward"),
                text("  R          - Reset"),
                text("  +/-        - Adjust speed"),
                text("  C          - Configure dataset"),
                text("  T          - Toggle trace panel"),
                text("  V          - Toggle state panel"),
                text("  D          - Toggle code panel"),
                text("  q/Esc      - Return to menu"),
            }) | border |
                flex,
        }) | flex,
    });
  });

  auto component = CatchEvent(renderer, [&](Event event) {
    if (event == Event::Character('q') || event == Event::Escape) {
      quit = true;
      screen.Exit();
      return true;
    }
    if (event == Event::Return) {
      if (selected >= 0 && selected < static_cast<int>(entries.size()) &&
          !entries[selected].is_category) {
        launch = true;
        screen.Exit();
      }
      return true;
    }
    if (event == Event::ArrowUp || event == Event::Character('k')) {
      move_selection(-1);
      return true;
    }
    if (event == Event::ArrowDown || event == Event::Character('j')) {
      move_selection(1);
      return true;
    }
    return false;
  });

  screen.Loop(component);

  if (quit)
    return -1;
  if (launch)
    return entries[selected].algo_id;
  return -1;
}

void run_selected(int algo_id) {
  constexpr int kDefaultSize = 15;
  auto data = viz::generators::random_array(kDefaultSize);
  int target = 0;

  if (algo_id == kTwoCrystalBalls) {
    data = viz::generators::two_crystal_balls_array(kDefaultSize);
  }

  bool needs_target = (algo_id == kLinearSearch || algo_id == kBinarySearch);
  if (needs_target) {
    target = viz::generators::pick_target(data);
  }

  switch (algo_id) {
  case kLinearSearch:
    viz::run_linear_search_viz(data, target);
    break;
  case kBinarySearch:
    viz::run_binary_search_viz(data, target);
    break;
  case kBubbleSort:
    viz::run_bubble_sort_viz(data);
    break;
  case kTwoCrystalBalls:
    viz::run_two_crystal_balls_viz(data);
    break;
  case kSinglyLinkedList:
    viz::run_singly_linked_list_viz({3, 7, 12, 5, 9});
    break;
  case kDoublyLinkedList:
    viz::run_doubly_linked_list_viz({3, 7, 12, 5, 9});
    break;
  default:
    break;
  }
}

} // anonymous namespace

int main() {
  while (true) {
    int selection = show_menu();
    if (selection < 0)
      break;
    run_selected(selection);
  }
  return 0;
}
