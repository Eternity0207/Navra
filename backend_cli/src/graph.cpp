#include "../include/graph.h"
#include <cmath>
#include <limits>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

Graph::Graph() : numVertices(0), dsu(nullptr) {}

void Graph::addAttraction(const Attraction& attr) {
    attractions[attr.id] = attr;
    nameToId[attr.name] = attr.id;

    if (adjList.find(attr.id) == adjList.end()) {
        adjList[attr.id] = vector<pair<int,double>>();
    }
}

void Graph::addEdge(int from, int to, double weight) {
    adjList[from].push_back({to, weight});
    adjList[to].push_back({from, weight});
}

vector<pair<int,double>> Graph::getNeighbors(int id) const {
    if (adjList.count(id)) return adjList.at(id);
    return {};
}

Attraction Graph::getAttraction(int id) const {
    if (attractions.count(id)) return attractions.at(id);
    return Attraction();
}

double Graph::getEdgeWeight(int from, int to) const {
    if (!adjList.count(from)) return numeric_limits<double>::infinity();
    for (auto& p : adjList.at(from)) {
        if (p.first == to) return p.second;
    }
    return numeric_limits<double>::infinity();
}

int Graph::getIdByName(const string& name) const {
    auto it = nameToId.find(name);
    if (it == nameToId.end()) return -1;
    return it->second;
}

vector<int> Graph::getAllAttractionIds() const {
    vector<int> v;
    for (auto& p : attractions) v.push_back(p.first);
    return v;
}

void Graph::buildDSU() {

    if (dsu) delete dsu;

    int maxId = 0;
    for (auto& p : attractions)
        maxId = max(maxId, p.first);

    dsu = new DSU(maxId + 1);

    for (auto& entry : adjList) {
        int u = entry.first;
        for (auto &p : entry.second) {
            int v = p.first;
            dsu->unite(u, v);
        }
    }
}

void Graph::loadFromCSV(const string& attFile, const string& roadsFile) {
    ifstream f(attFile);

    string line;
    getline(f, line);
    int id = 0;

    while (getline(f, line)) {
        if (line.empty()) continue;
        stringstream ss(line);

        string name, category;
        double rating, duration, fee, lat, lon;
        int pop;

        getline(ss, name, ',');
        getline(ss, category, ',');
        ss >> rating; ss.ignore();
        ss >> duration; ss.ignore();
        ss >> fee; ss.ignore();
        ss >> pop; ss.ignore();
        ss >> lat; ss.ignore();
        ss >> lon;

        Attraction at(id, name, category, lat, lon, duration, rating, fee, pop);
        addAttraction(at);
        id++;
    }
    f.close();

    ifstream fr(roadsFile);
    getline(fr, line);

    while (getline(fr, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string A, B;
        double w;

        getline(ss, A, ',');
        getline(ss, B, ',');
        ss >> w;

        if (!A.empty() && A.back() == '\r') A.pop_back();
        if (!B.empty() && B.back() == '\r') B.pop_back();

        int u = getIdByName(A);
        int v = getIdByName(B);

        if (u != -1 && v != -1) {
            addEdge(u, v, w);
        }
    }
    fr.close();

    buildDSU();
}
