#pragma once

#include <cmath>
#include <cstddef>
#include <vector>

namespace dsa {

// Jump by sqrt(n) to find the first breaking block, then scan linearly
// within that block to return the first breaking index. Return -1 if no
// breaking floor exists.
inline auto two_crystal_balls(const std::vector<int> &breaks) -> int {
  if (breaks.empty()) {
    return -1;
  }

  std::size_t jumpLength = static_cast<std::size_t>(std::sqrt(breaks.size()));
  if (jumpLength == 0) {
    jumpLength = 1;
  }

  std::size_t i = jumpLength;

  while (i < breaks.size() && breaks[i] == 0) {
    i += jumpLength;
  }

  std::size_t start = (i >= jumpLength) ? (i - jumpLength) : 0;
  std::size_t end = std::min(i, breaks.size() - 1);

  for (std::size_t j = start; j <= end; ++j) {
    if (breaks[j] == 1) {
      return static_cast<int>(j);
    }
  }

  return -1;
}

} // namespace dsa
