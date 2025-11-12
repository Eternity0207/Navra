#include "../include/algorithms.h"
#include <vector>
#include <limits>
#include <algorithm>
#include <unordered_set>
const double INF=std::numeric_limits<double>::infinity();
std::pair<double,std::vector<int>>
tspDP(const std::vector<std::vector<double>>& dist) {
    int n=dist.size();
    if (n>15||n==0) return std::make_pair(INF,std::vector<int>());
    int allMask=(1<<n)-1;
    std::vector<std::vector<double>> dp(1<<n,std::vector<double>(n,INF));
    std::vector<std::vector<int>> parent(1<<n,std::vector<int>(n,-1));
    dp[1][0]=0;
    for (int mask=1; mask<(1<<n); mask++) {
        for (int u=0; u<n; u++) {
            if (!(mask&(1<<u))) continue;
            if (dp[mask][u]==INF) continue;
            for (int v=0; v<n; v++) {
                if (mask&(1<<v)) continue;
                int newMask=mask|(1<<v);
                double newDist=dp[mask][u]+dist[u][v];
                if (newDist<dp[newMask][v]) {
                    dp[newMask][v]=newDist;
                    parent[newMask][v]=u;
                }
            }
        }
    }
    double minCost=INF;
    int lastCity=-1;
    for (int i=1; i<n; i++) {
        double cost=dp[allMask][i]+dist[i][0];
        if (cost<minCost) {
            minCost=cost; lastCity=i;
        }
    }
    std::vector<int> path;
    int mask=allMask;
    int curr=lastCity;
    while (curr!=-1) {
        path.push_back(curr);
        int prevCity=parent[mask][curr];
        if (prevCity!=-1) {
            mask^=(1<<curr);
        }
        curr=prevCity;
    }
    std::reverse(path.begin(),path.end());
    return std::make_pair(minCost,path);
}
std::pair<double,std::vector<int>>
greedyTSP(const Graph& g,int start,const std::vector<int>& mustVisit) {
    std::vector<int> tour;
    tour.push_back(start);
    std::unordered_set<int> visited;
    visited.insert(start);
    int current=start;
    double totalDist=0;
    while (visited.size()<mustVisit.size()) {
        double minDist=INF;
        int nextCity=-1;
        for (size_t i=0; i<mustVisit.size(); i++) {
            if (visited.count(mustVisit[i])) continue;
            double dist=g.getEdgeWeight(current,mustVisit[i]);
            if (dist<minDist) {
                minDist=dist; nextCity=mustVisit[i];
            }
        }
        if (nextCity==-1) break;
        tour.push_back(nextCity); visited.insert(nextCity); totalDist+=minDist; current=nextCity;
    }
    totalDist+=g.getEdgeWeight(current,start); tour.push_back(start);
    return std::make_pair(totalDist,tour);
}
void twoOptImprovement(std::vector<int>& tour,const std::vector<std::vector<double>>& dist) {
    int n=tour.size();
    bool improved=true;
    while (improved) {
        improved=false;
        for (int i=1; i<n-2; i++) {
            for (int j=i+1; j<n-1; j++) {
                double oldDist=dist[tour[i-1]][tour[i]]+dist[tour[j]][tour[j+1]];
                double newDist=dist[tour[i-1]][tour[j]]+dist[tour[i]][tour[j+1]];
                if (newDist<oldDist) {
                    std::reverse(tour.begin()+i,tour.begin()+j+1); improved=true;
                }
            }
        }
    }
}