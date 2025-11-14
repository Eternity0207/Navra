#!/usr/bin/env python3
"""
Updated 20-test runner for the polished IIT Jodhpur Route Optimizer.
Matches the new menu:
1 = TSP
2 = Dijkstra
3 = Full Graph Traversal
4 = Exit
"""

import subprocess
import sys
import re
import os
from typing import List

class Color:
    OK = "\033[92m"
    FAIL = "\033[91m"
    CYAN = "\033[96m"
    YELLOW = "\033[93m"
    RESET = "\033[0m"
    BOLD = "\033[1m"

# -------------------------------------
# USER INPUT GENERATOR (updated)
# -------------------------------------
def make_input(choice: int, locations: List[str]) -> str:
    """
    Produces simulated input for the program.
    - For choice 1 or 2: ask for number of locations + names.
    - For choice 3: full graph traversal.
    - Program terminates by sending "4".
    """
    s = f"{choice}\n"

    if choice in [1, 2]:
        s += f"{len(locations)}\n"
        for loc in locations:
            s += loc + "\n"

    # After the test finishes → exit program
    s += "4\n"
    return s

# -------------------------------------
# OUTPUT PARSER (updated to match polished messages)
# -------------------------------------
def parse_output(stdout: str):
    """
    Returns (total_time, stops)
    - total_time >= 0 → numeric success
    - total_time == -1 → rejection
    - total_time == None → fatal parse failure
    """

    lowered = stdout.lower()

    # New rejection triggers
    rejection_patterns = [
        "not reachable",
        "not connected",
        "cannot be performed",
        "full-graph traversal",
        "do not exist",          # invalid names
        "check the spelling",    # invalid names
        "connected region"       # DSU rejection
    ]

    if any(p in lowered for p in rejection_patterns):
        return -1, -1

    # Success pattern
    tm = re.search(r"Total Time\s*:\s*([0-9]+(?:\.[0-9]+)?)", stdout)
    st = re.search(r"Total Stops\s*:\s*(\d+)", stdout)

    if tm:
        total = float(tm.group(1))
        stops = int(st.group(1)) if st else None
        return total, stops

    return None, None


# -------------------------------------
# TEST DEFINITIONS (F1 updated to use choice 3)
# -------------------------------------
TESTS = [
    ("D1 CSE to Dining", 2, ["CSE Building", "Dining Hall"], 3, 4),
    ("D2 Main Gate to Medical", 2, ["Main Gate", "Medical Center"], 8, 20),
    ("D3 Workshop to Library", 2, ["Workshop", "Library"], 3, 10),

    ("D4 A to B unreachable", 2, ["CSE Building", "Innovation Lab"], -1, -1),
    ("D5 A to LostHut unreachable", 2, ["Library", "Lost Hut"], -1, -1),

    ("Z1 Fixed A component", 2, ["Main Gate", "Admin Block", "Library"], 6, 12),

    ("T1 A-small TSP", 1, ["Main Gate", "Library", "Dining Hall"], 9, 20),
    ("T2 A-academic quad", 1, ["CSE Building","Library","Research Block","Workshop"], 10, 40),

    ("T3 B-tech loop", 1, ["Innovation Lab", "Robotics Bay", "Control Tower"], 7, 40),

    ("T4 TSP A+B mix reject", 1, ["Main Gate", "Innovation Lab"], -1, -1),
    ("T5 TSP A + Lost Hut reject", 1, ["Library", "Lost Hut", "Dining Hall"], -1, -1),

    ("S1 Single TSP", 1, ["CSE Building"], 0, 0),
    ("S2 Single Fixed", 2, ["Innovation Lab"], 0, 0),

    ("DS1 A connected", 2, ["Main Gate", "Workshop"], 6, 20),
    ("DS2 B connected", 2, ["Innovation Lab", "Control Tower"], 4, 20),
    ("DS3 Component C isolated", 2, ["Lost Hut", "Main Gate"], -1, -1),

    # Updated: Full graph traversal = choice 3
    ("F1 Full Graph Traversal", 3, [], -1, -1),

    ("R1 Random A", 2, ["Dining Hall", "Parking Lot"], 5, 20),
    ("R2 Random B", 2, ["Robotics Bay", "Testing Ground"], 2, 20),
    ("R3 Random C invalid", 2, ["Lost Hut", "Cafeteria"], -1, -1),
]

# -------------------------------------
# RUNNER
# -------------------------------------
def run_test(exe: str, name: str, choice: int, locs: List[str], tmin: float, tmax: float):
    print(f"\n{Color.CYAN}{Color.BOLD}Running: {name}{Color.RESET}")
    inp = make_input(choice, locs)

    try:
        p = subprocess.Popen([exe], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE, text=True)
        stdout, stderr = p.communicate(inp, timeout=20)

        total, stops = parse_output(stdout)

        # Expected rejection
        if tmin == -1:
            if total == -1:
                print(f"{Color.OK}✓ Reject OK{Color.RESET}")
                return True
            print(f"{Color.FAIL}✗ Expected rejection but got: total={total}{Color.RESET}")
            print("=== PROGRAM STDOUT ===")
            print(stdout)
            return False

        # Expected numeric success
        if total is None:
            print(f"{Color.FAIL}✗ Could not parse output (expected numeric results).{Color.RESET}")
            print(stdout)
            return False

        if tmin <= total <= tmax:
            print(f"{Color.OK}✓ Passed  (time={total}, stops={stops}){Color.RESET}")
            return True

        print(f"{Color.FAIL}✗ FAILED  (time={total}, expected {tmin}-{tmax}){Color.RESET}")
        print(stdout)
        return False

    except subprocess.TimeoutExpired:
        print(f"{Color.FAIL}✗ TIMEOUT{Color.RESET}")
        return False


def main():
    exe = "./optimizer.exe"
    if len(sys.argv) > 1:
        exe = sys.argv[1]

    if not os.path.exists(exe):
        print(f"{Color.FAIL}Executable not found: {exe}{Color.RESET}")
        return

    passed = 0
    for name, choice, locs, mn, mx in TESTS:
        if run_test(exe, name, choice, locs, mn, mx):
            passed += 1

    print("\n" + "="*60)
    print(f"{Color.BOLD}TOTAL: {passed}/{len(TESTS)} passed{Color.RESET}")


if __name__ == "__main__":
    main()
