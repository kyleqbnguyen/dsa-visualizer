#include "trie_recorder.h"

#include <gtest/gtest.h>

static const std::vector<std::string> kDict = {"app", "apple", "ape", "bat", "ball"};

TEST(TrieViz, InsertHasSteps) {
  auto rec = viz::record_trie_insert(kDict, "bad");
  EXPECT_FALSE(rec.steps.empty());
}

TEST(TrieViz, InsertNewWordCreatesNodes) {
  auto rec = viz::record_trie_insert(kDict, "bad");
  bool found_new = false;
  for (const auto& step : rec.steps) {
    for (const auto& node : step.nodes) {
      if (node.state == viz::TrieNodeState::kNew)
        found_new = true;
    }
  }
  EXPECT_TRUE(found_new);
}

TEST(TrieViz, InsertExistingPrefix) {
  auto rec = viz::record_trie_insert(kDict, "app");
  EXPECT_FALSE(rec.steps.empty());
}

TEST(TrieViz, SearchFound) {
  auto rec = viz::record_trie_search(kDict, "apple");
  const auto& last = rec.steps.back();
  EXPECT_NE(last.status_text.find("found"), std::string::npos);
}

TEST(TrieViz, SearchNotFound) {
  auto rec = viz::record_trie_search(kDict, "ban");
  const auto& last = rec.steps.back();
  bool is_not_found =
      last.status_text.find("not found") != std::string::npos ||
      last.status_text.find("not a complete word") != std::string::npos;
  EXPECT_TRUE(is_not_found);
}

TEST(TrieViz, SearchPrefixOnly) {
  auto rec = viz::record_trie_search(kDict, "ap");
  const auto& last = rec.steps.back();
  EXPECT_NE(last.status_text.find("not a complete word"), std::string::npos);
}

TEST(TrieViz, DeleteExisting) {
  auto rec = viz::record_trie_delete(kDict, "ape");
  const auto& last = rec.steps.back();
  EXPECT_NE(last.status_text.find("complete"), std::string::npos);
}

TEST(TrieViz, DeleteNotFound) {
  auto rec = viz::record_trie_delete(kDict, "xyz");
  const auto& last = rec.steps.back();
  EXPECT_NE(last.status_text.find("not found"), std::string::npos);
}

TEST(TrieViz, IdleHasOneStep) {
  auto rec = viz::record_trie_idle(kDict);
  EXPECT_EQ(rec.steps.size(), 1u);
}
