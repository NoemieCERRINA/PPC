# ...existing code...
import subprocess
import re
import os
import json
import csv
import time

import matplotlib.pyplot as plt
import numpy as np
from ortools.sat.python import cp_model

def gen_n_queens(n):
    # ...existing code...
    filename = f"instances/test/{n}-queens.json"

    if not os.path.isdir("instances/test"):
        os.makedirs("instances/test")

    if not os.path.isfile(filename):

        edges = []
        
        for i in range(n):
            for j in range(i+1,n):
                edges.append([i,j])
        
        allowed = []

        for [i,j] in edges:
            new_edge = []
            for vXi in range(n):
                for vXj in range(n):
                    if (vXi != vXj and vXi + i != vXj + j and vXi - i != vXj - j):
                        new_edge.append([vXi,vXj])
            allowed.append(new_edge)

        dict_json = {
            "nVar": n,
            "Domaines": [{"vars": list(range(n)), "values": list(range(n))}],
            "Constraints": [{"vars": [edges[i]], "allowed": allowed[i]} for i in range(len(edges))]
            }

        with open(filename, "w") as f:
            json.dump(dict_json, f, separators=(",", ":"), ensure_ascii=False)

    return filename

def solve_with_or_tools(filename, timeout=30):
    # ...existing code...
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

# ...existing plotting / csv helpers unchanged...

def plot_cumulative_time(results, start_instance=1, end_instance=5):
    # ...existing code...
    plt.figure(figsize=(8, 6))

    for algo, data in results.items():
        solve_times = np.array(data["solve_time"])

        # On remplace les -1 (timeouts) par 0 pour éviter les ruptures de somme
        solve_times = np.where(solve_times < 0, 0, solve_times)

        instances = np.arange(start_instance, start_instance+len(solve_times))
        cumulative_time = np.cumsum(solve_times)

        plt.plot(instances, cumulative_time, marker="o", linewidth=2, label=algo)

    plt.xlabel("Taille du problème n (n-reines)")
    plt.ylabel("Temps d’exécution cumulé (secondes)")
    plt.title("Temps d’exécution cumulé pour n-reines")
    plt.legend()
    plt.grid(True, linestyle="--", alpha=0.6)
    plt.tight_layout()
    
    plt.savefig("output.png", dpi=300)
    plt.close()

def plot_results(results, file_output="output.png", timeout_seconds=30):
    # ...existing code...
    plt.figure(figsize=(8, 6))

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
    plt.legend()
    plt.grid(True, linestyle="--", alpha=0.6)
    plt.tight_layout()
    
    plt.savefig(file_output, dpi=300)
    plt.close()

def save_results_to_csv(results, filename, start_instance, end_instance):
    # ...existing code...
    algorithms = list(results.keys())
    n_instances = end_instance - start_instance

    header = ["Instance"]
    for algo in algorithms:
        header.append(f"{algo}_solve_time")
        header.append(f"{algo}_total_time")

    with open(filename, mode="w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(header)

        for i in range(n_instances):
            row = [start_instance+i]
            for algo in algorithms:
                solve_times = results[algo]["solve_time"]
                total_times = results[algo]["total_time"]
                row.append(solve_times[i] if i < len(solve_times) else "-1")
                row.append(total_times[i] if i < len(total_times) else "-1")
            writer.writerow(row)

def main():

    executable = "./solver"
    instance_path = "instances/test/"
    algorithms = ["FC"]
    heuristics = [0, 1, 2, 3]    # <-- add or adjust the list of heuristics to compare
    timeout_seconds = 30
    start_instance = 15
    end_instance = 50
    image_output = "Rapport/Images/n-queens4.pdf"
    data_output = "Rapport/Data/n-queens4.csv"
    results = {}

    # iterate algorithms x heuristics
    for algorithm in algorithms:
        for heuristic in heuristics:
            key = f"{algorithm}_h{heuristic}"
            algo_results = {"solve_time" : [], "total_time" : []}

            for i in range(start_instance, end_instance):
                instance_file = gen_n_queens(i)

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
                        print(f"{i}-reines - algo : {algorithm} (h={heuristic}) - temps de résolution : {solve_time}")
                        algo_results["solve_time"].append(solve_time)
                    else:
                        print(f"Temps de résolution introuvable for {i}-reines, algo {algorithm} h={heuristic}")
                        algo_results["solve_time"].append(-1)

                except subprocess.TimeoutExpired:
                    print(f"L'exécution pour {i}-reines a dépassé {timeout_seconds} secondes et a été arrêtée (algo {algorithm} h={heuristic})")
                    algo_results["solve_time"].append(-1)
                    algo_results["total_time"].append(timeout_seconds)
                    break

            results[key] = algo_results
    
    # CP-SAT baseline (unchanged) - keep one default heuristic
    algo_results = {"solve_time" : [], "total_time" : []}
    for i in range(start_instance, end_instance):
        instance_file = gen_n_queens(i)

        start = time.time()
        result = solve_with_or_tools(instance_file, timeout_seconds)
        end = time.time() - start
        algo_results["total_time"].append(end)

        if end > timeout_seconds:
            print(f"L'exécution pour {i}-reines a dépassé {timeout_seconds} secondes et a été arrêtée (CP-SAT)")
            break
        else:
            print(f"{i}-reines - algo : cp-sat - temps de résolution : {result['time']}")
            algo_results["solve_time"].append(result["time"])
    
    results["CP-SAT"] = algo_results
    print(results)

    # produce plots and CSV (will include algorithm_heuristic keys)
    plot_results(results, image_output, timeout_seconds=20)
    save_results_to_csv(results, data_output, start_instance, end_instance)

    return results



if __name__ == '__main__':
    main()