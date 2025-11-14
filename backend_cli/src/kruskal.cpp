#include "../include/algorithms.h"
#include "../include/dsu.h"
#include <algorithm>
#include <vector>
using namespace std;
vector<Edge> kruskalMST(vector<Edge>& edges,int n) {
    sort(edges.begin(),edges.end(),[](const Edge& a,const Edge& b)
    { return a.weight<b.weight; });
    DSU dsu(n);
    vector<Edge> mst;
    // Build the MST by picking the smallest edges that don't form a cycle
    //(unite,union find for efficient(stop cycles))
    for (size_t i=0; i<edges.size(); ++i) {
        if (edges[i].u<0 || edges[i].v<0) continue;
        if (edges[i].u>=n || edges[i].v>=n) continue;
        if (dsu.unite(edges[i].u,edges[i].v)) {
            mst.push_back(edges[i]);
            if ((int)mst.size()==n-1) break;
            }
        }
    return mst;
}
void dfsPreorder(int node,const vector<vector<int>>& adj,vector<bool>& visited,vector<int>& tour) {
    visited[node]=true;
    // Visit the node first, 
    //then recursively explore its neighbors (DFS preorder)
    tour.push_back(node);
    for (size_t i=0; i<adj[node].size(); ++i) {
        int v=adj[node][i];
        if (v<0 || v>=(int)adj.size()) continue;
        if (!visited[v]) dfsPreorder(v,adj,visited,tour);
        }
}
vector<int> mstToTour(const vector<Edge>& mst,int n,int start) {
    vector<vector<int>> adj(n);
    for (size_t i=0; i<mst.size(); ++i) {
        if (mst[i].u>=0 && mst[i].u<n && mst[i].v>=0 && mst[i].v<n) {
            adj[mst[i].u].push_back(mst[i].v);
            adj[mst[i].v].push_back(mst[i].u);
        }
    }
    vector<bool> visited(n,false);
    vector<int> tour;
    if (start<0 || start>=n) start=0;
    dfsPreorder(start,adj,visited,tour);
    return tour;
}
