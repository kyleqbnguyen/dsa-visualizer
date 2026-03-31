#include "stack.h"
#include <gtest/gtest.h>

// ──────────────────────────────────────────
//  is_empty / size
// ──────────────────────────────────────────

TEST(Stack, IsEmptyInitially) {
  dsa::Stack<int> s;
  EXPECT_TRUE(s.is_empty());
  EXPECT_EQ(s.size(), 0u);
}

TEST(Stack, NotEmptyAfterPush) {
  dsa::Stack<int> s;
  s.push(1);
  EXPECT_FALSE(s.is_empty());
  EXPECT_EQ(s.size(), 1u);
}

TEST(Stack, SizeTracksMultiplePushes) {
  dsa::Stack<int> s;
  s.push(1);
  s.push(2);
  s.push(3);
  EXPECT_EQ(s.size(), 3u);
}

// ──────────────────────────────────────────
//  push / pop (LIFO order)
// ──────────────────────────────────────────

TEST(Stack, PopReturnsLastPushed) {
  dsa::Stack<int> s;
  s.push(1);
  s.push(2);
  s.push(3);
  EXPECT_EQ(s.pop(), 3);
  EXPECT_EQ(s.pop(), 2);
  EXPECT_EQ(s.pop(), 1);
}

TEST(Stack, PopDecreasesSize) {
  dsa::Stack<int> s;
  s.push(10);
  s.push(20);
  s.pop();
  EXPECT_EQ(s.size(), 1u);
}

TEST(Stack, PopUntilEmpty) {
  dsa::Stack<int> s;
  s.push(7);
  s.pop();
  EXPECT_TRUE(s.is_empty());
  EXPECT_EQ(s.size(), 0u);
}

// ──────────────────────────────────────────
//  peek
// ──────────────────────────────────────────

TEST(Stack, PeekReturnsTopWithoutRemoving) {
  dsa::Stack<int> s;
  s.push(42);
  EXPECT_EQ(s.peek(), 42);
  EXPECT_EQ(s.size(), 1u); // still there
}

TEST(Stack, PeekReflectsLatestPush) {
  dsa::Stack<int> s;
  s.push(1);
  s.push(99);
  EXPECT_EQ(s.peek(), 99);
}

// ──────────────────────────────────────────
//  error cases
// ──────────────────────────────────────────

TEST(Stack, PopOnEmptyThrows) {
  dsa::Stack<int> s;
  EXPECT_THROW(s.pop(), std::out_of_range);
}

TEST(Stack, PeekOnEmptyThrows) {
  dsa::Stack<int> s;
  EXPECT_THROW(s.peek(), std::out_of_range);
}
