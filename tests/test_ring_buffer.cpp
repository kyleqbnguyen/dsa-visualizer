#include "ring_buffer.h"

#include <gtest/gtest.h>

// ──────────────────────────────────────────
//  is_empty / is_full / size
// ──────────────────────────────────────────

TEST(RingBuffer, IsEmptyInitially) {
  dsa::RingBuffer<int, 4> rb;
  EXPECT_TRUE(rb.is_empty());
  EXPECT_EQ(rb.size(), 0u);
}

TEST(RingBuffer, IsNotFullInitially) {
  dsa::RingBuffer<int, 4> rb;
  EXPECT_FALSE(rb.is_full());
}

TEST(RingBuffer, NotEmptyAfterEnqueue) {
  dsa::RingBuffer<int, 4> rb;
  rb.enqueue(1);
  EXPECT_FALSE(rb.is_empty());
  EXPECT_EQ(rb.size(), 1u);
}

TEST(RingBuffer, SizeTracksMultipleEnqueues) {
  dsa::RingBuffer<int, 4> rb;
  rb.enqueue(1);
  rb.enqueue(2);
  rb.enqueue(3);
  EXPECT_EQ(rb.size(), 3u);
}

TEST(RingBuffer, IsFullAfterCapacityEnqueues) {
  dsa::RingBuffer<int, 4> rb;
  rb.enqueue(1);
  rb.enqueue(2);
  rb.enqueue(3);
  rb.enqueue(4);
  EXPECT_TRUE(rb.is_full());
  EXPECT_EQ(rb.size(), 4u);
}

TEST(RingBuffer, CapacityConstant) {
  constexpr auto cap = dsa::RingBuffer<int, 8>::capacity();
  EXPECT_EQ(cap, 8u);
}

// ──────────────────────────────────────────
//  enqueue / dequeue (FIFO order)
// ──────────────────────────────────────────

TEST(RingBuffer, DequeueReturnsFIFOOrder) {
  dsa::RingBuffer<int, 4> rb;
  rb.enqueue(1);
  rb.enqueue(2);
  rb.enqueue(3);
  EXPECT_EQ(rb.dequeue(), 1);
  EXPECT_EQ(rb.dequeue(), 2);
  EXPECT_EQ(rb.dequeue(), 3);
}

TEST(RingBuffer, DequeueDecreasesSize) {
  dsa::RingBuffer<int, 4> rb;
  rb.enqueue(10);
  rb.enqueue(20);
  rb.dequeue();
  EXPECT_EQ(rb.size(), 1u);
}

TEST(RingBuffer, DequeueUntilEmpty) {
  dsa::RingBuffer<int, 4> rb;
  rb.enqueue(7);
  rb.dequeue();
  EXPECT_TRUE(rb.is_empty());
  EXPECT_EQ(rb.size(), 0u);
}

TEST(RingBuffer, WrapAroundBehavior) {
  dsa::RingBuffer<int, 4> rb;
  rb.enqueue(1);
  rb.enqueue(2);
  rb.dequeue();
  rb.dequeue();
  rb.enqueue(3);
  rb.enqueue(4);
  rb.enqueue(5);
  EXPECT_EQ(rb.dequeue(), 3);
  EXPECT_EQ(rb.dequeue(), 4);
  EXPECT_EQ(rb.dequeue(), 5);
}

// ──────────────────────────────────────────
//  peek
// ──────────────────────────────────────────

TEST(RingBuffer, PeekReturnsFrontWithoutRemoving) {
  dsa::RingBuffer<int, 4> rb;
  rb.enqueue(42);
  EXPECT_EQ(rb.peek(), 42);
  EXPECT_EQ(rb.size(), 1u);
}

TEST(RingBuffer, PeekReflectsOldestElement) {
  dsa::RingBuffer<int, 4> rb;
  rb.enqueue(1);
  rb.enqueue(99);
  EXPECT_EQ(rb.peek(), 1);
}

// ──────────────────────────────────────────
//  error cases
// ──────────────────────────────────────────

TEST(RingBuffer, DequeueOnEmptyThrows) {
  dsa::RingBuffer<int, 4> rb;
  EXPECT_THROW(rb.dequeue(), std::out_of_range);
}

TEST(RingBuffer, PeekOnEmptyThrows) {
  dsa::RingBuffer<int, 4> rb;
  EXPECT_THROW(rb.peek(), std::out_of_range);
}

TEST(RingBuffer, EnqueueOnFullThrows) {
  dsa::RingBuffer<int, 2> rb;
  rb.enqueue(1);
  rb.enqueue(2);
  EXPECT_THROW(rb.enqueue(3), std::out_of_range);
}
