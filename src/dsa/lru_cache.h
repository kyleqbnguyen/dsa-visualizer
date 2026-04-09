#pragma once

#include <list>
#include <optional>
#include <unordered_map>

namespace dsa {

class LruCache {
public:
  explicit LruCache(int capacity) : capacity_(capacity) {}

  auto get(int key) -> std::optional<int> {
    // TODO: implement
    (void)key;
    return std::nullopt;
  }

  void put(int key, int value) {
    // TODO: implement
    (void)key;
    (void)value;
  }

  auto size() const -> int { return static_cast<int>(list_.size()); }
  auto capacity() const -> int { return capacity_; }

private:
  int capacity_;
  std::list<std::pair<int, int>> list_;
  std::unordered_map<int, std::list<std::pair<int, int>>::iterator> map_;
};

} // namespace dsa
