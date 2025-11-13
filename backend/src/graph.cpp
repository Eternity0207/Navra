#include "../include/graph.h"
#include <cmath>
#include <limits>
#include <fstream>
#include <sstream>
#include <iostream>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
Graph::Graph() : numVertices(0) {}
void Graph::addAttraction(const Attraction& attr) {
    attractions[attr.id]=attr;
    nameToId[attr.name]=attr.id;
    if (adjList.find(attr.id)==adjList.end()) {
        adjList[attr.id]=std::vector<std::pair<int,double>>();
        numVertices++;
    }
}
void Graph::addEdge(int from,int to,double weight) {
    adjList[from].push_back(std::make_pair(to,weight));
    adjList[to].push_back(std::make_pair(from,weight));
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
int Graph::getIdByName(const std::string& name) const {
    auto it=nameToId.find(name);
    if (it!=nameToId.end()) {
        return it->second;
    }
    return -1;
}
void Graph::loadFromCSV(const std::string& attractionsFile,const std::string& roadsFile) {
    std::ifstream attFile(attractionsFile);
    if (!attFile.is_open()) {
        std::cerr << "Error: Cannot open " << attractionsFile << std::endl;
        return;
    }
    std::string line;
    std::getline(attFile,line);
    int id=0;
    while (std::getline(attFile,line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string name,category;
        double rating,duration,fee,lat,lon;
        int pop;
        std::getline(ss,name,',');
        std::getline(ss,category,',');
        ss >> rating; ss.ignore();
        ss >> duration; ss.ignore();
        ss >> fee; ss.ignore();
        ss >> pop; ss.ignore();
        ss >> lat; ss.ignore();
        ss >> lon;
        Attraction attr(id,name,category,lat,lon,duration,rating,fee,pop);
        addAttraction(attr);
        id++;
    }
    attFile.close();
    std::cout << "Loaded " << id << " attractions." << std::endl;
    std::ifstream roadsFileStream(roadsFile);
    if (!roadsFileStream.is_open()) {
        std::cerr << "Error: Cannot open " << roadsFile << std::endl;
        return;
    }
    std::getline(roadsFileStream,line);
    int edgeCount=0;
    while (std::getline(roadsFileStream,line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string from,to;
        double time;
        std::getline(ss,from,',');
        std::getline(ss,to,',');
        ss >> time;
        if (!from.empty() && from[from.length()-1]=='\r') 
            from=from.substr(0,from.length()-1);
        if (!to.empty() && to[to.length()-1]=='\r') 
            to=to.substr(0,to.length()-1);
        int fromId=getIdByName(from);
        int toId=getIdByName(to);
        if (fromId!=-1 && toId!=-1) {
            addEdge(fromId,toId,time);
            edgeCount++;
        }
    }
    roadsFileStream.close();
    std::cout << "Loaded " << edgeCount << " roads." << std::endl;
}