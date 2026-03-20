#pragma once

#include <cstddef>
#include <vector>

namespace dsa {

// Jump by sqrt(n) to find the first breaking block, then scan linearly
// within that block to return the first breaking index. Return -1 if no
// breaking floor exists.
inline auto two_crystal_balls(const std::vector<int> &breaks) -> int {
  // TODO
  (void)breaks;
  return -1;
}

} // namespace dsa
