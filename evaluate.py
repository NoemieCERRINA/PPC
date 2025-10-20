import subprocess
import re

def main():

    executable = "./solver"
    instance_path = "instances/n-queens/"
    algorithms = ["backtrack", "FC", "MAC3", "MAC4"]
    timeout_seconds = 30

    for algorithm in algorithms:
        
        for i in range(1,30):
            instance_file = f"{instance_path}{i}-queens.json"

            try:
                result = subprocess.run(
                    [executable, instance_file, algorithm],
                    capture_output=True,
                    text=True,
                    timeout=timeout_seconds
                )
                match = re.search(r"Temps de resolution:\s*([\d.]+)\s*secondes", result.stdout)
                if match:
                    solve_time = float(match.group(1))
                    print(f"{i}-reines - algo : {algorithm} - temps de résolution : {solve_time}")
                else:
                    print("Temps de résolution introuvable.")

            except subprocess.TimeoutExpired:
                print(f"L'exécution pour {i}-reines a dépassé {timeout_seconds} secondes et a été arrêtée")
                break

if __name__ == '__main__':
    main()