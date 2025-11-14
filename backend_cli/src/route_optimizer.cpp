#include "../include/route_optimizer.h"
#include "../include/algorithms.h"
#include <algorithm>
#include <unordered_set>
#include <limits>

using namespace std;

extern vector<Edge> kruskalMST(vector<Edge>& edges, int n);
extern vector<int> mstToTour(const vector<Edge>& mst, int n, int start);
extern vector<int> aStarPath(const Graph& g, int start, int goal);
extern pair<vector<double>, vector<int>> dijkstraWithPath(const Graph& g, int start);

// ---------------------------------------------------------
// Helper: append a reconstructed segment to fullPath
// ---------------------------------------------------------
static void appendSegment(vector<int>& fullPath, const vector<int>& segment) {
    if (segment.empty()) return;

    if (!fullPath.empty()) {
        // avoid repeating the first node of segment
        fullPath.insert(fullPath.end(), segment.begin() + 1, segment.end());
    } else {
        fullPath.insert(fullPath.end(), segment.begin(), segment.end());
    }
}

// ---------------------------------------------------------
// FULL GRAPH TRAVERSAL (MST + DFS + A*)
// ---------------------------------------------------------
RouteResult RouteOptimizer::computeFullGraphRoute() {
    RouteResult res;
    res.algorithm = "Kruskal + DFS + A*";

    vector<int> nodes = graph.getAllAttractionIds();
    if (nodes.empty()) return res;

    DSU* dsu = graph.getDSU();
    if (!dsu) return res;

    int root = dsu->find(nodes[0]);
    for (int id : nodes) {
        if (dsu->find(id) != root) {
            res.algorithm += " (Graph Not Connected)";
            return res;
        }
    }

    vector<Edge> edges = graph.getAllEdges();
    int maxId = graph.maxNodeId();
    vector<Edge> mst = kruskalMST(edges, maxId + 1);

    int startNode = *min_element(nodes.begin(), nodes.end());
    vector<int> traversal = mstToTour(mst, maxId + 1, startNode);

    vector<int> finalOrder;
    unordered_set<int> vis;
    for (int id : traversal) {
        if (graph.isValidAttraction(id) && !vis.count(id)) {
            finalOrder.push_back(id);
            vis.insert(id);
        }
    }

    res.attractionIds = finalOrder;

    double total = 0;

    for (size_t i = 0; i + 1 < finalOrder.size(); ++i) {
        int u = finalOrder[i];
        int v = finalOrder[i + 1];

        vector<int> path = aStarPath(graph, u, v);

        if (path.empty()) {
            // fallback Dijkstra
            auto dres = dijkstraWithPath(graph, u);
            if (dres.first[v] == numeric_limits<double>::infinity()) {
                res.algorithm += " (Unreachable Segment)";
                continue;
            }
            path = reconstructPath(dres.second, u, v);
        }

        appendSegment(res.fullPath, path);

        // accumulate time
        for (size_t k = 0; k + 1 < path.size(); ++k) {
            auto dres2 = dijkstraWithPath(graph, path[k]);
            total += dres2.first[path[k + 1]];
        }
    }

    res.totalTime = total;
    return res;
}

// ---------------------------------------------------------
// FIXED ORDER (Dijkstra) + FLEXIBLE ORDER (TSP)
// ---------------------------------------------------------
RouteResult RouteOptimizer::computeOptimalRoute(const vector<int>& locs, bool flexible) {
    RouteResult rr;

    if (locs.empty()) return rr;
    if (locs.size() == 1) {
        rr.attractionIds = locs;
        rr.fullPath = locs;
        rr.totalTime = 0;
        rr.algorithm = "Single";
        return rr;
    }

    // --------------------
    // FIXED ORDER
    // --------------------
    if (!flexible) {
        rr.algorithm = "Fixed Order";
        rr.attractionIds = locs;

        double total = 0;

        for (size_t i = 0; i + 1 < locs.size(); ++i) {
            int u = locs[i];
            int v = locs[i + 1];

            auto dres = dijkstraWithPath(graph, u);
            if (dres.first[v] == numeric_limits<double>::infinity()) {
                total += 1e9;
                continue;
            }

            vector<int> segment = reconstructPath(dres.second, u, v);
            appendSegment(rr.fullPath, segment);
            total += dres.first[v];
        }

        rr.totalTime = total;
        return rr;
    }

    // --------------------
    // FLEXIBLE ORDER (TSP)
    // --------------------
    rr.algorithm = "Flexible TSP";

    auto tspRes = computeOptimalRouteFree(graph, locs);
    rr.totalTime = tspRes.first;

    for (int idx : tspRes.second)
        rr.attractionIds.push_back(locs[idx]);

    // Build full expanded path
    for (size_t i = 0; i + 1 < rr.attractionIds.size(); ++i) {
        int u = rr.attractionIds[i];
        int v = rr.attractionIds[i + 1];

        auto dres = dijkstraWithPath(graph, u);
        vector<int> segment = reconstructPath(dres.second, u, v);
        appendSegment(rr.fullPath, segment);
    }

    return rr;
}
