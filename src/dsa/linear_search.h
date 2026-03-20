#pragma once

#include <cstddef>
#include <vector>

namespace dsa {

// Return index of target; return -1 when not found.
inline auto linear_search(const std::vector<int> &data, int target) -> int {
  for (std::size_t i {}; i < data.size(); i++) {
    if (data[i] == target) {
      return i;
    }
  }
  return -1;
}

} // namespace dsa
