#pragma once

#include <string>
#include <vector>

namespace viz {

enum class RingBufferOp {
  kNone,
  kEnqueue,
  kDequeue,
};

enum class RingSlotState {
  kEmpty,
  kOccupied,
  kActive,
  kNew,
  kRemoved,
  kDone,
};

struct RingSlot {
  int value = 0;
  RingSlotState state = RingSlotState::kEmpty;
};

struct RingBufferStepSnapshot {
  std::vector<RingSlot> slots;
  int head = 0;
  int tail = 0;
  int size = 0;

  std::string status_text;
  std::string trace_entry;
  int current_line = -1;
  std::vector<std::pair<std::string, std::string>> variables;
};

struct RingBufferRecording {
  std::string title;
  std::string algorithm_name = "ring_buffer";
  std::vector<RingBufferStepSnapshot> steps;
};

} // namespace viz
