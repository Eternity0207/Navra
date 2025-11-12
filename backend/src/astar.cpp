#include "../include/algorithms.h"
#include <queue>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
double haversine(double lat1,double lon1,double lat2,double lon2) {
    const double R=6371.0;
    double dLat=(lat2-lat1)*M_PI/180.0;
    double dLon=(lon2-lon1)*M_PI/180.0;
    lat1=lat1*M_PI/180.0;
    lat2=lat2*M_PI/180.0;
    double a=sin(dLat/2.0)*sin(dLat/2.0)+cos(lat1)*cos(lat2)*sin(dLon/2.0)*sin(dLon/2.0);
    double c=2.0*atan2(sqrt(a),sqrt(1.0-a));
    return R*c;
}
struct AStarNode {
    int id;
    double fScore;
    bool operator>(const AStarNode& other) const {
        return fScore>other.fScore;
    }
};
std::vector<int> aStarPath(const Graph& g,int start,int goal) {
    std::priority_queue<AStarNode,std::vector<AStarNode>,
    std::greater<AStarNode>> pq;
    std::unordered_map<int,double> gScore;
    std::unordered_map<int,double> fScore;
    std::unordered_map<int,int> cameFrom;
    Attraction startAttr=g.getAttraction(start);
    Attraction goalAttr=g.getAttraction(goal);
    gScore[start]=0;
    fScore[start]=haversine(startAttr.latitude,startAttr.longitude,goalAttr.latitude,goalAttr.longitude);
    pq.push({start,fScore[start]});
    while (!pq.empty()) {
        int current=pq.top().id;
        pq.pop();
        if (current==goal) {
            std::vector<int> path;
            while (cameFrom.find(current)!=cameFrom.end()) {
                path.push_back(current); current=cameFrom[current];
            }
            path.push_back(start);
            std::reverse(path.begin(),path.end());
            return path;
        }
        auto neighbors=g.getNeighbors(current);
        for (size_t i=0; i<neighbors.size(); i++) {
            int neighbor=neighbors[i].first;
            double weight=neighbors[i].second;
            double tentativeG=gScore[current]+weight;
            if (gScore.find(neighbor)==gScore.end()||
                tentativeG<gScore[neighbor]) {
                cameFrom[neighbor]=current; gScore[neighbor]=tentativeG;
                Attraction neighborAttr=g.getAttraction(neighbor);
                double h=haversine(neighborAttr.latitude,neighborAttr.longitude,goalAttr.latitude,goalAttr.longitude);
                fScore[neighbor]=tentativeG+h;
                pq.push({neighbor,fScore[neighbor]});
            }
        }
    }
    return std::vector<int>();
}
