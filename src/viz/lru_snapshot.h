#pragma once

#include <string>
#include <vector>

namespace viz {

enum class LruOp {
  kNone,
  kGet,
  kPut,
};

enum class LruNodeState { kNormal, kActive, kNew, kEvicted };

struct LruNodeSnap {
  int key = 0;
  int value = 0;
  LruNodeState state = LruNodeState::kNormal;
  bool has_next = true;
  bool has_prev = true;
};

struct LruMapSlot {
  int key = 0;
  int list_index = -1;
  bool occupied = false;
  bool active = false;
};

struct LruStepSnapshot {
  std::vector<LruNodeSnap> list;
  std::vector<LruMapSlot> map;
  int capacity = 0;

  std::string status_text;
  std::string trace_entry;
  int current_line = -1;
  std::vector<std::pair<std::string, std::string>> variables;
};

struct LruRecording {
  std::string title;
  std::string algorithm_name = "lru_cache";
  std::vector<LruStepSnapshot> steps;
};

} // namespace viz
