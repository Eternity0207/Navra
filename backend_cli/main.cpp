#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

#include "include/graph.h"
#include "include/route_optimizer.h"
#include "include/algorithms.h"

using namespace std;

//
// Display Menu with clear algorithm descriptions
//
void displayMenu() {
    cout << "\n============================================\n";
    cout << "          IIT JODHPUR ROUTE OPTIMIZER\n";
    cout << "============================================\n";
    cout << "1. Flexible Order Route  (Traveling Salesman Problem - TSP)\n";
    cout << "2. Fixed Order Route     (Shortest Path using Dijkstra's Algorithm)\n";
    cout << "3. Explore Entire Campus (MST: Kruskal + Traversal: DFS + Path Refinement: A*)\n";
    cout << "4. Exit Program\n";
    cout << "============================================\n";
    cout << "Enter your choice: ";
}

//
// Display list of locations
//
void displayLocations(const Graph& g) {
    cout << "\n---------------- Available Locations ----------------\n";

    auto ids = g.getAllAttractionIds();
    sort(ids.begin(), ids.end());

    for (int id : ids) {
        auto a = g.getAttraction(id);
        cout << setw(2) << id << ". " << a.name
             << "   (" << a.category << ")\n";
    }

    cout << "------------------------------------------------------\n";
}

//
// Read location names
//
vector<int> getLocationInput(const Graph& g) {
    int n;
    cout << "Enter number of locations you want to visit: ";

    if (!(cin >> n)) {
        cin.clear();
        cin.ignore(10000, '\n');
        return {};
    }

    cin.ignore(); // remove newline

    vector<int> locs;
    cout << "Enter location names (one per line):\n";

    for (int i = 0; i < n; ++i) {
        cout << "  Location " << (i + 1) << ": ";
        string name;
        getline(cin, name);
        locs.push_back(g.getIdByName(name));
    }

    return locs;
}

//
// Display final route with full intermediate path
//
void printRoute(const RouteResult& result, const Graph& g) {
    cout << "\n============================================\n";
    cout << "                 ROUTE SUMMARY\n";
    cout << "============================================\n";

    cout << "Algorithm Used : " << result.algorithm << "\n";
    cout << fixed << setprecision(2);
    cout << "Total Time     : " << result.totalTime << " minutes\n";
    cout << "Total Stops    : " << result.attractionIds.size() << "\n\n";

    // Print requested stops
    cout << "Requested Stops (in order):\n";
    for (size_t i = 0; i < result.attractionIds.size(); ++i) {
        auto a = g.getAttraction(result.attractionIds[i]);
        cout << (i + 1) << ". " << a.name << "\n";
    }

    // Print FULL PATH
    cout << "\nFULL OPTIMAL PATH (including intermediate nodes):\n";
    if (result.fullPath.empty()) {
        cout << "(No path reconstructed)\n";
    } else {
        for (size_t i = 0; i < result.fullPath.size(); ++i) {
            auto a = g.getAttraction(result.fullPath[i]);
            if (i == 0) cout << a.name;
            else        cout << " -> " << a.name;
        }
        cout << "\n";
    }

    cout << "============================================\n";
}

//
// MAIN PROGRAM
//
int main() {
    Graph graph;
    graph.loadFromCSV("attractions.csv", "roads.csv");

    RouteOptimizer optimizer;
    optimizer.setGraph(graph);

    displayLocations(graph);

    while (true) {
        displayMenu();

        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n[ERROR] Invalid input. Please enter 1-4.\n";
            continue;
        }
        cin.ignore();

        // Exit
        if (choice == 4) {
            cout << "\nThank you for using the IIT Jodhpur Route Optimizer!\n";
            break;
        }

        // Full campus traversal
        if (choice == 3) {
            cout << "\n[INFO] Running: Kruskal (MST) + DFS Traversal + A* Path Refinement\n";
            auto res = optimizer.computeFullGraphRoute();

            if (res.attractionIds.empty()) {
                cout << "\n[ERROR] Campus graph is not fully connected.\n";
                cout << "        Full traversal (Kruskal + DFS + A*) cannot be performed.\n";
            } else {
                printRoute(res, graph);
            }
            continue;
        }

        // For TSP or Dijkstra
        vector<int> locations = getLocationInput(graph);

        if (locations.empty()) {
            cout << "\n[INFO] No locations selected.\n";
            continue;
        }

        // Validate names
        bool invalid = false;
        for (int id : locations) {
            if (id == -1) invalid = true;
        }

        if (invalid) {
            cout << "\n[ERROR] One or more location names do not exist.\n";
            cout << "        Please check spelling and try again.\n";
            continue;
        }

        // DSU connectivity check
        DSU* dsu = graph.getDSU();
        if (dsu) {
            int root = dsu->find(locations[0]);
            bool reachable = true;

            for (int id : locations) {
                if (dsu->find(id) != root) {
                    reachable = false;
                    break;
                }
            }

            if (!reachable) {
                cout << "\n[ERROR] Selected locations are not reachable from each other.\n";
                cout << "        DSU connectivity check failed.\n";
                continue;
            }
        }

        bool flexibleOrder = (choice == 1);

        if (flexibleOrder) {
            cout << "\n[INFO] Running: Traveling Salesman Problem (TSP) — Optimal Reordering\n";
        } else {
            cout << "\n[INFO] Running: Fixed Order Route using Dijkstra's Shortest Path Algorithm\n";
        }

        RouteResult result = optimizer.computeOptimalRoute(locations, flexibleOrder);
        printRoute(result, graph);
    }

    return 0;
}
