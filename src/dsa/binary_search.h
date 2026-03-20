#pragma once

#include <cstddef>
#include <vector>

namespace dsa {

// Return index of target from sorted data; return -1 when not found.
inline auto binary_search(const std::vector<int> &data, int target) -> int {
  std::size_t lo {0};
  std::size_t hi {data.size()};
  std::size_t mid {};

  while (lo < hi) {
    mid = lo + ((hi - lo)/ 2);
    if (data[mid] == target) {
      return mid;
    } else if (target < data[mid]) {
      hi = mid;
    } else {
      lo = mid + 1;
    }
  }
  return -1;
}

} // namespace dsa
