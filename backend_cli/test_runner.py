#!/usr/bin/env python3
"""
New 20-test runner for the disconnected-graph dataset.
Detects DSU rejections robustly (either numeric sentinel or textual error).
Author: ChatGPT
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

def make_input(choice: int, locations: List[str]) -> str:
    s = f"{choice}\n"
    if choice in [1, 2]:
        s += f"{len(locations)}\n"
        for loc in locations:
            s += loc + "\n"
    s += "3\n"
    return s

def parse_output(stdout: str):
    """
    Returns (total_time, stops) where:
    - total_time: float >=0 on success, -1 on DSU rejection, None if parse failure
    - stops: int or None
    """
    lowered = stdout.lower()
    # Common textual rejection messages your app prints:
    if "not reachable" in lowered or "not connected" in lowered or "selected locations are not reachable" in lowered:
        return -1, -1

    # Try to parse numeric Total Time and Stops
    tm = re.search(r"Total Time:\s*([0-9]+(?:\.[0-9]+)?)", stdout)
    st = re.search(r"Stops:\s*(\d+)", stdout)
    if tm:
        total = float(tm.group(1))
        stops = int(st.group(1)) if st else None
        return total, stops

    # No total found and no textual rejection -> parse failure
    return None, None

# -------------------------
# Test definitions (20)
# -------------------------
TESTS = [
    # Dijkstra inside component A
    ("D1 CSE to Dining", 2, ["CSE Building", "Dining Hall"], 3, 4),
    ("D2 Main Gate to Medical", 2, ["Main Gate", "Medical Center"], 8, 20),
    ("D3 Workshop to Library", 2, ["Workshop", "Library"], 3, 10),

    # Dijkstra cross-component (should reject)
    ("D4 A to B unreachable", 2, ["CSE Building", "Innovation Lab"], -1, -1),
    ("D5 A to LostHut unreachable", 2, ["Library", "Lost Hut"], -1, -1),

    # Fixed order within A
    ("Z1 Fixed A component", 2, ["Main Gate", "Admin Block", "Library"], 6, 12),

    # TSP open-path in A
    ("T1 A-small TSP", 1, ["Main Gate", "Library", "Dining Hall"], 9, 20),
    ("T2 A-academic quad", 1, ["CSE Building","Library","Research Block","Workshop"], 10, 40),

    # TSP within B
    ("T3 B-tech loop", 1, ["Innovation Lab", "Robotics Bay", "Control Tower"], 7, 40),

    # TSP across components - should reject
    ("T4 TSP A+B mix reject", 1, ["Main Gate", "Innovation Lab"], -1, -1),
    ("T5 TSP A + Lost Hut reject", 1, ["Library", "Lost Hut", "Dining Hall"], -1, -1),

    # Single node tests
    ("S1 Single TSP", 1, ["CSE Building"], 0, 0),
    ("S2 Single Fixed", 2, ["Innovation Lab"], 0, 0),

    # DSU checks: connectivity checks
    ("DS1 A connected", 2, ["Main Gate", "Workshop"], 6, 20),
    ("DS2 B connected", 2, ["Innovation Lab", "Control Tower"], 4, 20),
    ("DS3 Component C isolated", 2, ["Lost Hut", "Main Gate"], -1, -1),

    # Full graph traversal: should fail because graph is disconnected
    ("F1 Full Graph Traversal", 4, [], -1, -1),

    # Random sanity checks
    ("R1 Random A", 2, ["Dining Hall", "Parking Lot"], 5, 20),
    ("R2 Random B", 2, ["Robotics Bay", "Testing Ground"], 2, 20),
    ("R3 Random C invalid", 2, ["Lost Hut", "Cafeteria"], -1, -1),
]

# -------------------------
# Runner
# -------------------------
def run_test(exe: str, name: str, choice: int, locs: List[str], tmin: float, tmax: float):
    print(f"\n{Color.CYAN}{Color.BOLD}Running: {name}{Color.RESET}")
    inp = make_input(choice, locs)
    try:
        p = subprocess.Popen([exe], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        stdout, stderr = p.communicate(inp, timeout=20)

        total, stops = parse_output(stdout)

        # Expected rejection
        if tmin == -1 and tmax == -1:
            if total == -1:
                print(f"{Color.OK}✓ Reject OK{Color.RESET}")
                return True
            else:
                print(f"{Color.FAIL}✗ Expected reject but got: {total}{Color.RESET}")
                print("=== program stdout ===")
                print(stdout)
                print("=== program stderr ===")
                print(stderr)
                return False

        if total is None:
            print(f"{Color.FAIL}✗ Could not parse output (expected numeric).{Color.RESET}")
            print("=== program stdout ===")
            print(stdout)
            print("=== program stderr ===")
            print(stderr)
            return False

        if tmin <= total <= tmax:
            print(f"{Color.OK}✓ Passed  (time={total}, stops={stops}){Color.RESET}")
            return True

        print(f"{Color.FAIL}✗ FAILED  (time={total}, expected {tmin}-{tmax}){Color.RESET}")
        print("=== program stdout ===")
        print(stdout)
        print("=== program stderr ===")
        print(stderr)
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
        ok = run_test(exe, name, choice, locs, mn, mx)
        if ok:
            passed += 1

    print("\n" + "="*60)
    print(f"{Color.BOLD}TOTAL: {passed}/{len(TESTS)} passed{Color.RESET}")

if __name__ == "__main__":
    main()
