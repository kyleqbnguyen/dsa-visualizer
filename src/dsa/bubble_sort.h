#pragma once

#include <cstddef>
#include <vector>

namespace dsa {

// Sort data in place in ascending order.
inline void bubble_sort(std::vector<int> &data) {
  for (std::size_t i {}; i < data.size(); i++) {
    for (std::size_t j{}; j < data.size() - 1 - i; j++) {
      int tmp {data[j]};

      if (data[j] > data[j + 1]) {
        data[j] = data[j + 1];
        data[j + 1] = tmp;
      }
    }
  }
}

} // namespace dsa
