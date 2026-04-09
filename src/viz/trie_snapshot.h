#pragma once

#include <string>
#include <vector>

namespace viz {

enum class TrieVizOp {
  kNone,
  kInsert,
  kSearch,
  kDelete,
};

enum class TrieNodeState {
  kNormal,
  kActive,
  kNew,
  kFound,
  kNotFound,
  kDeleted,
  kEndOfWord,
};

struct TrieNodeSnap {
  char edge_char = '\0';
  int parent = -1;
  std::vector<int> children;
  bool is_end = false;
  TrieNodeState state = TrieNodeState::kNormal;
};

struct TrieStepSnapshot {
  std::vector<TrieNodeSnap> nodes;
  std::string current_word;
  int current_depth = -1;

  std::string status_text;
  std::string trace_entry;
  int current_line = -1;
  std::vector<std::pair<std::string, std::string>> variables;
};

struct TrieRecording {
  std::string title;
  std::string algorithm_name = "trie";
  std::vector<TrieStepSnapshot> steps;
};

} // namespace viz
