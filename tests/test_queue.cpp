#include "queue.h"

#include <gtest/gtest.h>

// ──────────────────────────────────────────
//  is_empty / size
// ──────────────────────────────────────────

TEST(Queue, IsEmptyInitially) {
  dsa::Queue<int> q;
  EXPECT_TRUE(q.is_empty());
  EXPECT_EQ(q.size(), 0u);
}

TEST(Queue, NotEmptyAfterEnqueue) {
  dsa::Queue<int> q;
  q.enqueue(1);
  EXPECT_FALSE(q.is_empty());
  EXPECT_EQ(q.size(), 1u);
}

TEST(Queue, SizeTracksMultipleEnqueues) {
  dsa::Queue<int> q;
  q.enqueue(1);
  q.enqueue(2);
  q.enqueue(3);
  EXPECT_EQ(q.size(), 3u);
}

// ──────────────────────────────────────────
//  enqueue / dequeue (FIFO order)
// ──────────────────────────────────────────

TEST(Queue, DequeueReturnsFIFOOrder) {
  dsa::Queue<int> q;
  q.enqueue(1);
  q.enqueue(2);
  q.enqueue(3);
  EXPECT_EQ(q.dequeue(), 1);
  EXPECT_EQ(q.dequeue(), 2);
  EXPECT_EQ(q.dequeue(), 3);
}

TEST(Queue, DequeueDecreasesSize) {
  dsa::Queue<int> q;
  q.enqueue(10);
  q.enqueue(20);
  q.dequeue();
  EXPECT_EQ(q.size(), 1u);
}

TEST(Queue, DequeueUntilEmpty) {
  dsa::Queue<int> q;
  q.enqueue(7);
  q.dequeue();
  EXPECT_TRUE(q.is_empty());
  EXPECT_EQ(q.size(), 0u);
}

// ──────────────────────────────────────────
//  peek
// ──────────────────────────────────────────

TEST(Queue, PeekReturnsFrontWithoutRemoving) {
  dsa::Queue<int> q;
  q.enqueue(42);
  EXPECT_EQ(q.peek(), 42);
  EXPECT_EQ(q.size(), 1u); // still there
}

TEST(Queue, PeekReflectsOldestElement) {
  dsa::Queue<int> q;
  q.enqueue(1);
  q.enqueue(99);
  EXPECT_EQ(q.peek(), 1); // front is the first enqueued
}

// ──────────────────────────────────────────
//  error cases
// ──────────────────────────────────────────

TEST(Queue, DequeueOnEmptyThrows) {
  dsa::Queue<int> q;
  EXPECT_THROW(q.dequeue(), std::out_of_range);
}

TEST(Queue, PeekOnEmptyThrows) {
  dsa::Queue<int> q;
  EXPECT_THROW(q.peek(), std::out_of_range);
}
