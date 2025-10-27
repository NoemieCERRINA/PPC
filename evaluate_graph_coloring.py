import subprocess
import re
import os
import json
import csv
import time
import glob

import matplotlib.pyplot as plt
import numpy as np
from ortools.sat.python import cp_model

def solve_with_or_tools(filename, timeout=30):
    with open(filename, "r") as f:
        data = json.load(f)

    n = data["nVar"]
    model = cp_model.CpModel()
    
    X = [None] * n

    for domaine_info in data["Domaines"]:
        vars_in_group = domaine_info["vars"]
        values = domaine_info["values"]

        min_val, max_val = min(values), max(values)
        
        if set(values) == set(range(min_val, max_val + 1)):
            for var_idx in vars_in_group:
                X[var_idx] = model.NewIntVar(min_val, max_val, f"x{var_idx}")
        else:
            for var_idx in vars_in_group:
                X[var_idx] = model.NewIntVarFromDomain(
                    cp_model.Domain.FromValues(values),
                    f"x{var_idx}"
                )

    for constraint in data["Constraints"]:
        vars_pair = constraint["vars"][0]
        allowed_tuples = constraint["allowed"]
        model.AddAllowedAssignments([X[vars_pair[0]], X[vars_pair[1]]], allowed_tuples)

    model.AddDecisionStrategy(
        X,
        var_strategy=cp_model.CHOOSE_FIRST,
        domain_strategy=cp_model.CHOOSE_FIRST,
    )

    solver = cp_model.CpSolver()
    solver.parameters.max_time_in_seconds = timeout

    status = solver.Solve(model)

    if status in (cp_model.OPTIMAL, cp_model.FEASIBLE):
        solution = [solver.Value(x) for x in X]
        return {
            "status": "FEASIBLE" if status == cp_model.FEASIBLE else "OPTIMAL",
            "solution": solution,
            "time": solver.WallTime()
        }
    else:
        return {"status": "INFEASIBLE", "solution": None, "time": solver.WallTime()}

def plot_results(results, file_output="output.png", timeout_seconds=30):
    plt.figure(figsize=(10, 6))
    time_thresholds = np.linspace(0, timeout_seconds, 100)

    for algo, data in results.items():
        solve_times = np.array([t for t in data["solve_time"] if t >= 0])
        solved_counts = []

        for t in time_thresholds:
            solved_counts.append(np.sum(solve_times <= t))

        plt.plot(time_thresholds, solved_counts, label=algo, linewidth=2)

    plt.xlabel("Temps (secondes)")
    plt.ylabel("Nombre d’instances résolues")
    plt.title("Nombre d’instances résolues en fonction du temps")
    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
    plt.grid(True, linestyle="--", alpha=0.6)
    plt.tight_layout()
    
    plt.savefig(file_output, dpi=300, bbox_inches='tight')
    plt.close()

def save_results_to_csv(results, filename, instances):
    header = ["Instance"]
    algorithms = list(results.keys())
    
    for algo in algorithms:
        header.append(f"{algo}_solve_time")
        header.append(f"{algo}_total_time")

    with open(filename, mode="w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(header)

        for i, instance in enumerate(instances):
            row = [os.path.basename(instance)]
            for algo in algorithms:
                solve_times = results[algo]["solve_time"]
                total_times = results[algo]["total_time"]
                row.append(solve_times[i] if i < len(solve_times) else "-1")
                row.append(total_times[i] if i < len(total_times) else "-1")
            writer.writerow(row)

def main():
    executable = "./solver"
    instance_path = "instances/test/graph_coloring/*.json"  # Path to your graph coloring instances
    algorithms = ["FC", "MAC4"]  # Algorithms to test
    heuristics = [0, 1, 2, 3]    # Variable ordering heuristics to compare
    timeout_seconds = 10
    image_output = "Rapport/Images/graph_coloring3.pdf"
    data_output = "Rapport/Data/graph_coloring3.csv"
    results = {}

    # Get list of instance files
    instance_files = sorted(glob.glob(instance_path))
    if not instance_files:
        print(f"No instance files found in {instance_path}")
        return

    # iterate algorithms x heuristics
    for algorithm in algorithms:
        for heuristic in heuristics:
            fail_count = 0
            key = f"{algorithm}_h{heuristic}"
            algo_results = {"solve_time": [], "total_time": []}

            for instance_file in instance_files:
                instance_name = os.path.basename(instance_file)
                try:
                    start = time.time()
                    result = subprocess.run(
                        [executable, instance_file, algorithm, str(heuristic)],
                        capture_output=True,
                        text=True,
                        timeout=timeout_seconds
                    )
                    elapsed = time.time() - start
                    algo_results["total_time"].append(elapsed)

                    match = re.search(r"Temps de resolution:\s*([\d.]+)\s*secondes", result.stdout)
                    if match:
                        solve_time = float(match.group(1))
                        print(f"{instance_name} - algo: {algorithm} (h={heuristic}) - solving time: {solve_time}")
                        algo_results["solve_time"].append(solve_time)
                    else:
                        print(f"Solving time not found for {instance_name}, algo {algorithm} h={heuristic}")
                        algo_results["solve_time"].append(-1)

                except subprocess.TimeoutExpired:
                    print(f"Execution for {instance_name} exceeded {timeout_seconds} seconds (algo {algorithm} h={heuristic})")
                    algo_results["solve_time"].append(-1)
                    algo_results["total_time"].append(timeout_seconds)
                    fail_count += 1
                    if fail_count == 10:
                        break
            results[key] = algo_results
    

    # Generate plots and CSV
    plot_results(results, image_output, timeout_seconds)
    save_results_to_csv(results, data_output, instance_files)

    return results

if __name__ == '__main__':
    main()