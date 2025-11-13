#!/usr/bin/env python3
"""
Extended Automated Test Runner for IIT Jodhpur Route Optimizer
100 deterministic test cases:
- Dijkstra tests
- TSP tests
- DSU connectivity
- edge cases and stress tests
"""

import subprocess
import sys
import time
import re
from typing import List, Tuple, Optional
import os
import random
import json

# Make runs reproducible
random.seed(0)

class Color:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'


# Valid nodes - must match your attractions.csv exactly
VALID_NODES = [
    "Main Gate", "CSE Building", "Library", "Dining Hall",
    "Hostel A", "Lecture Hall Complex", "Sports Complex",
    "Innovation Center", "Canteen", "Medical Center",
    "Admin Block", "Parking Lot", "Garden Park", "Bus Stop",
    "Cafeteria", "Research Block", "Old Boys Hostel",
    "Student Activity Center", "Lecture Hall B", "Workshop"
]

# Known invalid nodes for DSU-negative tests
INVALID_NODES = [
    "Old Campus Gate", "Old Library", "Metro Station",
    "Unknown Place", "Fake Building", "Ghost Block"
]


class TestCase:
    def __init__(self, name: str, choice: str, locations: List[str],
                 expected_min_time: float, expected_max_time: float,
                 description: str = "", expect_dsu_fail: bool = False):
        self.name = name
        self.choice = choice
        self.locations = locations
        self.expected_min_time = expected_min_time
        self.expected_max_time = expected_max_time
        self.description = description
        self.expect_dsu_fail = expect_dsu_fail

    def get_input(self) -> str:
        inp = f"{self.choice}\n"
        inp += f"{len(self.locations)}\n"
        for loc in self.locations:
            inp += loc + "\n"
        inp += "3\n"  # exit
        return inp


class TestResult:
    def __init__(self, test_case: TestCase, passed: bool,
                 actual_time: Optional[float], actual_stops: Optional[int],
                 stdout: str = "", stderr: str = "", error_msg: str = ""):
        self.test_case = test_case
        self.passed = passed
        self.actual_time = actual_time
        self.actual_stops = actual_stops
        self.stdout = stdout
        self.stderr = stderr
        self.error_msg = error_msg


class RouteOptimizerTester:
    def __init__(self, executable_path: str = "./optimizer.exe", timeout_s: int = 30):
        self.executable = executable_path
        self.timeout = timeout_s
        self.test_cases: List[TestCase] = []
        self.results: List[TestResult] = []
        self._load_100_tests()

    # -------------------------
    # Build deterministic 100 tests
    # -------------------------
    def _load_100_tests(self):
        self.test_cases = []
        idx = 1

        def add(name_suffix: str, choice: str, locs: List[str], lo: float, hi: float, desc: str, dsu_fail: bool=False):
            nonlocal idx
            name = f"TC{idx:03d} {name_suffix}"
            self.test_cases.append(TestCase(name, choice, locs, lo, hi, desc, dsu_fail))
            idx += 1

        # Core deterministic tests (based on earlier suite)
        add("Small Fixed Order (Dijkstra)", "2", ["Main Gate", "CSE Building", "Library"], 5.0, 5.0, "Basic Dijkstra")
        add("Small TSP (Flexible)", "1", ["Main Gate", "Dining Hall", "Library", "Hostel A"], 12.0, 18.0, "TSP small")
        add("Adjacent Locations (Fixed)", "2", ["Garden Park", "Bus Stop"], 2.0, 2.0, "Direct edge")
        add("Distant Locations TSP", "1", ["Main Gate", "Innovation Center", "Research Block", "Sports Complex", "Student Activity Center"], 15.0, 40.0, "Large TSP")
        add("Star Pattern Fixed", "2", ["CSE Building", "Library", "Lecture Hall Complex", "CSE Building"], 6.0, 6.0, "Hub traversal")
        add("Medium TSP (10)", "1", ["Main Gate","Admin Block","Parking Lot","CSE Building","Library","Dining Hall","Medical Center","Canteen","Lecture Hall Complex","Innovation Center"], 20.0, 80.0, "10-node TSP")
        add("Single Location", "1", ["Main Gate"], 0.0, 0.0, "Single")
        add("Two Locations Fixed", "2", ["Library","Lecture Hall Complex"], 2.0, 2.0, "Direct 2")
        add("Two Locations Flexible", "1", ["Library","Lecture Hall Complex"], 2.0, 2.0, "2-node TSP")
        add("Circular Fixed Route", "2", ["Main Gate","CSE Building","Library","Dining Hall","Hostel A","Main Gate"], 17.0, 17.0, "Cycle")
        add("Large TSP Greedy", "1", ["Main Gate","Admin Block","CSE Building","Library","Dining Hall","Canteen","Medical Center","Sports Complex","Student Activity Center","Old Boys Hostel","Lecture Hall Complex","Lecture Hall B","Workshop","Research Block","Innovation Center"], 40.0, 150.0, "Greedy >15")
        add("Academic Buildings Tour", "1", ["CSE Building","Library","Lecture Hall Complex","Lecture Hall B","Research Block","Innovation Center"], 10.0, 40.0, "Academic cluster")
        add("Food Places Tour", "1", ["Dining Hall","Canteen","Cafeteria"], 25.0, 40.0, "Food cluster")

        # DSU tests (deterministic)
        add("DSU-T1 Connected Nodes", "2", ["Main Gate","Library"], 1.0, 100.0, "Connected", dsu_fail=False)
        add("DSU-T2 IITJ vs Old Campus", "2", ["Main Gate","Old Campus Gate"], 0.0, 0.0, "Old Campus should be rejected", dsu_fail=True)
        add("DSU-T3 Mixed Nodes", "1", ["Library","Old Library","Sports Complex"], 0.0, 0.0, "Mixed invalid", dsu_fail=True)
        add("DSU-T4 Single Old Campus Node", "1", ["Old Campus Gate"], 0.0, 0.0, "Single invalid allowed", dsu_fail=False)

        # deterministic reverse-pairs (10)
        for i in range(10):
            a = VALID_NODES[i % len(VALID_NODES)]
            b = VALID_NODES[(i+3) % len(VALID_NODES)]
            add(f"Reverse pair {i+1}", "2", [b, a], 1.0, 100.0, "Reverse Dijkstra")

        # duplicate-name tests (5)
        for i in range(5):
            n = VALID_NODES[i % len(VALID_NODES)]
            add(f"Duplicate names {i+1}", "1", [n, n, n], 0.0, 10.0, "Duplicates")

        # long TSP tests (10) - deterministic samples
        long_samples = [
            VALID_NODES[i:i+15] if i+15<=len(VALID_NODES) else (VALID_NODES[i:]+VALID_NODES[:(i+15)%len(VALID_NODES)])
            for i in range(10)
        ]
        for i, sample in enumerate(long_samples):
            add(f"Long TSP {i+1}", "1", sample, 30.0, 200.0, "Long TSP")

        # random Dijkstra pairs (10 deterministic picks)
        for i in range(10):
            a = VALID_NODES[(i*2) % len(VALID_NODES)]
            b = VALID_NODES[(i*2+5) % len(VALID_NODES)]
            add(f"Pair Dijkstra {i+1}", "2", [a, b], 1.0, 100.0, "Random Dijkstra pair")

        # Mixed valid/invalid pairs (10)
        for i in range(10):
            a = VALID_NODES[i % len(VALID_NODES)]
            b = INVALID_NODES[i % len(INVALID_NODES)]
            add(f"Mixed invalid {i+1}", "2", [a, b], 0.0, 0.0, "Mixed invalid", dsu_fail=True)

        # Single-node tests (10 deterministic picks)
        for i in range(10):
            a = VALID_NODES[(i*3) % len(VALID_NODES)]
            add(f"Single node {i+1}", "1", [a], 0.0, 0.0, "Single node")

        # Random small TSPs (fill up to 100)
        while len(self.test_cases) < 100:
            k = 3 + (len(self.test_cases) % 4)  # 3..6 small variety
            # deterministic deterministic sample using index
            start_idx = (len(self.test_cases) * 7) % len(VALID_NODES)
            sample = VALID_NODES[start_idx:start_idx + k]
            if len(sample) < k:
                sample = sample + VALID_NODES[:k - len(sample)]
            add(f"Small TSP fill {len(self.test_cases)+1}", "1", sample, 5.0, 100.0, "Filler small TSP")

        # final assert
        if len(self.test_cases) != 100:
            raise RuntimeError("Failed to build exactly 100 tests; built: {}".format(len(self.test_cases)))

    # -------------------------
    # Parse output
    # -------------------------
    def _parse_output(self, output: str) -> Tuple[Optional[float], Optional[int]]:
        if not output:
            return None, None
        if "not reachable" in output.lower():
            return -1, 0
        time_match = re.search(r"Total Time:\s*([0-9.]+)", output)
        stops_match = re.search(r"Stops:\s*(\d+)", output)
        total_time = float(time_match.group(1)) if time_match else None
        stops = int(stops_match.group(1)) if stops_match else None
        return total_time, stops

    # -------------------------
    # Run a single test
    # -------------------------
    def run_test(self, test: TestCase) -> TestResult:
        print(f"\n{Color.BOLD}{Color.OKBLUE}Running: {test.name}{Color.ENDC}")
        print(f"{Color.OKCYAN}{test.description}{Color.ENDC}")

        if not os.path.exists(self.executable):
            return TestResult(test, False, None, None, "", "", "Executable not found")

        try:
            proc = subprocess.Popen(
                [self.executable],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )
            stdout, stderr = proc.communicate(input=test.get_input(), timeout=self.timeout)

            total_time, stops = self._parse_output(stdout)

            # DSU rejection handling
            if total_time == -1:
                if test.expect_dsu_fail:
                    print(f"{Color.OKGREEN}✓ DSU rejection OK{Color.ENDC}")
                    return TestResult(test, True, None, None, stdout, stderr)
                else:
                    print(f"{Color.FAIL}✗ Unexpected DSU rejection{Color.ENDC}")
                    return TestResult(test, False, None, None, stdout, stderr, "Unexpected DSU rejection")

            if total_time is None:
                print(f"{Color.FAIL}✗ Output parse failed{Color.ENDC}")
                return TestResult(test, False, None, None, stdout, stderr, "Parse failed")

            passed = (test.expected_min_time <= total_time <= test.expected_max_time)
            if passed:
                print(f"{Color.OKGREEN}✓ PASSED{Color.ENDC}")
            else:
                print(f"{Color.FAIL}✗ FAILED (time {total_time}){Color.ENDC}")

            return TestResult(test, passed, total_time, stops, stdout, stderr)

        except subprocess.TimeoutExpired:
            print(f"{Color.FAIL}✗ TIMEOUT{Color.ENDC}")
            return TestResult(test, False, None, None, "", "", "Timeout")

    # -------------------------
    # Run all tests
    # -------------------------
    def run_all_tests(self, save_failures: bool = True, failures_dir: str = "fail_logs"):
        print(f"\n{Color.HEADER}{Color.BOLD}{'='*60}{Color.ENDC}")
        print(f"{Color.HEADER}{Color.BOLD} IIT JODHPUR ROUTE OPTIMIZER - 100 TEST SUITE {Color.ENDC}")
        print(f"{Color.HEADER}{Color.BOLD}{'='*60}{Color.ENDC}")

        if save_failures:
            os.makedirs(failures_dir, exist_ok=True)

        executed = 0
        for test in self.test_cases:
            res = self.run_test(test)
            self.results.append(res)
            executed += 1
            # Save failing stdout/stderr for inspection
            if save_failures and (not res.passed):
                fname = os.path.join(failures_dir, f"{test.name.replace(' ', '_')}.log")
                with open(fname, "w", encoding="utf-8") as fh:
                    fh.write("=== STDOUT ===\n")
                    fh.write((res.stdout or "") + "\n")
                    fh.write("=== STDERR ===\n")
                    fh.write((res.stderr or "") + "\n")
                    fh.write("=== META ===\n")
                    fh.write(json.dumps({
                        "name": test.name,
                        "expected_min": test.expected_min_time,
                        "expected_max": test.expected_max_time,
                        "dsu_fail_expected": test.expect_dsu_fail,
                        "error": res.error_msg
                    }, indent=2))
        self._print_summary(executed)

    # -------------------------
    # Summary
    # -------------------------
    def _print_summary(self, executed_count: int):
        passed = sum(1 for r in self.results if r.passed)
        failed = executed_count - passed
        print(f"\n{Color.BOLD}{Color.HEADER}{'='*60}{Color.ENDC}")
        print(f"{Color.BOLD}{Color.HEADER} TEST SUMMARY {Color.ENDC}")
        print(f"{Color.BOLD}{Color.HEADER}{'='*60}{Color.ENDC}")
        print(f"Total Tests Built: {len(self.test_cases)}")
        print(f"Total Tests Executed: {executed_count}")
        print(f"{Color.OKGREEN}Passed: {passed}{Color.ENDC}")
        print(f"{Color.FAIL}Failed: {failed}{Color.ENDC}")

        if failed:
            print(f"\n{Color.WARNING}Failed Cases:{Color.ENDC}")
            for r in self.results:
                if not r.passed:
                    print(f" - {r.test_case.name}: {r.error_msg or 'Failed'}")

def main():
    exe = "./optimizer.exe"
    if len(sys.argv) > 1:
        exe = sys.argv[1]
    tester = RouteOptimizerTester(executable_path=exe, timeout_s=30)
    tester.run_all_tests(save_failures=True, failures_dir="fail_logs")

if __name__ == "__main__":
    main()
