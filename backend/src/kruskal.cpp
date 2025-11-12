#include "../include/algorithms.h"
#include "../include/dsu.h"
#include <algorithm>
bool Edge::operator<(const Edge& other) const {
    return weight<other.weight;
}
std::vector<Edge> kruskalMST(std::vector<Edge>& edges,int n) {
    std::sort(edges.begin(),edges.end());
    DSU dsu(n);
    std::vector<Edge> mst;
    for (size_t i=0; i<edges.size(); i++) {
        if (!dsu.connected(edges[i].u,edges[i].v)) {
            dsu.unite(edges[i].u,edges[i].v); mst.push_back(edges[i]);
        }
    }
    return mst;
}
void dfsPreorder(int node,const std::vector<std::vector<int>>& adj,std::vector<bool>& visited,std::vector<int>& tour) {
    visited[node]=true;
    tour.push_back(node);
    for (size_t i=0; i<adj[node].size(); i++) {
        if (!visited[adj[node][i]]) {
            dfsPreorder(adj[node][i],adj,visited,tour);
        }
    }
}
std::vector<int> mstToTour(const std::vector<Edge>& mst,int n,int start) {
    std::vector<std::vector<int>> adj(n);
    for (size_t i=0; i<mst.size(); i++) {
        adj[mst[i].u].push_back(mst[i].v); adj[mst[i].v].push_back(mst[i].u);
    }
    std::vector<bool> visited(n,false); std::vector<int> tour;
    dfsPreorder(start,adj,visited,tour);
    return tour;
}