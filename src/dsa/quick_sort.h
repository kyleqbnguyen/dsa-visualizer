#pragma once

#include <vector>

namespace dsa {

inline void quick_sort(std::vector<int>& arr, int lo, int hi) {
  // TODO: implement
  (void)arr;
  (void)lo;
  (void)hi;
}

inline void quick_sort(std::vector<int>& arr) {
  if (!arr.empty())
    quick_sort(arr, 0, static_cast<int>(arr.size()) - 1);
}

} // namespace dsa
