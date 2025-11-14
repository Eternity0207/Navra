#include "../include/algorithms.h"
#include "../include/graph.h"
#include <queue>
#include <vector>
#include <limits>
#include <algorithm>
using namespace std;
vector<double> dijkstra(const Graph& g,int start) {
    int maxId=g.maxNodeId();
    int n=maxId+1;
    if (n<=0) return vector<double>();
    vector<double> dist(n,numeric_limits<double>::infinity());
    typedef pair<double,int> P;
    priority_queue<P,vector<P>,greater<P>> pq;
    if (!g.isValidAttraction(start)) return dist;
    dist[start]=0.0;
    pq.push(P(0.0,start));
    while (!pq.empty()) {
        P top=pq.top(); pq.pop();
        double d=top.first;
        int u=top.second;
        if (d>dist[u]) continue;
        auto nbrs=g.getNeighbors(u);
        for (size_t i=0; i<nbrs.size(); ++i) {
            int v=nbrs[i].first;
            double w=nbrs[i].second;
            if (v<0 || v>=(int)dist.size()) continue;
            if (dist[v]>d+w) {
                dist[v]=d+w;
                pq.push(P(dist[v],v));
            }
        }
    }
    return dist;
}
pair<vector<double>,vector<int>> dijkstraWithPath(const Graph& g,int start) {
    int maxId=g.maxNodeId();
    int n=maxId+1;
    if (n<=0) return {vector<double>(),vector<int>()};
    vector<double> dist(n,numeric_limits<double>::infinity());
    vector<int> parent(n,-1);
    typedef pair<double,int> P;
    priority_queue<P,vector<P>,greater<P>> pq;
    if (!g.isValidAttraction(start)) return {dist,parent};
    dist[start]=0.0;
    pq.push(P(0.0,start));
    while (!pq.empty()) {
        P top=pq.top(); pq.pop();
        double d=top.first;
        int u=top.second;
        if (d>dist[u]) continue;
        auto nbrs=g.getNeighbors(u);
        for (size_t i=0; i<nbrs.size(); ++i) {
            int v=nbrs[i].first;
            double w=nbrs[i].second;
            if (v<0 || v>=(int)dist.size()) continue;
            if (dist[v]>d+w) {
                dist[v]=d+w;
                parent[v]=u;
                pq.push(P(dist[v],v));
            }
        }
    }
    return {dist,parent};
}
vector<int> reconstructPath(const vector<int>& parent,int start,int end) {
    vector<int> path;
    if (end<0 || end>=(int)parent.size()) return path;
    int cur=end;
    while (cur!=-1) {
        path.push_back(cur);
        if (cur==start) break;
        cur=parent[cur];
    }
    reverse(path.begin(),path.end());
    if (!path.empty() && path.front()==start) return path;
    return vector<int>();
}