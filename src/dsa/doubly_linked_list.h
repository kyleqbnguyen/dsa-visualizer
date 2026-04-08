#pragma once

#include <cstddef>

namespace dsa {

template <typename T> struct DoublyNode {
  T val;
  DoublyNode<T>* next = nullptr;
  DoublyNode<T>* prev = nullptr;
  explicit DoublyNode(T v) : val(v) {}
};

template <typename T> class DoublyLinkedList {
public:
  DoublyLinkedList() = default;

  ~DoublyLinkedList() {
    auto* curr = head_;
    while (curr) {
      auto* tmp = curr;
      curr = curr->next;
      delete tmp;
    }
  }

  DoublyLinkedList(const DoublyLinkedList&) = delete;
  DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;

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
  DoublyNode<T>* head_ = nullptr;
  DoublyNode<T>* tail_ = nullptr;
  size_t length_ = 0;
};

} // namespace dsa
