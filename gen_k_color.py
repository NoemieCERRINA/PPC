import sys
import json

def main():

    if len(sys.argv) > 2:
        filename = sys.argv[1]
        k = int(sys.argv[2])
    
    with open(filename, 'r') as f:
        lines = [line.strip() for line in f if line.strip()]

    nVar = 1
    edges = []
    
    for line in lines:
        
        if line[0] == "p":
            vals = line.split()
            nVar = int(vals[2])
        
        if line[0] == "e":
            vals = line.split()
            edges.append([int(vals[1])-1, int(vals[2])-1])
    
    allowed = []
    for i in range(k):
        for j in range(k):
            if i != j:
                allowed.append([i,j])

    dict_json = {
        "nVar": nVar,
        "Domaines": [{"vars": list(range(nVar)), "values": list(range(k))}],
        "Constraints": [{"vars": edges, "allowed": allowed}]
        }

    with open(f"instances/graph_coloring/{k}_{filename}.json", "w") as f:
        json.dump(dict_json, f, separators=(",", ":"), ensure_ascii=False)

if __name__ == '__main__':
    main()