#include "../include/algorithms.h"
#include "../include/dsu.h"
#include <limits>
#include <algorithm>
#include <unordered_set>
#include <iostream>

const double INF=std::numeric_limits<double>::infinity();

std::pair<double,std::vector<int>> tspDP(const std::vector<std::vector<double>>& dist) {
    int n=dist.size();
    
    if (n==0) return std::make_pair(INF,std::vector<int>());
    if (n==1) return std::make_pair(0.0,std::vector<int>{0});
    if (n > 15) return std::make_pair(INF,std::vector<int>());

    int allMask=(1 << n)-1;
    std::vector<std::vector<double>> dp(1 << n,std::vector<double>(n,INF));
    std::vector<std::vector<int>> parent(1 << n,std::vector<int>(n,-1));

    dp[1][0]=0;

    for (int mask=1; mask<(1 << n); mask++) {
        for (int u=0; u<n; u++) {
            if (!(mask & (1 << u))) continue;
            if (dp[mask][u]==INF) continue;

            for (int v=0; v<n; v++) {
                if (mask & (1 << v)) continue;
                if (dist[u][v]==INF) continue;
                
                int newMask=mask | (1 << v);
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
    for (int i=0; i<n; i++) {
        if (dp[allMask][i]<minCost) {
            minCost=dp[allMask][i];
            lastCity=i;
        }
    }

    std::vector<int> path;
    if (lastCity==-1) return std::make_pair(INF,path);
    
    int mask=allMask;
    int curr=lastCity;

    while (curr!=-1) {
        path.push_back(curr);
        int prevCity=parent[mask][curr];
        if (prevCity!=-1) {
            mask ^= (1 << curr);
        }
        curr=prevCity;
    }

    std::reverse(path.begin(),path.end());
    return std::make_pair(minCost,path);
}

std::pair<double,std::vector<int>> tspMSTApproximation(const Graph& g,const std::vector<int>& locations) {
    int n=locations.size();
    std::vector<std::vector<double>> dist(n,std::vector<double>(n,INF));
    for (int i=0; i<n; i++) {
        dist[i][i]=0;
        auto dijkDist=dijkstra(g,locations[i]);
        for (int j=0; j<n; j++) {
            if (i!=j) {
                dist[i][j]=dijkDist[locations[j]];
            }
        }
    }
    std::vector<Edge> edges;
    for (int i=0; i<n; i++) {
        for (int j=i+1; j<n; j++) {
            edges.push_back({i,j,dist[i][j]});
        }
    }
    std::vector<Edge> mst=kruskalMST(edges,n);
    std::vector<int> tour=mstToTour(mst,n,0);
    double totalDist=0;
    for (size_t i=0; i<tour.size()-1; i++) {
        totalDist+=dist[tour[i]][tour[i+1]];
    }
    twoOptImprovement(tour,dist);
    totalDist=0;
    for (size_t i=0; i<tour.size()-1; i++) {
        totalDist+=dist[tour[i]][tour[i+1]];
    }
    
    return std::make_pair(totalDist,tour);
}

std::pair<double,std::vector<int>> greedyTSP(const Graph& g,int start,const std::vector<int>& mustVisit) {
    int n=mustVisit.size();
    std::vector<std::vector<double>> dist(n,std::vector<double>(n));
    
    for (int i=0; i<n; i++) {
        auto dijkDist=dijkstra(g,mustVisit[i]);
        for (int j=0; j<n; j++) {
            dist[i][j]=dijkDist[mustVisit[j]];
        }
    }
    
    int startIdx=0;
    for (int i=0; i<n; i++) {
        if (mustVisit[i]==start) {
            startIdx=i;
            break;
        }
    }
    
    std::vector<int> tour;
    tour.push_back(startIdx);
    std::unordered_set<int> visited;
    visited.insert(startIdx);

    int current=startIdx;
    double totalDist=0;

    while (visited.size()<(size_t)n) {
        double minDist=INF;
        int nextIdx=-1;

        for (int i=0; i<n; i++) {
            if (visited.find(i)==visited.end()) {
                if (dist[current][i]<minDist) {
                    minDist=dist[current][i];
                    nextIdx=i;
                }
            }
        }
        if (nextIdx!=-1) {
            tour.push_back(nextIdx);
            visited.insert(nextIdx);
            totalDist+=minDist;
            current=nextIdx;
        } else {
            break;
        }
    }
    return std::make_pair(totalDist,tour);
}
void twoOptImprovement(std::vector<int>& tour,const std::vector<std::vector<double>>& dist) {
    int n=tour.size();
    bool improved=true;
    int iterations=0;
    const int MAX_ITERATIONS=100;
    while (improved && iterations<MAX_ITERATIONS) {
        improved=false;
        iterations++;
        for (int i=1; i<n-1; i++) {
            for (int j=i+1; j<n; j++) {
                double oldDist=dist[tour[i-1]][tour[i]]+dist[tour[j]][tour[(j+1) % n]];
                double newDist=dist[tour[i-1]][tour[j]]+dist[tour[i]][tour[(j+1) % n]];
                if (newDist<oldDist-0.001) {
                    std::reverse(tour.begin()+i,tour.begin()+j+1);
                    improved=true;
                }
            }
        }
    }
}
std::pair<double,std::vector<int>> computeOptimalRoute(const Graph& g,const std::vector<int>& locations,bool flexibleOrder) {
    int n=locations.size();
    if (n==1) {
        return std::make_pair(0.0,std::vector<int>{0});
    }
    if (!flexibleOrder) {
        return computeOrderedRoute(g,locations);
    }
    std::vector<std::vector<double>> distMatrix(n,std::vector<double>(n,INF));
    for (int i=0; i<n; i++) {
        distMatrix[i][i]=0;
        auto dijkDist=dijkstra(g,locations[i]);
        for (int j=0; j<n; j++) {
            if (i!=j) {
                distMatrix[i][j]=dijkDist[locations[j]];
            }
        }
    }
    
    if (n <= 10) {
        return tspDP(distMatrix);
    } else if (n <= 15) {
        auto dpResult=tspDP(distMatrix);
        auto mstResult=tspMSTApproximation(g,locations);
        
        if (dpResult.first <= mstResult.first) {
            return dpResult;
        } else {
            return mstResult;
        }
    } else {
        return tspMSTApproximation(g,locations);
    }
}
