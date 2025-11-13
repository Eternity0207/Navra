#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "graph.h"
#include <vector>
#include <utility>
#include <unordered_set>

std::vector<double> dijkstra(const Graph& g,int start);
std::pair<std::vector<double>,std::vector<int>>dijkstraWithPath(const Graph& g,int start);
std::vector<int> reconstructPath(const std::vector<int>& parent,int start,int end);

std::vector<int> aStarPath(const Graph& g,int start,int goal);
double haversine(double lat1,double lon1,double lat2,double lon2);

std::pair<double,std::vector<int>>tspDP(const std::vector<std::vector<double>>& dist);
std::pair<double,std::vector<int>>greedyTSP(const Graph& g,int start,const std::vector<int>& mustVisit);
void twoOptImprovement(std::vector<int>& tour,const std::vector<std::vector<double>>& dist);

struct Edge{
    int u,v;
    double weight;
    bool operator<(const Edge& other) const;
};

std::vector<Edge> kruskalMST(std::vector<Edge>& edges, int n);
std::vector<int> mstToTour(const std::vector<Edge>& mst, int n, int start);

// Route computation for ordered list
std::pair<double, std::vector<int>> computeOrderedRoute(const Graph& g, const std::vector<int>& orderedList);

// Smart algorithm selector
std::pair<double, std::vector<int>> computeOptimalRoute(const Graph& g, const std::vector<int>& locations, bool flexibleOrder);

#endif


