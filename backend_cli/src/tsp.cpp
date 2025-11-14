#include "../include/algorithms.h"
#include "../include/graph.h"
#include <limits>
#include <algorithm>
#include <unordered_set>
#include <vector>
using namespace std;
const double INF=numeric_limits<double>::infinity();
static vector<vector<double>> generateDistanceMatrix(const Graph& g,const vector<int>& locs) {
    int n =(int)locs.size();
    vector<vector<double>> dist(n,vector<double>(n,INF));
    for (int i=0; i<n; ++i) {
        auto d=dijkstra(g,locs[i]);
        for (int j=0; j<n; ++j) {
            if (j<(int)d.size()) dist[i][j]=d[locs[j]];
        }
        dist[i][i]=0;
    }
    return dist;
}
pair<double,vector<int>> computeOrderedRoute(const Graph& g,const vector<int>& order) {
    double total=0;
    vector<int> r=order;
    for (size_t i=0; i+1<order.size(); ++i) {
        auto dres=dijkstraWithPath(g,order[i]);
        double seg=INF;
        if (order[i+1]>=0 && order[i+1]<(int)dres.first.size()) seg=dres.first[order[i+1]];
        if (seg==INF) total+=1e9;
        else total+=seg;
    }
    return {total,r};
}
pair<double,vector<int>> tspDP(const vector<vector<double>>& dist) {
    int n=(int)dist.size();
    if (n==0) return {0,{}};
    if (n==1) return {0,{0}};
    if (n > 15) return {INF,{}};
    int ALL=1 << n;
    vector<vector<double>> dp(ALL,vector<double>(n,INF));
    vector<vector<int>> parent(ALL,vector<int>(n,-1));
    dp[1][0]=0;
    for (int mask=1; mask<ALL; ++mask) {
        for (int u=0; u<n; ++u) {
            if (!(mask & (1<<u))) continue;
            if (dp[mask][u]==INF) continue;
            for (int v=0; v<n; ++v) {
                if (mask & (1<<v)) continue;
                if (dist[u][v]==INF) continue;
                int nm=mask | (1<<v);
                double nd=dp[mask][u]+dist[u][v];
                if (nd<dp[nm][v]) { dp[nm][v]=nd; parent[nm][v]=u; }
            }
        }
    }
    int full=ALL-1; double best=INF; int last=-1;
    for (int i=0; i<n; ++i) if (dp[full][i]<best) { best=dp[full][i]; last=i; }
    vector<int> order;
    int mask=full;
    while (last!=-1) {
        order.push_back(last);
        int p=parent[mask][last];
        if (p!=-1) mask^=(1<<last);
        last=p;
    }
    reverse(order.begin(),order.end());
    return {best,order};
}
pair<double,vector<int>> tspMSTApproximation(const Graph& g,const vector<int>& locs) {
    int n=(int)locs.size();
    if (n==0) return {0,{}};
    auto dist=generateDistanceMatrix(g,locs);
    vector<Edge> edges;
    for (int i=0; i<n; ++i)
        for (int j=i+1; j<n; ++j)
            edges.push_back({i,j,dist[i][j]});
    vector<Edge> mst=kruskalMST(edges,n);
    vector<int> tour=mstToTour(mst,n,0);
    double total=0;
    for (int i=0; i+1<(int)tour.size(); ++i)
        total+=dist[tour[i]][tour[i+1]];
    twoOptImprovement(tour,dist);
    return {total,tour};
}
pair<double,vector<int>> greedyTSP(const Graph& g,int start,const vector<int>& locs) {
    int n=(int)locs.size();
    if (n==0) return {0,{}};
    auto dist=generateDistanceMatrix(g,locs);
    unordered_set<int> used;
    vector<int> order;
    used.insert(0); order.push_back(0);
    int cur=0;
    while ((int)used.size()<n) {
        double best=INF; int nxt=-1;
        for (int i=0; i<n; ++i) {
            if (!used.count(i) && dist[cur][i]<best) { best=dist[cur][i]; nxt=i; }
        }
        if (nxt==-1) break;
        used.insert(nxt); order.push_back(nxt); cur=nxt;
    }
    double total=0; for (int i=0; i+1<(int)order.size(); ++i) total+=dist[order[i]][order[i+1]];
    return {total,order};
}
void twoOptImprovement(vector<int>& tour,const vector<vector<double>>& dist) {
    int n=(int)tour.size();
    if (n<4) return;
    bool improved=true;
    while (improved) {
        improved=false;
        for (int i=1; i<n-2; ++i) {
            for (int j=i+1; j<n-1; ++j) {
                double oldD=dist[tour[i-1]][tour[i]]+dist[tour[j]][tour[j+1]];
                double newD=dist[tour[i-1]][tour[j]]+dist[tour[i]][tour[j+1]];
                if (newD+1e-9<oldD) { reverse(tour.begin()+i,tour.begin()+j+1); improved=true; }
            }
        }
    }
}
pair<double,vector<int>> computeOptimalRouteFree(const Graph& g,const vector<int>& locs) {
    int n=(int)locs.size();
    if (n<=10) return tspDP(generateDistanceMatrix(g,locs));
    if (n<=15) {
        auto dp=tspDP(generateDistanceMatrix(g,locs));
        auto mst=tspMSTApproximation(g,locs);
        if (dp.first<=mst.first) return dp;
        return mst;
    }
    return tspMSTApproximation(g,locs);
}
