#pragma once

#include "lru_snapshot.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

namespace viz {

namespace lru_detail {

struct LruState {
  struct Entry {
    int key;
    int value;
  };
  std::vector<Entry> list;
  int capacity;

  auto findKey(int key) const -> int {
    for (int i = 0; i < static_cast<int>(list.size()); ++i) {
      if (list[i].key == key)
        return i;
    }
    return -1;
  }

  auto snap() const -> LruStepSnapshot {
    LruStepSnapshot s;
    s.capacity = capacity;

    for (int i = 0; i < static_cast<int>(list.size()); ++i) {
      LruNodeSnap n;
      n.key = list[i].key;
      n.value = list[i].value;
      n.has_prev = (i > 0);
      n.has_next = (i < static_cast<int>(list.size()) - 1);
      s.list.push_back(n);
    }

    for (int i = 0; i < static_cast<int>(list.size()); ++i) {
      LruMapSlot slot;
      slot.key = list[i].key;
      slot.list_index = i;
      slot.occupied = true;
      s.map.push_back(slot);
    }

    return s;
  }
};

} // namespace lru_detail

inline auto record_lru_idle(
    const std::vector<std::pair<int, int>>& initial_entries, int capacity)
    -> LruRecording {
  LruRecording rec;
  rec.title = "LRU Cache";

  lru_detail::LruState state;
  state.capacity = capacity;
  for (const auto& [k, v] : initial_entries)
    state.list.push_back({k, v});

  auto s = state.snap();
  s.status_text = "Press [C] to select an operation";
  s.variables = {{"capacity", std::to_string(capacity)},
                 {"size", std::to_string(state.list.size())}};
  rec.steps.push_back(std::move(s));
  return rec;
}

inline auto record_lru_get(
    const std::vector<std::pair<int, int>>& initial_entries, int capacity,
    int key) -> LruRecording {
  LruRecording rec;
  rec.title = "LRU Cache — get(" + std::to_string(key) + ")";

  lru_detail::LruState state;
  state.capacity = capacity;
  for (const auto& [k, v] : initial_entries)
    state.list.push_back({k, v});

  {
    auto s = state.snap();
    s.status_text = "get(key=" + std::to_string(key) + ")";
    s.trace_entry = "get(" + std::to_string(key) + ")";
    s.current_line = 0;
    s.variables = {{"key", std::to_string(key)},
                   {"capacity", std::to_string(capacity)},
                   {"size", std::to_string(state.list.size())}};
    rec.steps.push_back(std::move(s));
  }

  int idx = state.findKey(key);
  if (idx < 0) {
    auto s = state.snap();
    s.status_text = "Key " + std::to_string(key) + " not found — cache miss";
    s.trace_entry = "miss: key " + std::to_string(key);
    s.current_line = 1;
    s.variables = {{"key", std::to_string(key)}, {"result", "miss"}};
    rec.steps.push_back(std::move(s));
    return rec;
  }

  {
    auto s = state.snap();
    s.list[idx].state = LruNodeState::kActive;
    for (auto& slot : s.map) {
      if (slot.key == key)
        slot.active = true;
    }
    s.status_text = "Found key " + std::to_string(key) + " at position " +
                    std::to_string(idx);
    s.trace_entry = "found at position " + std::to_string(idx);
    s.current_line = 2;
    s.variables = {{"key", std::to_string(key)},
                   {"value", std::to_string(state.list[idx].value)}};
    rec.steps.push_back(std::move(s));
  }

  auto entry = state.list[idx];
  state.list.erase(state.list.begin() + idx);
  state.list.insert(state.list.begin(), entry);

  {
    auto s = state.snap();
    s.list[0].state = LruNodeState::kActive;
    s.status_text = "Moved key " + std::to_string(key) +
                    " to MRU (head) — value=" +
                    std::to_string(entry.value);
    s.trace_entry = "move to head → value=" + std::to_string(entry.value);
    s.current_line = 3;
    s.variables = {{"key", std::to_string(key)},
                   {"value", std::to_string(entry.value)},
                   {"result", "hit"}};
    rec.steps.push_back(std::move(s));
  }

  return rec;
}

inline auto record_lru_put(
    const std::vector<std::pair<int, int>>& initial_entries, int capacity,
    int key, int value) -> LruRecording {
  LruRecording rec;
  rec.title = "LRU Cache — put(" + std::to_string(key) + ", " +
              std::to_string(value) + ")";

  lru_detail::LruState state;
  state.capacity = capacity;
  for (const auto& [k, v] : initial_entries)
    state.list.push_back({k, v});

  {
    auto s = state.snap();
    s.status_text = "put(key=" + std::to_string(key) +
                    ", value=" + std::to_string(value) + ")";
    s.trace_entry = "put(" + std::to_string(key) + ", " +
                    std::to_string(value) + ")";
    s.current_line = 0;
    s.variables = {{"key", std::to_string(key)},
                   {"value", std::to_string(value)},
                   {"capacity", std::to_string(capacity)},
                   {"size", std::to_string(state.list.size())}};
    rec.steps.push_back(std::move(s));
  }

  int idx = state.findKey(key);
  if (idx >= 0) {
    {
      auto s = state.snap();
      s.list[idx].state = LruNodeState::kActive;
      s.status_text = "Key " + std::to_string(key) +
                      " exists — update value and move to head";
      s.trace_entry = "key exists at " + std::to_string(idx);
      s.current_line = 1;
      rec.steps.push_back(std::move(s));
    }

    state.list[idx].value = value;
    auto entry = state.list[idx];
    state.list.erase(state.list.begin() + idx);
    state.list.insert(state.list.begin(), entry);

    {
      auto s = state.snap();
      s.list[0].state = LruNodeState::kActive;
      s.status_text = "Updated and moved to head";
      s.trace_entry = "updated value=" + std::to_string(value);
      s.current_line = 2;
      rec.steps.push_back(std::move(s));
    }
    return rec;
  }

  if (static_cast<int>(state.list.size()) >= capacity) {
    int evict_idx = static_cast<int>(state.list.size()) - 1;
    int evict_key = state.list[evict_idx].key;

    {
      auto s = state.snap();
      s.list[evict_idx].state = LruNodeState::kEvicted;
      s.status_text = "Cache full — evict LRU (key=" +
                      std::to_string(evict_key) + ")";
      s.trace_entry = "evict key=" + std::to_string(evict_key);
      s.current_line = 3;
      rec.steps.push_back(std::move(s));
    }

    state.list.pop_back();
  }

  state.list.insert(state.list.begin(), {key, value});

  {
    auto s = state.snap();
    s.list[0].state = LruNodeState::kNew;
    s.status_text = "Inserted key=" + std::to_string(key) +
                    " value=" + std::to_string(value) + " at head";
    s.trace_entry = "insert at head";
    s.current_line = 5;
    s.variables = {{"key", std::to_string(key)},
                   {"value", std::to_string(value)},
                   {"size", std::to_string(state.list.size())}};
    rec.steps.push_back(std::move(s));
  }

  return rec;
}

} // namespace viz
