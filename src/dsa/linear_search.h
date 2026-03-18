#pragma once

#include <concepts>
#include <functional>
#include <optional>
#include <span>

namespace dsa {

template <typename T>
using SearchStepCallback =
    std::function<void(std::span<const T>, std::size_t, const T &, bool)>;

template <std::equality_comparable T>
auto linear_search(std::span<const T> data, const T &target,
                   SearchStepCallback<T> on_step = {})
    -> std::optional<std::size_t> {
  for (std::size_t i = 0; i < data.size(); ++i) {
    bool found = (data[i] == target);
    if (on_step) {
      on_step(data, i, target, found);
    }
    if (found) {
      return i;
    }
  }
  return std::nullopt;
}

} // namespace dsa
