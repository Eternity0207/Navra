#include "../include/route_optimizer.h"
#include "../include/algorithms.h"
#include <algorithm>
#include <unordered_set>
#include <limits>
using namespace std;
extern vector<Edge> kruskalMST(vector<Edge>& edges,int n);
extern vector<int> mstToTour(const vector<Edge>& mst,int n,int start);
extern vector<int> aStarPath(const Graph& g,int start,int goal);
extern pair<vector<double>,vector<int>> dijkstraWithPath(const Graph& g,int start);
RouteResult RouteOptimizer::computeFullGraphRoute() {
    RouteResult res;
    res.algorithm="Kruskal+DFS+A*";
    vector<int> nodes=graph.getAllAttractionIds();//attractions(i.e the locations{per PS.})
    if (nodes.empty()){ res.algorithm+=" (Empty)"; return res; }
    DSU* dsu=graph.getDSU();
    if (!dsu) { res.algorithm+=" (No DSU)"; return res; }
    int root=dsu->find(nodes[0]);
    for (int id:nodes) if (dsu->find(id)!=root) {res.algorithm+=" (Graph not connected)"; return res; }
    // Build MST over the whole graph (Kruskal).
    vector<Edge> edges=graph.getAllEdges();
    int maxId=graph.maxNodeId();
    vector<Edge> mst=kruskalMST(edges,maxId+1);


    // Pick a deterministic start node (smallest id).
    int startNode=*min_element(nodes.begin(),nodes.end());
    vector<int> tour=mstToTour(mst,maxId+1,startNode);
    unordered_set<int> seen;
    vector<int> finalOrder;
    for (int id:tour) {
        if (graph.isValidAttraction(id) && !seen.count(id)) { 
            finalOrder.push_back(id); seen.insert(id);}
    }
    double total=0;
    for (size_t i=0; i+1<finalOrder.size(); ++i) {
        int u=finalOrder[i],v=finalOrder[i+1];
        vector<int> path=aStarPath(graph,u,v);
        if (path.empty()) {
            double seg=dijkstraWithPath(graph,u).first[v];
            if (seg==numeric_limits<double>::infinity()) res.algorithm+=" (Unreachable)"; else total+=seg;
        } else {
            for (size_t k=0;k+1< path.size();++k) {
                double seg=dijkstraWithPath(graph,path[k]).first[path[k+1]];
                if (seg==numeric_limits<double>::infinity()) res.algorithm+=" (Unreachable)"; else total+=seg;
            }
        }
    }
    res.attractionIds=finalOrder;
    res.totalTime=total;
    return res;
}
// Handles empty, single, fixed-order, and flexible-order (TSP-like) cases.
RouteResult RouteOptimizer::computeOptimalRoute(const vector<int>& locations,bool flexibleOrder) {
    RouteResult rr;
    if (locations.empty()) { rr.algorithm="None"; return rr; }
    if (locations.size()==1) { rr.attractionIds=locations; rr.totalTime=0; rr.algorithm="Single"; return rr; }
    if (!flexibleOrder) {//respect provided order
        auto p=computeOrderedRoute(graph,locations);
        rr.totalTime=p.first;
        rr.attractionIds=p.second;
        rr.algorithm="Fixed Order";
        return rr;

    }
    auto freeRes=computeOptimalRouteFree(graph,locations);
    rr.totalTime=freeRes.first;
    for (int idx:freeRes.second) rr.attractionIds.push_back(locations[idx]);
    rr.algorithm="Flexible TSP";
    return rr;
}