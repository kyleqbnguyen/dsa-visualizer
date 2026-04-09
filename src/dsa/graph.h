#pragma once

#include <string>
#include <vector>

namespace dsa {

struct Edge {
  int to = 0;
  int weight = 1;
};

struct Graph {
  int num_nodes = 0;
  std::vector<std::vector<Edge>> adj;
  std::vector<std::string> labels;
  bool directed = false;

  void addEdge(int from, int to, int weight = 1) {
    adj[from].push_back({to, weight});
    if (!directed)
      adj[to].push_back({from, weight});
  }
};

inline auto graphBfs(const Graph& g, int source) -> std::vector<int> {
  // TODO: implement
  (void)g;
  (void)source;
  return {};
}

inline auto graphDfs(const Graph& g, int source) -> std::vector<int> {
  // TODO: implement
  (void)g;
  (void)source;
  return {};
}

struct DijkstraResult {
  std::vector<int> dist;
  std::vector<int> prev;
};

inline auto dijkstra(const Graph& g, int source) -> DijkstraResult {
  // TODO: implement
  (void)g;
  (void)source;
  return {};
}

} // namespace dsa
