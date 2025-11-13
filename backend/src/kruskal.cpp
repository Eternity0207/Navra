#include "../include/algorithms.h"
#include "../include/dsu.h"
#include <algorithm>
using namespace std;
bool Edge::operator<(const Edge& other) const {
    return weight < other.weight;
}

vector<Edge> kruskalMST(vector<Edge>& edges,int n) {
    sort(edges.begin(), edges.end());
    DSU dsu(n);
    vector<Edge> mst;

    for (size_t i = 0;i <edges.size(); i++) {
        if (dsu.unite(edges[i].u,edges[i].v)) {//the unite function of DSU

            mst.push_back(edges[i]);//only pushes a edge after DSu efficiently checks cycle
            if((int)mst.size()==n-1) break;
        }
    }
    return mst;
}

void dfsPreorder(int node,const vector<std::vector<int>>& adj,vector<bool>& visited,vector<int>& tour) {
                 
    visited[node] = true;
    tour.push_back(node);
    for (size_t i = 0; i < adj[node].size();i++) {
        if (!visited[adj[node][i]]) {
     dfsPreorder(adj[node][i],adj,visited,tour);
        }
    }
}

vector<int> mstToTour(const vector<Edge>& mst,int n,int start) {
vector<vector<int>> adj(n);
    for (size_t i = 0;i < mst.size();i++) {
        adj[mst[i].u].push_back(mst[i].v);
        adj[mst[i].v].push_back(mst[i].u);
    }

vector<bool> visited(n, false);
vector<int> tour;
    dfsPreorder(start,adj,visited,tour);
    return tour;
}
