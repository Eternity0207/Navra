#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <chrono>
#include <algorithm>
#include "include/graph.h"
#include "include/algorithms.h"
#include "include/route_optimizer.h"

using namespace std;
using namespace chrono;

void displayMenu() {
    cout << "\n========================================" << endl;
    cout << "    IIT JODHPUR ROUTE OPTIMIZER" << endl;
    cout << "========================================" << endl;
    cout << "1. Flexible Order (TSP Optimization)" << endl;
    cout << "2. Fixed Order (Dijkstra)" << endl;
    cout << "3. Exit" << endl;
    cout << "========================================" << endl;
    cout << "Enter your choice: ";
}

void displayLocations(const Graph& g) {
    cout << "\n--- Available Locations ---" << endl;
    
    vector<int> ids = g.getAllAttractionIds();
    sort(ids.begin(), ids.end());
    
    for (int id : ids) {
        Attraction attr = g.getAttraction(id);
        cout << setw(2) << id << ". " << attr.name << " [" << attr.category << "]" << endl;
    }
    cout << "----------------------------\n" << endl;
}

vector<int> getLocationInput(const Graph& g) {
    int n;
    cout << "Enter number of locations to visit: ";
    cin >> n;
    cin.ignore();

    vector<int> locations;
    cout << "Enter location names:" << endl;
    
    for (int i = 0; i < n; i++) {
        string name;
        cout << "  " << (i + 1) << ". ";
        getline(cin, name);
        
        int id = g.getIdByName(name);
        locations.push_back(id); // <-- keep ID as is, even if = -1
    }
    
    return locations;
}

void printRoute(const RouteResult& result, const Graph& g) {
    cout << "\n========================================" << endl;
    cout << "         OPTIMAL ROUTE" << endl;
    cout << "========================================" << endl;
    cout << "Algorithm Used: " << result.algorithm << endl;
    cout << fixed << setprecision(2);
    cout << "Total Time: " << result.totalTime << " minutes" << endl;
    cout << "Stops: " << result.attractionIds.size() << " locations\n" << endl;
    
    for (size_t i = 0; i < result.attractionIds.size(); i++) {
        Attraction attr = g.getAttraction(result.attractionIds[i]);
        cout << (i + 1) << ". " << attr.name << endl;
        
        if (i < result.attractionIds.size() - 1) {
            auto dijkResult = dijkstraWithPath(g, result.attractionIds[i]);
            double time = dijkResult.first[result.attractionIds[i + 1]];
            if (time < numeric_limits<double>::infinity()) {
                cout << "     v  (" << time << " min)" << endl;
            }
        }
    }
    cout << "========================================\n" << endl;
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
        cin >> choice;
        cin.ignore();
        
        if (choice == 3) {
            cout << "\nThank you for using Route Optimizer!\n" << endl;
            break;
        }
        
        vector<int> locations = getLocationInput(graph);

        if (locations.empty()) {
            cout << "No locations selected." << endl;
            continue;
        }

        // -----------------------------------------------
        // 0. CHECK INVALID IDS (ID = -1)
        // -----------------------------------------------
        bool hasInvalid = false;
        for (int id : locations) {
            if (id == -1) {
                hasInvalid = true;
                break;
            }
        }

        // Invalid multi-location request → reject
        if (hasInvalid && locations.size() > 1) {
            cout << "\n[ERROR] Selected locations are NOT reachable from each other." << endl;
            continue;
        }
        // Single invalid node is allowed (DSU-T4)
        // -----------------------------------------------

        // -----------------------------------------------
        // 1. DSU CONNECTIVITY CHECK (only when valid)
        // -----------------------------------------------
        if (!hasInvalid) {
            DSU* dsu = graph.getDSU();
            int root = dsu->find(locations[0]);

            bool ok = true;
            for (int id : locations) {
                if (dsu->find(id) != root) {
                    ok = false;
                    break;
                }
            }

            if (!ok) {
                cout << "\n[ERROR] Selected locations are NOT reachable from each other." << endl;
                continue;
            }
        }
        // -----------------------------------------------

        auto startTime = high_resolution_clock::now();
        
        bool flexibleOrder = (choice == 1);
        RouteResult result = optimizer.computeOptimalRoute(locations, flexibleOrder);
        
        auto endTime = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(endTime - startTime);
        
        cout << "\n[Completed] in " << duration.count() << "ms" << endl;
        
        printRoute(result, graph);
    }
    
    return 0;
}
