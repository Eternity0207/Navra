#pragma once
#include <string>
#include <vector>
#include "graph.h"
#include "route_optimizer.h"

struct ApiResult {
    bool success;
    std::string algorithm;
    double totalTime;
    std::vector<std::string> routeNames;
    std::vector<int> routeIds;
    int stopCount;
    std::string errorMessage;
};

// For choices 1 & 2 (TSP or Dijkstra)
ApiResult runOptimizerAPI(
    int mode, 
    const std::vector<std::string>& locations,
    Graph& graph
);

// For choice 3 (Full campus traversal)
ApiResult runFullGraphTraversal(Graph& graph);
