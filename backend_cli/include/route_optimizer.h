#ifndef ROUTE_OPTIMIZER_H
#define ROUTE_OPTIMIZER_H

#include "graph.h"
#include <vector>
#include <string>

struct RouteResult {
    std::vector<int> attractionIds;   // only selected stops in order
    std::vector<int> fullPath;        // FULL actual path including intermediate nodes
    double totalTime = 0.0;
    std::string algorithm;
};

class RouteOptimizer {
private:
    Graph graph;

public:
    RouteOptimizer() = default;
    void setGraph(const Graph& g) { graph = g; }

    RouteResult computeOptimalRoute(const std::vector<int>& locations, bool flexibleOrder);
    RouteResult computeFullGraphRoute();
};

#endif // ROUTE_OPTIMIZER_H
