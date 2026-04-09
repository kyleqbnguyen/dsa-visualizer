#pragma once

#include "code_panel.h"
#include "graph_snapshot.h"

namespace viz {

inline auto graph_bfs_code() -> CodePanel {
  return {
      .title = "graph_bfs.h",
      .lines =
          {
              "void bfs(graph, source) {",           // 0
              "    seen[source] = true;",             // 1
              "    queue.push(source);",              // 2
              "    while (!queue.empty()) {",         // 3
              "        curr = queue.pop();",          // 4
              "        visit(curr);",                 // 5
              "        for (next : adj[curr]) {",     // 6
              "            if (!seen[next]) {",       // 7
              "                seen[next] = true;",   // 8
              "                queue.push(next);",    // 9
              "            }",                        // 10
              "        }",                            // 11
              "    }",                                // 12
              "}",                                    // 13
          },
  };
}

inline auto graph_dfs_code() -> CodePanel {
  return {
      .title = "graph_dfs.h",
      .lines =
          {
              "void dfs(graph, curr, seen) {",       // 0
              "    if (seen[curr]) return;",          // 1
              "    seen[curr] = true;",               // 2
              "    visit(curr);",                     // 3
              "    for (next : adj[curr]) {",         // 4
              "        dfs(graph, next, seen);",      // 5
              "    }",                                // 6
              "}",                                    // 7
          },
  };
}

inline auto dijkstra_code() -> CodePanel {
  return {
      .title = "dijkstra.h",
      .lines =
          {
              "dist[source] = 0;",                          // 0
              "while (unvisited not empty) {",              // 1
              "    curr = node with min dist;",             // 2
              "    for (each neighbor n of curr) {",        // 3
              "        if (dist[curr]+w < dist[n]) {",      // 4
              "            dist[n] = dist[curr]+w;",        // 5
              "            prev[n] = curr;",                // 6
              "        }",                                  // 7
              "    }",                                      // 8
              "    mark curr as visited;",                  // 9
              "}",                                          // 10
          },
  };
}

inline auto graph_idle_code_panel() -> CodePanel {
  return {
      .title = "No operation selected",
      .lines =
          {
              "Press [C] to configure an operation",
          },
  };
}

inline auto get_graph_code_panel(GraphOp op) -> CodePanel {
  switch (op) {
  case GraphOp::kBfs:
    return graph_bfs_code();
  case GraphOp::kDfs:
    return graph_dfs_code();
  case GraphOp::kDijkstra:
    return dijkstra_code();
  default:
    return graph_idle_code_panel();
  }
}

} // namespace viz
