#include "../include/algorithms.h"
#include "../include/graph.h"
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <vector>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
using namespace std;
double haversine(double lat1,double lon1,double lat2,double lon2) {
    const double R=6371.0; // km
    double dLat=(lat2-lat1)*M_PI/180.0;
    double dLon=(lon2-lon1)*M_PI/180.0;
    lat1=lat1*M_PI/180.0;
    lat2=lat2*M_PI/180.0;
    double a=sin(dLat/2.0)*sin(dLat/2.0)+cos(lat1)*cos(lat2)*sin(dLon/2.0)*sin(dLon/2.0);
    double c=2.0*atan2(sqrt(a),sqrt(1.0-a));
    return R*c*1000.0; // meters
}
struct AStarNode { int id; double f; };
struct AStarCompare { bool operator()(const AStarNode& a,const AStarNode& b) const { return a.f > b.f; } };
vector<int> aStarPath(const Graph& g,int start,int goal) {
    // Basic A* — returns empty vector if heuristic or nodes not present or no path
    if (!g.isValidAttraction(start) || !g.isValidAttraction(goal)) return {};
        Attraction sa=g.getAttraction(start);
        Attraction ga=g.getAttraction(goal);
    if (sa.latitude==0 && sa.longitude==0) return {};
    if (ga.latitude==0 && ga.longitude==0) return {};
// Heuristic: estimate distance from current node to goal using harversine(calculatres geogrpahic distance on earth with lat,long)

    unordered_map<int,double> gscore;
    unordered_map<int,double> fscore;
    unordered_map<int,int> cameFrom;
    unordered_set<int> closed;

    auto heuristic=[&](int node) {//the heuristic function(A* is dijkstra with heuristic)
        Attraction a=g.getAttraction(node);
        return haversine(a.latitude,a.longitude,ga.latitude,ga.longitude)/1000.0;
    };
    priority_queue<AStarNode,vector<AStarNode>,AStarCompare> pq;
    gscore[start]=0.0;
    fscore[start]=heuristic(start);
    pq.push({start,fscore[start]});

    while (!pq.empty()) {
        AStarNode cur=pq.top(); pq.pop();
        int u=cur.id;
        if (u==goal) {
            vector<int> path;
            int x=goal;
            while (cameFrom.find(x)!=cameFrom.end()) {
                path.push_back(x);
                x=cameFrom[x];
                }
            path.push_back(start);
            reverse(path.begin(),path.end());// Reconstruct the final path by walking backward from the goal to the start

            return path;
        }
        if (closed.count(u)) continue;
        closed.insert(u);
        auto nbrs=g.getNeighbors(u);
        for (size_t i=0; i<nbrs.size(); ++i) {
            int v=nbrs[i].first;
            double w=nbrs[i].second;
            if (closed.count(v)) continue;
            double tentative=gscore[u]+w;
            if (gscore.find(v)==gscore.end() || tentative<gscore[v]) {
                cameFrom[v]=u;
                gscore[v]=tentative;
                fscore[v]=tentative+heuristic(v);
                pq.push({v,fscore[v]});
            }
        }
    }
    return {};
}
