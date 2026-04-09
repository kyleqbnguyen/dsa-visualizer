#pragma once

#include <array>
#include <string>
#include <vector>

namespace dsa {

struct TrieNode {
  std::array<int, 26> children{};
  bool is_end = false;

  TrieNode() { children.fill(-1); }
};

class Trie {
public:
  Trie() { nodes_.emplace_back(); }

  void insert(const std::string& word) {
    // TODO: implement
    (void)word;
  }

  auto search(const std::string& word) const -> bool {
    // TODO: implement
    (void)word;
    return false;
  }

  auto startsWith(const std::string& prefix) const -> bool {
    // TODO: implement
    (void)prefix;
    return false;
  }

  auto nodes() const -> const std::vector<TrieNode>& { return nodes_; }

private:
  std::vector<TrieNode> nodes_;
};

} // namespace dsa
