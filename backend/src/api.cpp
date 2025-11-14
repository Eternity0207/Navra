#include "api.h"
#include "algorithms.h"

ApiResult runOptimizerAPI(
    int mode, 
    const std::vector<std::string>& locations,
    Graph& graph
) {
    ApiResult result;
    result.success = false;
    result.totalTime = 0.0;
    result.stopCount = 0;

    // Convert location names to IDs
    std::vector<int> ids;
    for (const auto& name : locations) {
        int id = graph.getIdByName(name);
        ids.push_back(id);
    }

    if (ids.empty()) {
        result.errorMessage = "No locations selected";
        return result;
    }

    // Check for invalid IDs (-1 means location not found)
    bool hasInvalid = false;
    std::vector<std::string> invalidNames;
    
    for (size_t i = 0; i < ids.size(); ++i) {
        if (ids[i] == -1) {
            hasInvalid = true;
            invalidNames.push_back(locations[i]);
        }
    }

    if (hasInvalid) {
        result.errorMessage = "One or more location names do not exist: ";
        for (size_t i = 0; i < invalidNames.size(); ++i) {
            result.errorMessage += invalidNames[i];
            if (i < invalidNames.size() - 1) result.errorMessage += ", ";
        }
        return result;
    }

    // DSU connectivity check
    DSU* dsu = graph.getDSU();
    if (dsu != nullptr) {
        int root = dsu->find(ids[0]);
        bool allConnected = true;

        for (int id : ids) {
            if (dsu->find(id) != root) {
                allConnected = false;
                break;
            }
        }

        if (!allConnected) {
            result.errorMessage = "Selected locations are not reachable from each other (DSU connectivity check failed)";
            return result;
        }
    }

    // Compute optimal route
    bool flexible = (mode == 1);
    RouteOptimizer optimizer;
    optimizer.setGraph(graph);

    RouteResult r = optimizer.computeOptimalRoute(ids, flexible);

    // Build result
    result.success = true;
    result.algorithm = r.algorithm;
    result.totalTime = r.totalTime;
    result.routeIds = r.attractionIds;
    result.stopCount = r.attractionIds.size();

    for (int id : r.attractionIds) {
        result.routeNames.push_back(graph.getAttraction(id).name);
    }

    return result;
}

ApiResult runFullGraphTraversal(Graph& graph) {
    ApiResult result;
    result.success = false;
    result.totalTime = 0.0;
    result.stopCount = 0;

    RouteOptimizer optimizer;
    optimizer.setGraph(graph);

    RouteResult r = optimizer.computeFullGraphRoute();

    if (r.attractionIds.empty()) {
        result.errorMessage = "Campus graph is not fully connected. Full traversal (Kruskal + DFS + A*) cannot be performed";
        result.algorithm = "Kruskal (MST) + DFS Traversal + A* Path Refinement";
        return result;
    }

    result.success = true;
    result.algorithm = r.algorithm;
    result.totalTime = r.totalTime;
    result.routeIds = r.attractionIds;
    result.stopCount = r.attractionIds.size();

    for (int id : r.attractionIds) {
        result.routeNames.push_back(graph.getAttraction(id).name);
    }

    return result;
}
