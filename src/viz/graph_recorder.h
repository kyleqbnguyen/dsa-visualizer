#pragma once

#include "graph_snapshot.h"

#include <climits>
#include <queue>
#include <string>
#include <vector>

namespace viz {

struct GraphDef {
  int num_nodes = 0;
  std::vector<std::string> labels;
  std::vector<std::pair<int, int>> positions;
  struct EdgeDef {
    int from, to, weight;
  };
  std::vector<EdgeDef> edges;
  bool directed = false;
};

namespace graph_detail {

inline auto makeSnap(const GraphDef& gd) -> GraphStepSnapshot {
  GraphStepSnapshot snap;
  for (int i = 0; i < gd.num_nodes; ++i) {
    GraphNodeSnap n;
    n.id = i;
    n.label = gd.labels[i];
    n.x = gd.positions[i].first;
    n.y = gd.positions[i].second;
    snap.nodes.push_back(n);
  }
  for (const auto& e : gd.edges) {
    GraphEdgeSnap es;
    es.from = e.from;
    es.to = e.to;
    es.weight = e.weight;
    es.directed = gd.directed;
    snap.edges.push_back(es);
  }
  return snap;
}

inline auto adj(const GraphDef& gd) -> std::vector<std::vector<std::pair<int, int>>> {
  std::vector<std::vector<std::pair<int, int>>> a(gd.num_nodes);
  for (const auto& e : gd.edges) {
    a[e.from].push_back({e.to, e.weight});
    if (!gd.directed)
      a[e.to].push_back({e.from, e.weight});
  }
  return a;
}

} // namespace graph_detail

inline auto record_graph_idle(const GraphDef& gd) -> GraphRecording {
  GraphRecording rec;
  rec.title = "Graph";
  rec.algorithm_name = "graph";
  rec.op = GraphOp::kNone;

  auto snap = graph_detail::makeSnap(gd);
  snap.status_text = "Press [C] to select an operation";
  rec.steps.push_back(std::move(snap));
  return rec;
}

inline auto record_graph_bfs(const GraphDef& gd, int source) -> GraphRecording {
  GraphRecording rec;
  rec.title = "Graph BFS";
  rec.algorithm_name = "graph";
  rec.op = GraphOp::kBfs;

  auto a = graph_detail::adj(gd);
  int n = gd.num_nodes;

  std::vector<bool> seen(n, false);
  std::queue<int> q;

  {
    auto snap = graph_detail::makeSnap(gd);
    snap.nodes[source].state = GraphNodeState::kSource;
    snap.status_text = "BFS from " + gd.labels[source];
    snap.trace_entry = "bfs(source=" + gd.labels[source] + ")";
    snap.current_line = 0;
    rec.steps.push_back(std::move(snap));
  }

  seen[source] = true;
  q.push(source);

  while (!q.empty()) {
    int curr = q.front();
    q.pop();

    auto snap = graph_detail::makeSnap(gd);
    for (int i = 0; i < n; ++i) {
      if (seen[i] && i != curr)
        snap.nodes[i].state = GraphNodeState::kVisited;
    }
    snap.nodes[curr].state = GraphNodeState::kActive;
    snap.nodes[source].state = (curr == source)
                                   ? GraphNodeState::kActive
                                   : GraphNodeState::kSource;

    std::vector<int> frontier_copy;
    {
      auto tmp = q;
      while (!tmp.empty()) {
        int f = tmp.front();
        tmp.pop();
        frontier_copy.push_back(f);
        snap.nodes[f].state = GraphNodeState::kFrontier;
      }
    }
    snap.frontier = frontier_copy;

    snap.status_text = "Visit " + gd.labels[curr];
    snap.trace_entry = "visit(" + gd.labels[curr] + ")";
    snap.current_line = 3;

    std::string q_str;
    for (int f : frontier_copy) {
      if (!q_str.empty())
        q_str += ", ";
      q_str += gd.labels[f];
    }
    snap.variables = {{"curr", gd.labels[curr]},
                      {"queue", "[" + q_str + "]"}};
    rec.steps.push_back(std::move(snap));

    for (const auto& [next, w] : a[curr]) {
      if (!seen[next]) {
        seen[next] = true;
        q.push(next);

        auto snap2 = graph_detail::makeSnap(gd);
        for (int i = 0; i < n; ++i) {
          if (seen[i])
            snap2.nodes[i].state = GraphNodeState::kVisited;
        }
        snap2.nodes[curr].state = GraphNodeState::kActive;
        snap2.nodes[next].state = GraphNodeState::kFrontier;
        snap2.nodes[source].state = GraphNodeState::kSource;

        for (auto& e : snap2.edges) {
          if ((e.from == curr && e.to == next) ||
              (!gd.directed && e.from == next && e.to == curr))
            e.state = GraphEdgeState::kTree;
        }

        snap2.status_text = "Enqueue " + gd.labels[next] +
                            " from " + gd.labels[curr];
        snap2.trace_entry = "enqueue " + gd.labels[next];
        snap2.current_line = 7;
        rec.steps.push_back(std::move(snap2));
      }
    }
  }

  {
    auto snap = graph_detail::makeSnap(gd);
    for (int i = 0; i < n; ++i) {
      if (seen[i])
        snap.nodes[i].state = GraphNodeState::kVisited;
      snap.nodes[source].state = GraphNodeState::kSource;
    }
    snap.status_text = "BFS complete";
    snap.trace_entry = "done";
    snap.current_line = 9;
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

inline auto record_graph_dfs(const GraphDef& gd, int source) -> GraphRecording {
  GraphRecording rec;
  rec.title = "Graph DFS";
  rec.algorithm_name = "graph";
  rec.op = GraphOp::kDfs;

  auto a = graph_detail::adj(gd);
  int n = gd.num_nodes;

  std::vector<bool> seen(n, false);

  {
    auto snap = graph_detail::makeSnap(gd);
    snap.nodes[source].state = GraphNodeState::kSource;
    snap.status_text = "DFS from " + gd.labels[source];
    snap.trace_entry = "dfs(source=" + gd.labels[source] + ")";
    snap.current_line = 0;
    rec.steps.push_back(std::move(snap));
  }

  struct Frame {
    int node;
    int edge_idx;
  };
  std::vector<Frame> stack;

  seen[source] = true;
  stack.push_back({source, 0});

  {
    auto snap = graph_detail::makeSnap(gd);
    snap.nodes[source].state = GraphNodeState::kActive;
    snap.status_text = "Visit " + gd.labels[source];
    snap.trace_entry = "visit(" + gd.labels[source] + ")";
    snap.current_line = 2;
    snap.variables = {{"curr", gd.labels[source]}};
    rec.steps.push_back(std::move(snap));
  }

  while (!stack.empty()) {
    auto& f = stack.back();
    if (f.edge_idx >= static_cast<int>(a[f.node].size())) {
      stack.pop_back();
      continue;
    }

    int next = a[f.node][f.edge_idx].first;
    int curr_node = f.node;
    ++f.edge_idx;

    if (!seen[next]) {
      seen[next] = true;

      auto snap = graph_detail::makeSnap(gd);
      for (int i = 0; i < n; ++i) {
        if (seen[i])
          snap.nodes[i].state = GraphNodeState::kVisited;
      }
      snap.nodes[next].state = GraphNodeState::kActive;
      snap.nodes[source].state = GraphNodeState::kSource;

      for (auto& e : snap.edges) {
        if ((e.from == curr_node && e.to == next) ||
            (!gd.directed && e.from == next && e.to == curr_node))
          e.state = GraphEdgeState::kTree;
      }

      std::vector<int> stack_ids;
      for (const auto& sf : stack)
        stack_ids.push_back(sf.node);
      stack_ids.push_back(next);
      snap.frontier = stack_ids;

      std::string stk_str;
      for (int id : stack_ids) {
        if (!stk_str.empty())
          stk_str += ", ";
        stk_str += gd.labels[id];
      }
      snap.status_text = "Visit " + gd.labels[next];
      snap.trace_entry = "visit(" + gd.labels[next] + ")";
      snap.current_line = 3;
      snap.variables = {{"curr", gd.labels[next]},
                        {"stack", "[" + stk_str + "]"}};
      rec.steps.push_back(std::move(snap));

      stack.push_back({next, 0});
    }
  }

  {
    auto snap = graph_detail::makeSnap(gd);
    for (int i = 0; i < n; ++i) {
      if (seen[i])
        snap.nodes[i].state = GraphNodeState::kVisited;
    }
    snap.nodes[source].state = GraphNodeState::kSource;
    snap.status_text = "DFS complete";
    snap.trace_entry = "done";
    snap.current_line = 5;
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

inline auto record_graph_dijkstra(const GraphDef& gd, int source)
    -> GraphRecording {
  GraphRecording rec;
  rec.title = "Dijkstra's Shortest Path";
  rec.algorithm_name = "graph";
  rec.op = GraphOp::kDijkstra;

  auto a = graph_detail::adj(gd);
  int n = gd.num_nodes;

  std::vector<int> dist(n, INT_MAX);
  std::vector<int> prev(n, -1);
  std::vector<bool> visited(n, false);

  dist[source] = 0;

  {
    auto snap = graph_detail::makeSnap(gd);
    snap.nodes[source].state = GraphNodeState::kSource;
    snap.nodes[source].distance = 0;
    snap.status_text = "Dijkstra from " + gd.labels[source];
    snap.trace_entry = "dijkstra(source=" + gd.labels[source] + ")";
    snap.current_line = 0;
    rec.steps.push_back(std::move(snap));
  }

  for (int iter = 0; iter < n; ++iter) {
    int u = -1;
    int best = INT_MAX;
    for (int i = 0; i < n; ++i) {
      if (!visited[i] && dist[i] < best) {
        best = dist[i];
        u = i;
      }
    }
    if (u < 0)
      break;

    visited[u] = true;

    {
      auto snap = graph_detail::makeSnap(gd);
      for (int i = 0; i < n; ++i) {
        snap.nodes[i].distance = dist[i];
        snap.nodes[i].prev = prev[i];
        if (visited[i])
          snap.nodes[i].state = GraphNodeState::kVisited;
      }
      snap.nodes[u].state = GraphNodeState::kActive;
      snap.nodes[source].state =
          (u == source) ? GraphNodeState::kActive : GraphNodeState::kSource;

      std::string dist_str = (dist[u] == INT_MAX) ? "inf" : std::to_string(dist[u]);
      snap.status_text = "Process " + gd.labels[u] +
                         " (dist=" + dist_str + ")";
      snap.trace_entry = "visit " + gd.labels[u] +
                         " dist=" + dist_str;
      snap.current_line = 1;

      std::vector<std::pair<std::string, std::string>> vars;
      for (int i = 0; i < n; ++i) {
        std::string d = (dist[i] == INT_MAX) ? "inf" : std::to_string(dist[i]);
        vars.push_back({"dist[" + gd.labels[i] + "]", d});
      }
      snap.variables = vars;
      rec.steps.push_back(std::move(snap));
    }

    for (const auto& [next, w] : a[u]) {
      if (visited[next])
        continue;
      int new_dist = dist[u] + w;
      if (new_dist < dist[next]) {
        dist[next] = new_dist;
        prev[next] = u;

        auto snap = graph_detail::makeSnap(gd);
        for (int i = 0; i < n; ++i) {
          snap.nodes[i].distance = dist[i];
          snap.nodes[i].prev = prev[i];
          if (visited[i])
            snap.nodes[i].state = GraphNodeState::kVisited;
        }
        snap.nodes[u].state = GraphNodeState::kActive;
        snap.nodes[next].state = GraphNodeState::kFrontier;
        snap.nodes[source].state = GraphNodeState::kSource;

        for (auto& e : snap.edges) {
          if ((e.from == u && e.to == next) ||
              (!gd.directed && e.from == next && e.to == u))
            e.state = GraphEdgeState::kActive;
        }

        snap.status_text = "Relax " + gd.labels[u] + " → " +
                           gd.labels[next] + " (new dist=" +
                           std::to_string(new_dist) + ")";
        snap.trace_entry = "relax " + gd.labels[next] + "=" +
                           std::to_string(new_dist);
        snap.current_line = 3;
        rec.steps.push_back(std::move(snap));
      }
    }
  }

  {
    auto snap = graph_detail::makeSnap(gd);
    for (int i = 0; i < n; ++i) {
      snap.nodes[i].distance = dist[i];
      snap.nodes[i].prev = prev[i];
      snap.nodes[i].state = visited[i] ? GraphNodeState::kVisited
                                        : GraphNodeState::kNormal;
    }
    snap.nodes[source].state = GraphNodeState::kSource;

    std::vector<std::pair<std::string, std::string>> vars;
    for (int i = 0; i < n; ++i) {
      std::string d = (dist[i] == INT_MAX) ? "inf" : std::to_string(dist[i]);
      std::string p = (prev[i] < 0) ? "-" : gd.labels[prev[i]];
      vars.push_back({gd.labels[i], "d=" + d + " prev=" + p});
    }
    snap.variables = vars;
    snap.status_text = "Dijkstra complete";
    snap.trace_entry = "done";
    snap.current_line = 6;
    rec.steps.push_back(std::move(snap));
  }

  return rec;
}

} // namespace viz
