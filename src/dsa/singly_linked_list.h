#pragma once

#include <cstddef>

namespace dsa {

template <typename T> struct SinglyNode {
  T val;
  SinglyNode<T>* next = nullptr;
  explicit SinglyNode(T v) : val(v) {}
};

template <typename T> class SinglyLinkedList {
public:
  SinglyLinkedList() = default;

  ~SinglyLinkedList() {
    auto* curr = head_;
    while (curr) {
      auto* tmp = curr;
      curr = curr->next;
      delete tmp;
    }
  }

  SinglyLinkedList(const SinglyLinkedList&) = delete;
  SinglyLinkedList& operator=(const SinglyLinkedList&) = delete;

  void prepend(T val) {
    // TODO: implement
  }

  void append(T val) {
    // TODO: implement
  }

  void insert_at(size_t index, T val) {
    // TODO: implement
  }

  void remove_at(size_t index) {
    // TODO: implement
  }

  T get(size_t index) const {
    // TODO: implement
    return T{};
  }

  size_t length() const { return length_; }

private:
  SinglyNode<T>* head_ = nullptr;
  SinglyNode<T>* tail_ = nullptr;
  size_t length_ = 0;
};

} // namespace dsa
