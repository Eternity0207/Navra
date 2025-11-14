#include <iostream>
#include <string>
#include <vector>
#include "include/json.hpp"
#include "include/graph.h"
#include "include/api.h"

using json = nlohmann::json;
using namespace std;

int main() {
    try {
        // Read complete JSON input from stdin
        string input;
        string line;
        while (getline(cin, line)) {
            input += line;
        }

        // Parse JSON with error handling
        json j;
        try {
            j = json::parse(input);
        } catch (const json::parse_error& e) {
            json err;
            err["success"] = false;
            err["error"] = string("JSON parse error: ") + e.what();
            cout << err.dump() << endl;
            cout.flush();
            return 1;
        }

        // Validate required fields
        if (!j.contains("choice") || !j.contains("count") || !j.contains("locations")) {
            json err;
            err["success"] = false;
            err["error"] = "Missing required fields: choice, count, or locations";
            cout << err.dump() << endl;
            cout.flush();
            return 1;
        }

        int choice = j["choice"];
        int count = j["count"];
        vector<string> names = j["locations"];

        // Load graph
        Graph graph;
        try {
            graph.loadFromCSV("attractions.csv", "roads.csv");
        } catch (const exception& e) {
            json err;
            err["success"] = false;
            err["error"] = string("Failed to load graph data: ") + e.what();
            cout << err.dump() << endl;
            cout.flush();
            return 1;
        }

        // ------------------------------------------
        // Choice 4: Exit
        // ------------------------------------------
        if (choice == 4) {
            json out;
            out["success"] = true;
            out["message"] = "Exiting Route Optimizer";
            cout << out.dump() << endl;
            cout.flush();
            return 0;
        }

        // ------------------------------------------
        // Choice 3: Full campus traversal (MST + DFS + A*)
        // ------------------------------------------
        if (choice == 3) {
            ApiResult result = runFullGraphTraversal(graph);

            json out;
            if (!result.success) {
                out["success"] = false;
                out["error"] = result.errorMessage;
                out["algorithm"] = "Kruskal (MST) + DFS Traversal + A* Path Refinement";
            } else {
                out["success"] = true;
                out["algorithm"] = result.algorithm;
                out["totalTime"] = result.totalTime;
                out["routeIds"] = result.routeIds;
                out["routeNames"] = result.routeNames;
                out["stopCount"] = result.stopCount;
            }
            cout << out.dump() << endl;
            cout.flush();
            return 0;
        }

        // ------------------------------------------
        // Choices 1 & 2: TSP or Dijkstra
        // ------------------------------------------
        ApiResult result = runOptimizerAPI(choice, names, graph);

        json out;
        if (!result.success) {
            out["success"] = false;
            out["error"] = result.errorMessage;
        } else {
            out["success"] = true;
            out["algorithm"] = result.algorithm;
            out["totalTime"] = result.totalTime;
            out["routeIds"] = result.routeIds;
            out["routeNames"] = result.routeNames;
            out["stopCount"] = result.stopCount;
        }
        cout << out.dump() << endl;
        cout.flush();
        return 0;

    } catch (const exception& e) {
        json err;
        err["success"] = false;
        err["error"] = string("Unexpected error: ") + e.what();
        cout << err.dump() << endl;
        cout.flush();
        return 1;
    }
}
