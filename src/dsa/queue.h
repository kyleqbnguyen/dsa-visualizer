#pragma once

#include <cstddef>
#include <deque>
#include <stdexcept>

namespace dsa {

template <typename T> class Queue {
public:
  Queue() = default;

  void enqueue(T val) {
    // TODO: implement
    (void)val;
  }

  T dequeue() {
    // TODO: implement
    return T{};
  }

  T peek() const {
    // TODO: implement
    return T{};
  }

  bool is_empty() const {
    // TODO: implement
    return true;
  }

  std::size_t size() const {
    // TODO: implement
    return 0;
  }

private:
  std::deque<T> data_;
};

} // namespace dsa
