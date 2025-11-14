#include "../include/graph.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <limits>
#include "../include/algorithms.h" // for Edge type in getAllEdges
using namespace std;
Graph::Graph():numVertices(0),dsu(nullptr) {}
Graph::~Graph() { if (dsu) delete dsu; }
void Graph::addAttraction(const Attraction& attr) {
    attractions[attr.id]=attr;
    if (!attr.name.empty()) nameToId[attr.name]=attr.id;
    if (adjList.find(attr.id)==adjList.end())
        adjList[attr.id]=vector<pair<int,double>>();
    numVertices=(int)attractions.size();
}
void Graph::addEdge(int from,int to,double weight) {
    if (from==to) return;
    if (adjList.find(from)==adjList.end()) adjList[from]={};
    if (adjList.find(to)==adjList.end()) adjList[to]={};
    adjList[from].push_back({to,weight});
    adjList[to].push_back({from,weight});
}
vector<pair<int,double>> Graph::getNeighbors(int nodeId) const {
    auto it=adjList.find(nodeId);
    if (it==adjList.end()) return {};
    return it->second;
}
Attraction Graph::getAttraction(int id) const {
    auto it=attractions.find(id);
    if (it==attractions.end()) return Attraction();
    return it->second;
}
double Graph::getEdgeWeight(int from,int to) const {
    auto it=adjList.find(from);
    if (it==adjList.end()) return numeric_limits<double>::infinity();
    for (auto &p:it->second) if (p.first==to) return p.second;
    return numeric_limits<double>::infinity();
}
vector<int> Graph::getAllAttractionIds() const {
    vector<int> ids;
    ids.reserve(attractions.size());
    for (auto &kv:attractions) ids.push_back(kv.first);
    return ids;
}
bool Graph::hasAttraction(int id) const {
    return attractions.find(id) != attractions.end();
}
int Graph::getIdByName(const string& name) const {
    auto it=nameToId.find(name);
    if (it==nameToId.end()) return -1;
    return it->second;
}
int Graph::maxNodeId() const {
    int maxId=-1;
    for (auto &kv:attractions) if (kv.first > maxId) maxId=kv.first;
    for (auto &kv:adjList) {
        if (kv.first > maxId) maxId=kv.first;
        for (auto &p:kv.second) if (p.first > maxId) maxId=p.first;
    }
    return maxId;
}
bool Graph::isValidAttraction(int id) const {
    return hasAttraction(id);
}
bool Graph::isFullyConnected() const {
    if (!dsu) return false;
    auto ids=getAllAttractionIds();
    if (ids.empty()) return false;
    int root=dsu->find(ids[0]);
    for (int id:ids) if (dsu->find(id) != root) return false;
    return true;
}
void Graph::buildDSU() {
    if (dsu) { delete dsu; dsu=nullptr; }
    int maxId=maxNodeId();
    if (maxId < 0) return;
    dsu=new DSU(maxId+1);
    for (auto &kv:adjList) {
        int u=kv.first;
        for (auto &p:kv.second) {
            int v=p.first;
            if (v >= 0) dsu->unite(u,v);
            }
        }
}
// CSV loader expecting attractions.csv header: name,category,rating,duration,fee,popularity,latitude,longitude
// and roads.csv header: from,to,time (names)
void Graph::loadFromCSV(const string& attractionsFile,const string& roadsFile) {
    attractions.clear();
    adjList.clear();
    nameToId.clear();
    //above 3 lines are required to CLEAR any
    //old stored nodes/adj lists from prior,so cleared every single time(important)
    numVertices=0;
    if (dsu) { delete dsu; dsu=nullptr; }
    ifstream aif(attractionsFile);
    if (!aif.is_open()) {
        cerr<<"[graph] cannot open attractions file: "<<attractionsFile<<"\n";
        return;
    }
    //catches error just in case file cannot be opened
    string line;
    // read header
    if (!getline(aif,line)) { aif.close(); return; }
    int nextId=0;
    while (getline(aif,line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        Attraction at;
        at.id=nextId++;
        string latS,lonS,ratingS,durationS,feeS,popS;
        getline(ss,at.name,',');
        getline(ss,at.category,',');
        getline(ss,ratingS,',');
        getline(ss,durationS,',');
        getline(ss,feeS,',');
        getline(ss,popS,',');
        getline(ss,latS,',');
        getline(ss,lonS,',');
        try {
            if (!ratingS.empty()) at.rating=stod(ratingS);
            if (!durationS.empty()) at.visitDuration=stod(durationS);
            if (!feeS.empty()) at.entryFee=stod(feeS);
            if (!popS.empty()) at.popularity=stoi(popS);
            if (!latS.empty()) at.latitude=stod(latS);
            if (!lonS.empty()) at.longitude=stod(lonS);
        } catch (...) { /* ignore parse errors */ }
        addAttraction(at);
    }
    aif.close();
    ifstream rif(roadsFile);
    if (!rif.is_open()) {
        cerr<<"[graph] cannot open roads file: "<<roadsFile<<"\n";
        buildDSU();
        return;
    }
    if (!getline(rif,line)) { rif.close(); buildDSU(); return; } // header
    while (getline(rif,line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string fromN,toN,wS;
        getline(ss,fromN,',');
        getline(ss,toN,',');
        getline(ss,wS,',');
        int u=getIdByName(fromN);
        int v=getIdByName(toN);
        double w=1.0;
        try { if (!wS.empty()) w=stod(wS); } catch(...) {}
        if (u != -1 && v != -1) addEdge(u,v,w);
    }
    rif.close();
    buildDSU();
}
vector<Edge> Graph::getAllEdges() const {
    vector<Edge> edges;
    unordered_map<long long,bool> seen;
    for (auto &kv:adjList) {
        int u=kv.first;
        for (auto &p:kv.second) {
            int v=p.first;
            double w=p.second;
            int a=min(u,v),b=max(u,v);
            long long key=((long long)a<<32) | (unsigned long long)b;
            if (!seen[key]) {
                Edge e; e.u=a; e.v=b; e.weight=w;
                edges.push_back(e);
                seen[key]=true;
            }
           }
       }
    return edges;
}