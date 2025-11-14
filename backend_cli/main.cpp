#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "include/graph.h"
#include "include/route_optimizer.h"
#include "include/algorithms.h"
#include <algorithm>


using namespace std;

void displayMenu() {
    cout << "\n========================================\n";
    cout << "    IIT JODHPUR ROUTE OPTIMIZER\n";
    cout << "========================================\n";
    cout << "1. Flexible Order (TSP Optimization)\n";
    cout << "2. Fixed Order (Dijkstra)\n";
    cout << "3. Exit\n";
    cout << "4. Traverse Entire Graph (Kruskal + DFS + A*)\n";
    cout << "========================================\n";
    cout << "Enter your choice: ";
}

void displayLocations(const Graph& g) {
    cout << "\n--- Available Locations ---\n";
    auto ids = g.getAllAttractionIds();
    sort(ids.begin(), ids.end());
    for (int id : ids) {
        auto a = g.getAttraction(id);
        cout << setw(2) << id << ". " << a.name << " [" << a.category << "]\n";
    }
    cout << "----------------------------\n";
}

vector<int> getLocationInput(const Graph& g) {
    int n;
    cout << "Enter number of locations to visit: ";
    if (!(cin >> n)) { cin.clear(); cin.ignore(10000,'\n'); return {}; }
    cin.ignore();
    vector<int> locs;
    cout << "Enter location names:\n";
    for (int i = 0; i < n; ++i) {
        string name;
        cout << "  " << (i+1) << ". ";
        getline(cin, name);
        int id = g.getIdByName(name);
        locs.push_back(id);
    }
    return locs;
}

void printRoute(const RouteResult& result, const Graph& g) {
    cout << "\n========================================\n";
    cout << "         OPTIMAL ROUTE\n";
    cout << "========================================\n";
    cout << "Algorithm Used: " << result.algorithm << "\n";
    cout << fixed << setprecision(2);
    cout << "Total Time: " << result.totalTime << " minutes\n";
    cout << "Stops: " << result.attractionIds.size() << " locations\n\n";
    for (size_t i = 0; i < result.attractionIds.size(); ++i) {
        auto a = g.getAttraction(result.attractionIds[i]);
        cout << (i+1) << ". " << a.name << "\n";
    }
    cout << "========================================\n";
}

int main() {
    Graph graph;
    graph.loadFromCSV("attractions.csv", "roads.csv");

    RouteOptimizer optimizer;
    optimizer.setGraph(graph);

    displayLocations(graph);

    while (true) {
        displayMenu();
        int choice;
        if (!(cin >> choice)) { cin.clear(); cin.ignore(10000,'\n'); continue; }
        cin.ignore();
        if (choice == 3) { cout << "\nThank you for using Route Optimizer!\n"; break; }
        if (choice == 4) {
            auto res = optimizer.computeFullGraphRoute();
            if (res.attractionIds.empty()) cout << "[ERROR] Entire graph is NOT connected. Full traversal impossible.\n";
            else printRoute(res, graph);
            continue;
        }
        vector<int> locations = getLocationInput(graph);
        if (locations.empty()) { cout << "No locations selected.\n"; continue; }

        // check invalid ids
        bool hasInvalid = false;
        for (int id : locations) if (id == -1) hasInvalid = true;
        if (hasInvalid && locations.size() > 1) {
            cout << "\n[ERROR] Selected locations are NOT reachable from each other.\n";
            continue;
        }
        if (!hasInvalid) {
            DSU* dsu = graph.getDSU();
            if (dsu) {
                int root = dsu->find(locations[0]);
                bool ok = true;
                for (int id : locations) if (dsu->find(id) != root) { ok = false; break; }
                if (!ok) { cout << "\n[ERROR] Selected locations are NOT reachable from each other.\n"; continue; }
            }
        }

        bool flexible = (choice == 1);
        RouteResult res = optimizer.computeOptimalRoute(locations, flexible);
        printRoute(res, graph);
    }
    return 0;
}
