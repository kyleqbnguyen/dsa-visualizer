#pragma once

#include <climits>
#include <string>
#include <vector>

namespace viz {

enum class GraphOp {
  kNone,
  kBfs,
  kDfs,
  kDijkstra,
};

enum class GraphNodeState { kNormal, kFrontier, kActive, kVisited, kPath, kSource };
enum class GraphEdgeState { kNormal, kActive, kTree, kPath };

struct GraphNodeSnap {
  int id = 0;
  std::string label;
  int x = 0;
  int y = 0;
  GraphNodeState state = GraphNodeState::kNormal;
  int distance = INT_MAX;
  int prev = -1;
};

struct GraphEdgeSnap {
  int from = 0;
  int to = 0;
  int weight = 1;
  GraphEdgeState state = GraphEdgeState::kNormal;
  bool directed = false;
};

struct GraphStepSnapshot {
  std::vector<GraphNodeSnap> nodes;
  std::vector<GraphEdgeSnap> edges;
  std::vector<int> frontier;

  std::string status_text;
  std::string trace_entry;
  int current_line = -1;
  std::vector<std::pair<std::string, std::string>> variables;
};

struct GraphRecording {
  std::string title;
  std::string algorithm_name;
  GraphOp op = GraphOp::kNone;
  std::vector<GraphStepSnapshot> steps;
};

} // namespace viz
