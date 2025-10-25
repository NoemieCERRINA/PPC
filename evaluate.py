import subprocess
import re
import sys
import json
import os
import time

def gen_n_queens(n):
    
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

def main():

    executable = "./solver"
    instance_path = "instances/n-queens/"
    algorithms = ["backtrack", "FC", "MAC3", "MAC4"]
    timeout_seconds = 30

    instance_files = []

    for i in range(1,30):
            # instance_file = f"{instance_path}{i}-queens.json"
            instance_files.append(gen_n_queens(i))

    for algorithm in algorithms:
        
        for i,instance_file in enumerate(instance_files):

            try:
                start = time.time()
                result = subprocess.run(
                    [executable, instance_file, algorithm],
                    capture_output=True,
                    text=True,
                    timeout=timeout_seconds
                )
                total_time = time.time() - start
                match = re.search(r"Temps de resolution:\s*([\d.]+)\s*secondes", result.stdout)
                if match:
                    solve_time = float(match.group(1))
                    print(f"{i}-reines - algo : {algorithm} - temps de résolution : {solve_time} - temps total : {total_time:.3f}")
                else:
                    print("Temps de résolution introuvable.")

            except subprocess.TimeoutExpired:
                print(f"L'exécution pour {i}-reines a dépassé {timeout_seconds} secondes et a été arrêtée")
                break

if __name__ == '__main__':
    main()