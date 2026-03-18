#pragma once

#include <concepts>
#include <functional>
#include <optional>
#include <span>

namespace dsa {

template <typename T>
using BinarySearchStepCallback =
    std::function<void(std::span<const T>, std::size_t, std::size_t,
                       std::size_t, const T &, bool)>;

template <std::totally_ordered T>
auto binary_search(std::span<const T> data, const T &target,
                   BinarySearchStepCallback<T> on_step = {})
    -> std::optional<std::size_t> {
  if (data.empty())
    return std::nullopt;

  std::size_t low = 0;
  std::size_t high = data.size();

  while (low < high) {
    std::size_t mid = low + (high - low) / 2;
    bool found = (data[mid] == target);

    if (on_step) {
      on_step(data, low, high, mid, target, found);
    }

    if (found) {
      return mid;
    } else if (data[mid] < target) {
      low = mid + 1;
    } else {
      high = mid;
    }
  }

  return std::nullopt;
}

} // namespace dsa
