#include "../include/graph.h"
#include <cmath>
#include <algorithm>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
Graph::Graph():numVertices(0) {}
void Graph::addAttraction(const Attraction& attr) {
    attractions[attr.id]=attr;
    if (adjList.find(attr.id)==adjList.end()) {
        adjList[attr.id]=std::vector<std::pair<int,double>>();
        numVertices++;
    }
}
void Graph::addEdge(int from,int to,double distance) {
    adjList[from].push_back(std::make_pair(to,distance));
}
std::vector<std::pair<int,double>> Graph::getNeighbors(int nodeId) const {
    auto it=adjList.find(nodeId);
    if (it!=adjList.end()) {
        return it->second;
    }
    return std::vector<std::pair<int,double>>();
}
Attraction Graph::getAttraction(int id) const {
    auto it=attractions.find(id);
    if (it!=attractions.end()) {
        return it->second;
    }
    return Attraction();
}
double Graph::getEdgeWeight(int from,int to) const {
    auto neighbors=getNeighbors(from);
    for (size_t i=0; i<neighbors.size(); i++) {
        if (neighbors[i].first==to) {
            return neighbors[i].second;
        }
    }
    return std::numeric_limits<double>::infinity();
}
std::vector<int> Graph::getAllAttractionIds() const {
    std::vector<int> ids;
    for (auto it=attractions.begin(); it!=attractions.end(); ++it) {
        ids.push_back(it->first);
    }
    return ids;
}
bool Graph::hasAttraction(int id) const {
    return attractions.find(id)!=attractions.end();
}
double haversineDistance(double lat1,double lon1,double lat2,double lon2) {
    const double R=6371.0;
    double dLat=(lat2-lat1)*M_PI/180.0;
    double dLon=(lon2-lon1)*M_PI/180.0;
    lat1=lat1*M_PI/180.0;
    lat2=lat2*M_PI/180.0;
    double a=sin(dLat/2.0)*sin(dLat/2.0)+cos(lat1)*cos(lat2)*sin(dLon/2.0)*sin(dLon/2.0);
    double c=2.0*atan2(sqrt(a),sqrt(1.0-a));
    return R*c;
}
void Graph::buildCompleteGraph() {
    std::vector<int> ids=getAllAttractionIds();
    for (size_t i=0; i<ids.size(); i++) {
        for (size_t j=0; j<ids.size(); j++) {
            if (ids[i]!=ids[j]) {
                Attraction attr1=attractions[ids[i]];
                Attraction attr2=attractions[ids[j]];
                double dist=haversineDistance(
                    attr1.latitude,attr1.longitude,
                    attr2.latitude,attr2.longitude
                );
                addEdge(ids[i],ids[j],dist);
            }
        }
    }
}
