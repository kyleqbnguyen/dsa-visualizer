#pragma once

#include <concepts>
#include <functional>
#include <span>
#include <utility>

namespace dsa {

template <typename T>
using SortStepCallback =
    std::function<void(std::span<const T>, std::size_t, std::size_t, bool)>;

template <std::totally_ordered T>
void bubble_sort(std::span<T> data, SortStepCallback<T> on_step = {}) {
  if (data.size() <= 1)
    return;

  for (std::size_t i = 0; i < data.size(); ++i) {
    bool swapped = false;
    for (std::size_t j = 0; j < data.size() - 1 - i; ++j) {
      bool will_swap = data[j] > data[j + 1];
      if (on_step) {
        on_step(data, j, j + 1, will_swap);
      }
      if (will_swap) {
        std::swap(data[j], data[j + 1]);
        swapped = true;
      }
    }
    if (!swapped)
      break;
  }
}

} // namespace dsa
