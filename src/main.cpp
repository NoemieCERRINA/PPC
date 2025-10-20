#include <iostream>
#include <chrono>
#include <string.h>
#include "Constraint.h"
#include "CSP.h"
#include "utils.h"

using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cerr << "[ERR] - Usage: " << argv[0] << " <instance_file> <solving_algorithm>" << endl;
        return 1;
    }

    string filename = argv[1];
    string algo = argv[2];

    CSP csp;
    csp = CSP(filename);
    int n = 16;
    // csp = Queens_generator(n);
    // vector<int> domain_last_elts = csp.AC4();
    csp.print();
    // for (int elt : domain_last_elts)
    //    cout << elt << ", ";

    pair<bool, vector<int>> result;
    auto start_timer = high_resolution_clock::now();

    if (algo == "backtrack") result = csp.backtrack();
    else if (algo == "FC") result = csp.fullFC();
    else if (algo == "MAC3") result = csp.MAC(&CSP::AC3);
    else if (algo == "MAC4") result = csp.MAC(&CSP::AC4);
    else
    {
        cerr << "[ERR] - Unknown algorithm: " << algo << ". Pick a value among [backtrack, FC, MAC3, MAC4]" << endl;
        return 1;
    }
    auto stop_timer = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop_timer - start_timer);
    cout << "Temps de resolution: " << duration.count() / 1000.0 << " secondes" << endl;

    cout << "Resultat: " << result.first << "\n";
    if (result.first)
    {
        cout << "Instance solution: ";
        for (int v : result.second)
            cout << v << " ";
        cout << endl;
    }

    csp.generate_json_instance("instances/test.json");

    return 0;
}