#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>

namespace dsa {

template <typename T, std::size_t N> class RingBuffer {
public:
  RingBuffer() = default;

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

  bool is_full() const {
    // TODO: implement
    return false;
  }

  std::size_t size() const {
    // TODO: implement
    return 0;
  }

  static constexpr std::size_t capacity() { return N; }

private:
  std::array<T, N> data_{};
  std::size_t head_ = 0;
  std::size_t tail_ = 0;
  std::size_t size_ = 0;
};

} // namespace dsa
