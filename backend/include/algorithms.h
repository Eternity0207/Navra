#ifndef ALGORITHMS_H
#define ALGORITHMS_H

// forward declare Graph to avoid circular include with graph.h
class Graph;

#include <vector>
#include <utility>

// Dijkstra
std::vector<double> dijkstra(const Graph& g, int start);
std::pair<std::vector<double>, std::vector<int>> dijkstraWithPath(const Graph& g, int start);
std::vector<int> reconstructPath(const std::vector<int>& parent, int start, int end);

// A*
std::vector<int> aStarPath(const Graph& g, int start, int goal);
double haversine(double lat1, double lon1, double lat2, double lon2);

// TSP
std::pair<double, std::vector<int>> tspDP(const std::vector<std::vector<double>>& dist);
std::pair<double, std::vector<int>> tspMSTApproximation(const Graph& g, const std::vector<int>& locs);
std::pair<double, std::vector<int>> greedyTSP(const Graph& g, int start, const std::vector<int>& locs);
void twoOptImprovement(std::vector<int>& tour, const std::vector<std::vector<double>>& dist);
std::pair<double, std::vector<int>> computeOptimalRouteFree(const Graph& g, const std::vector<int>& locs);

// Kruskal & MST
struct Edge {
    int u, v;
    double weight;
    bool operator<(const Edge& other) const { return weight < other.weight; }
};
std::vector<Edge> kruskalMST(std::vector<Edge>& edges, int n);
std::vector<int> mstToTour(const std::vector<Edge>& mst, int n, int start);

// Ordered route helper (fixed-order)
std::pair<double, std::vector<int>> computeOrderedRoute(const Graph& g, const std::vector<int>& order);

#endif // ALGORITHMS_H
