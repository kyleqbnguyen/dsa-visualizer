#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace dsa {

template <typename T>
class Stack {
public:
  Stack() = default;

  void push(T val) {
    // TODO: implement
    (void)val;
  }

  T pop() {
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
  std::vector<T> data_;
};

} // namespace dsa
