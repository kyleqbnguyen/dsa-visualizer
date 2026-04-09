#pragma once

#include "trie_snapshot.h"

#include <algorithm>
#include <string>
#include <vector>

namespace viz {

namespace trie_detail {

struct FlatTrie {
  std::vector<TrieNodeSnap> nodes;
};

inline auto buildFromWords(const std::vector<std::string>& words) -> FlatTrie {
  FlatTrie ft;
  TrieNodeSnap root;
  root.edge_char = '*';
  ft.nodes.push_back(root);

  for (const auto& word : words) {
    int curr = 0;
    for (char ch : word) {
      int found = -1;
      for (int child : ft.nodes[curr].children) {
        if (ft.nodes[child].edge_char == ch) {
          found = child;
          break;
        }
      }
      if (found < 0) {
        int new_idx = static_cast<int>(ft.nodes.size());
        TrieNodeSnap n;
        n.edge_char = ch;
        n.parent = curr;
        ft.nodes.push_back(n);
        ft.nodes[curr].children.push_back(new_idx);
        curr = new_idx;
      } else {
        curr = found;
      }
    }
    ft.nodes[curr].is_end = true;
  }
  return ft;
}

inline auto snap(const FlatTrie& ft, const std::string& word, int depth)
    -> TrieStepSnapshot {
  TrieStepSnapshot s;
  s.nodes = ft.nodes;
  s.current_word = word;
  s.current_depth = depth;
  return s;
}

inline void setAllNormal(FlatTrie& ft) {
  for (auto& n : ft.nodes)
    n.state = TrieNodeState::kNormal;
}

} // namespace trie_detail

inline auto record_trie_idle(const std::vector<std::string>& words)
    -> TrieRecording {
  TrieRecording rec;
  rec.title = "Trie";

  auto ft = trie_detail::buildFromWords(words);
  auto s = trie_detail::snap(ft, "", -1);
  s.status_text = "Press [C] to select an operation";
  rec.steps.push_back(std::move(s));
  return rec;
}

inline auto record_trie_insert(const std::vector<std::string>& existing,
                               const std::string& word) -> TrieRecording {
  TrieRecording rec;
  rec.title = "Trie — Insert \"" + word + "\"";

  auto ft = trie_detail::buildFromWords(existing);

  {
    auto s = trie_detail::snap(ft, word, -1);
    s.status_text = "Insert \"" + word + "\" into trie";
    s.trace_entry = "insert(\"" + word + "\")";
    s.current_line = 0;
    rec.steps.push_back(std::move(s));
  }

  int curr = 0;
  for (int d = 0; d < static_cast<int>(word.size()); ++d) {
    char ch = word[d];

    int found = -1;
    for (int child : ft.nodes[curr].children) {
      if (ft.nodes[child].edge_char == ch) {
        found = child;
        break;
      }
    }

    if (found >= 0) {
      trie_detail::setAllNormal(ft);
      ft.nodes[found].state = TrieNodeState::kActive;

      auto s = trie_detail::snap(ft, word, d);
      s.status_text = "'" + std::string(1, ch) + "' exists — traverse";
      s.trace_entry = "traverse '" + std::string(1, ch) + "'";
      s.current_line = 2;
      s.variables = {{"char", std::string(1, ch)},
                     {"depth", std::to_string(d + 1)}};
      rec.steps.push_back(std::move(s));

      curr = found;
    } else {
      int new_idx = static_cast<int>(ft.nodes.size());
      TrieNodeSnap n;
      n.edge_char = ch;
      n.parent = curr;
      n.state = TrieNodeState::kNew;
      ft.nodes.push_back(n);
      ft.nodes[curr].children.push_back(new_idx);

      trie_detail::setAllNormal(ft);
      ft.nodes[new_idx].state = TrieNodeState::kNew;

      auto s = trie_detail::snap(ft, word, d);
      s.status_text = "'" + std::string(1, ch) + "' new — create node";
      s.trace_entry = "create '" + std::string(1, ch) + "'";
      s.current_line = 3;
      s.variables = {{"char", std::string(1, ch)},
                     {"depth", std::to_string(d + 1)}};
      rec.steps.push_back(std::move(s));

      curr = new_idx;
    }
  }

  ft.nodes[curr].is_end = true;
  trie_detail::setAllNormal(ft);
  ft.nodes[curr].state = TrieNodeState::kEndOfWord;

  {
    auto s = trie_detail::snap(ft, word, static_cast<int>(word.size()));
    s.status_text = "Mark end-of-word — insert complete";
    s.trace_entry = "mark end-of-word";
    s.current_line = 5;
    rec.steps.push_back(std::move(s));
  }

  return rec;
}

inline auto record_trie_search(const std::vector<std::string>& existing,
                               const std::string& word) -> TrieRecording {
  TrieRecording rec;
  rec.title = "Trie — Search \"" + word + "\"";

  auto ft = trie_detail::buildFromWords(existing);

  {
    auto s = trie_detail::snap(ft, word, -1);
    s.status_text = "Search for \"" + word + "\"";
    s.trace_entry = "search(\"" + word + "\")";
    s.current_line = 0;
    rec.steps.push_back(std::move(s));
  }

  int curr = 0;
  for (int d = 0; d < static_cast<int>(word.size()); ++d) {
    char ch = word[d];

    int found = -1;
    for (int child : ft.nodes[curr].children) {
      if (ft.nodes[child].edge_char == ch) {
        found = child;
        break;
      }
    }

    if (found < 0) {
      trie_detail::setAllNormal(ft);
      ft.nodes[curr].state = TrieNodeState::kNotFound;

      auto s = trie_detail::snap(ft, word, d);
      s.status_text = "'" + std::string(1, ch) +
                      "' not found — word does not exist";
      s.trace_entry = "miss at '" + std::string(1, ch) + "'";
      s.current_line = 2;
      rec.steps.push_back(std::move(s));
      return rec;
    }

    trie_detail::setAllNormal(ft);
    ft.nodes[found].state = TrieNodeState::kActive;

    auto s = trie_detail::snap(ft, word, d);
    s.status_text = "'" + std::string(1, ch) + "' found — continue";
    s.trace_entry = "match '" + std::string(1, ch) + "'";
    s.current_line = 3;
    s.variables = {{"char", std::string(1, ch)},
                   {"depth", std::to_string(d + 1)}};
    rec.steps.push_back(std::move(s));

    curr = found;
  }

  trie_detail::setAllNormal(ft);
  if (ft.nodes[curr].is_end) {
    ft.nodes[curr].state = TrieNodeState::kFound;
    auto s = trie_detail::snap(ft, word, static_cast<int>(word.size()));
    s.status_text = "\"" + word + "\" found!";
    s.trace_entry = "found!";
    s.current_line = 5;
    rec.steps.push_back(std::move(s));
  } else {
    ft.nodes[curr].state = TrieNodeState::kNotFound;
    auto s = trie_detail::snap(ft, word, static_cast<int>(word.size()));
    s.status_text = "Prefix exists but \"" + word +
                    "\" is not a complete word";
    s.trace_entry = "prefix only — not a word";
    s.current_line = 6;
    rec.steps.push_back(std::move(s));
  }

  return rec;
}

inline auto record_trie_delete(const std::vector<std::string>& existing,
                               const std::string& word) -> TrieRecording {
  TrieRecording rec;
  rec.title = "Trie — Delete \"" + word + "\"";

  auto ft = trie_detail::buildFromWords(existing);

  {
    auto s = trie_detail::snap(ft, word, -1);
    s.status_text = "Delete \"" + word + "\" from trie";
    s.trace_entry = "delete(\"" + word + "\")";
    s.current_line = 0;
    rec.steps.push_back(std::move(s));
  }

  int curr = 0;
  std::vector<int> path = {0};
  bool found_all = true;

  for (int d = 0; d < static_cast<int>(word.size()); ++d) {
    char ch = word[d];
    int next = -1;
    for (int child : ft.nodes[curr].children) {
      if (ft.nodes[child].edge_char == ch) {
        next = child;
        break;
      }
    }
    if (next < 0) {
      found_all = false;
      break;
    }
    curr = next;
    path.push_back(curr);
  }

  if (!found_all || !ft.nodes[curr].is_end) {
    trie_detail::setAllNormal(ft);
    auto s = trie_detail::snap(ft, word, -1);
    s.status_text = "\"" + word + "\" not found — nothing to delete";
    s.trace_entry = "not found";
    s.current_line = 1;
    rec.steps.push_back(std::move(s));
    return rec;
  }

  {
    trie_detail::setAllNormal(ft);
    ft.nodes[curr].state = TrieNodeState::kActive;
    auto s = trie_detail::snap(ft, word, static_cast<int>(word.size()));
    s.status_text = "Found \"" + word + "\" — clear end-of-word marker";
    s.trace_entry = "clear end-of-word";
    s.current_line = 2;
    rec.steps.push_back(std::move(s));
  }

  ft.nodes[curr].is_end = false;

  for (int i = static_cast<int>(path.size()) - 1; i > 0; --i) {
    int node = path[i];
    if (ft.nodes[node].is_end || !ft.nodes[node].children.empty())
      break;

    ft.nodes[node].state = TrieNodeState::kDeleted;

    {
      auto s = trie_detail::snap(ft, word, i - 1);
      s.status_text = "Prune node '" +
                      std::string(1, ft.nodes[node].edge_char) + "'";
      s.trace_entry = "prune '" +
                      std::string(1, ft.nodes[node].edge_char) + "'";
      s.current_line = 4;
      rec.steps.push_back(std::move(s));
    }

    int parent = path[i - 1];
    auto& children = ft.nodes[parent].children;
    children.erase(
        std::remove(children.begin(), children.end(), node), children.end());
  }

  {
    trie_detail::setAllNormal(ft);
    auto s = trie_detail::snap(ft, word, -1);
    s.status_text = "Delete complete";
    s.trace_entry = "done";
    s.current_line = 5;
    rec.steps.push_back(std::move(s));
  }

  return rec;
}

} // namespace viz
