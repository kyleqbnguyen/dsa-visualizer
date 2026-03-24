#include "doubly_linked_list.h"
#include <gtest/gtest.h>

TEST(DoublyPrepend, ToEmpty) {
  dsa::DoublyLinkedList<int> list;
  list.prepend(42);
  EXPECT_EQ(list.length(), 1u);
  EXPECT_EQ(list.get(0), 42);
}

TEST(DoublyPrepend, ToNonEmpty) {
  dsa::DoublyLinkedList<int> list;
  list.append(1);
  list.append(2);
  list.prepend(99);
  EXPECT_EQ(list.length(), 3u);
  EXPECT_EQ(list.get(0), 99);
  EXPECT_EQ(list.get(1), 1);
}

TEST(DoublyAppend, ToEmpty) {
  dsa::DoublyLinkedList<int> list;
  list.append(42);
  EXPECT_EQ(list.length(), 1u);
  EXPECT_EQ(list.get(0), 42);
}

TEST(DoublyAppend, ToNonEmpty) {
  dsa::DoublyLinkedList<int> list;
  list.append(1);
  list.append(2);
  list.append(99);
  EXPECT_EQ(list.length(), 3u);
  EXPECT_EQ(list.get(2), 99);
}

TEST(DoublyInsertAt, Middle) {
  dsa::DoublyLinkedList<int> list;
  list.append(1);
  list.append(2);
  list.append(3);
  list.insert_at(1, 99);
  EXPECT_EQ(list.length(), 4u);
  EXPECT_EQ(list.get(0), 1);
  EXPECT_EQ(list.get(1), 99);
  EXPECT_EQ(list.get(2), 2);
  EXPECT_EQ(list.get(3), 3);
}

TEST(DoublyInsertAt, IndexZero) {
  dsa::DoublyLinkedList<int> list;
  list.append(1);
  list.append(2);
  list.insert_at(0, 99);
  EXPECT_EQ(list.length(), 3u);
  EXPECT_EQ(list.get(0), 99);
}

TEST(DoublyInsertAt, IndexLength) {
  dsa::DoublyLinkedList<int> list;
  list.append(1);
  list.append(2);
  list.insert_at(2, 99);
  EXPECT_EQ(list.length(), 3u);
  EXPECT_EQ(list.get(2), 99);
}

TEST(DoublyInsertAt, CheckPrevPointers) {
  dsa::DoublyLinkedList<int> list;
  list.append(1);
  list.append(2);
  list.append(3);
  list.insert_at(1, 99);
  // After insert: [1, 99, 2, 3]
  EXPECT_EQ(list.get(0), 1);
  EXPECT_EQ(list.get(1), 99);
  EXPECT_EQ(list.get(2), 2);
  EXPECT_EQ(list.get(3), 3);
  // Verify by removing and checking consistency
  list.remove_at(1);
  // After remove: [1, 2, 3]
  EXPECT_EQ(list.get(0), 1);
  EXPECT_EQ(list.get(1), 2);
  EXPECT_EQ(list.get(2), 3);
}

TEST(DoublyRemoveAt, Middle) {
  dsa::DoublyLinkedList<int> list;
  list.append(1);
  list.append(2);
  list.append(3);
  list.remove_at(1);
  EXPECT_EQ(list.length(), 2u);
  EXPECT_EQ(list.get(0), 1);
  EXPECT_EQ(list.get(1), 3);
}

TEST(DoublyRemoveAt, Head) {
  dsa::DoublyLinkedList<int> list;
  list.append(1);
  list.append(2);
  list.append(3);
  list.remove_at(0);
  EXPECT_EQ(list.length(), 2u);
  EXPECT_EQ(list.get(0), 2);
}

TEST(DoublyRemoveAt, CheckPrevPointers) {
  dsa::DoublyLinkedList<int> list;
  list.append(1);
  list.append(2);
  list.append(3);
  list.append(4);
  list.remove_at(2); // remove 3 -> [1, 2, 4]
  EXPECT_EQ(list.length(), 3u);
  EXPECT_EQ(list.get(0), 1);
  EXPECT_EQ(list.get(1), 2);
  EXPECT_EQ(list.get(2), 4);
  // Verify prev links by inserting back
  list.insert_at(2, 3); // [1, 2, 3, 4]
  EXPECT_EQ(list.get(2), 3);
  EXPECT_EQ(list.get(3), 4);
}

TEST(DoublyGet, ValidIndex) {
  dsa::DoublyLinkedList<int> list;
  list.append(10);
  list.append(20);
  list.append(30);
  EXPECT_EQ(list.get(0), 10);
  EXPECT_EQ(list.get(1), 20);
  EXPECT_EQ(list.get(2), 30);
}

TEST(DoublyGet, OutOfBounds) {
  dsa::DoublyLinkedList<int> list;
  list.append(1);
  EXPECT_THROW(list.get(5), std::out_of_range);
}
