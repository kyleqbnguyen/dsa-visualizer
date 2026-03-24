#include "singly_linked_list.h"
#include <gtest/gtest.h>

TEST(SinglyPrepend, ToEmpty) {
  dsa::SinglyLinkedList<int> list;
  list.prepend(42);
  EXPECT_EQ(list.length(), 1u);
  EXPECT_EQ(list.get(0), 42);
}

TEST(SinglyPrepend, ToNonEmpty) {
  dsa::SinglyLinkedList<int> list;
  list.append(1);
  list.append(2);
  list.prepend(99);
  EXPECT_EQ(list.length(), 3u);
  EXPECT_EQ(list.get(0), 99);
  EXPECT_EQ(list.get(1), 1);
}

TEST(SinglyAppend, ToEmpty) {
  dsa::SinglyLinkedList<int> list;
  list.append(42);
  EXPECT_EQ(list.length(), 1u);
  EXPECT_EQ(list.get(0), 42);
}

TEST(SinglyAppend, ToNonEmpty) {
  dsa::SinglyLinkedList<int> list;
  list.append(1);
  list.append(2);
  list.append(99);
  EXPECT_EQ(list.length(), 3u);
  EXPECT_EQ(list.get(2), 99);
}

TEST(SinglyInsertAt, Middle) {
  dsa::SinglyLinkedList<int> list;
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

TEST(SinglyInsertAt, IndexZero) {
  dsa::SinglyLinkedList<int> list;
  list.append(1);
  list.append(2);
  list.insert_at(0, 99);
  EXPECT_EQ(list.length(), 3u);
  EXPECT_EQ(list.get(0), 99);
}

TEST(SinglyInsertAt, IndexLength) {
  dsa::SinglyLinkedList<int> list;
  list.append(1);
  list.append(2);
  list.insert_at(2, 99);
  EXPECT_EQ(list.length(), 3u);
  EXPECT_EQ(list.get(2), 99);
}

TEST(SinglyRemoveAt, Middle) {
  dsa::SinglyLinkedList<int> list;
  list.append(1);
  list.append(2);
  list.append(3);
  list.remove_at(1);
  EXPECT_EQ(list.length(), 2u);
  EXPECT_EQ(list.get(0), 1);
  EXPECT_EQ(list.get(1), 3);
}

TEST(SinglyRemoveAt, Head) {
  dsa::SinglyLinkedList<int> list;
  list.append(1);
  list.append(2);
  list.append(3);
  list.remove_at(0);
  EXPECT_EQ(list.length(), 2u);
  EXPECT_EQ(list.get(0), 2);
}

TEST(SinglyGet, ValidIndex) {
  dsa::SinglyLinkedList<int> list;
  list.append(10);
  list.append(20);
  list.append(30);
  EXPECT_EQ(list.get(0), 10);
  EXPECT_EQ(list.get(1), 20);
  EXPECT_EQ(list.get(2), 30);
}

TEST(SinglyGet, OutOfBounds) {
  dsa::SinglyLinkedList<int> list;
  list.append(1);
  EXPECT_THROW(list.get(5), std::out_of_range);
}
