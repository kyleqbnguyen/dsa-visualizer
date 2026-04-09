#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace dsa {

class MinHeap {
public:
  void push(int value) {
    // TODO: implement
    (void)value;
  }

  auto pop() -> int {
    // TODO: implement
    return 0;
  }

  auto peek() const -> int {
    // TODO: implement
    return 0;
  }

  auto size() const -> size_t { return data_.size(); }
  auto empty() const -> bool { return data_.empty(); }
  auto data() const -> const std::vector<int>& { return data_; }

  static auto buildHeap(std::vector<int> arr) -> MinHeap {
    // TODO: implement
    (void)arr;
    return MinHeap{};
  }

private:
  std::vector<int> data_;

  void heapifyUp(size_t idx) {
    // TODO: implement
    (void)idx;
  }

  void heapifyDown(size_t idx) {
    // TODO: implement
    (void)idx;
  }
};

} // namespace dsa
