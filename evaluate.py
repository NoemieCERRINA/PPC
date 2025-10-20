import subprocess

executable = "./solver"
instance_file = "instances/example.json"
timeout_seconds = 30

try:
    result = subprocess.run(
        [executable, instance_file],
        capture_output=True,
        text=True,
        timeout=timeout_seconds
    )
    print("Output:\n", result.stdout)

except subprocess.TimeoutExpired:
    print(f"L'exécution a dépassé {timeout_seconds} secondes et a été arrêtée")
