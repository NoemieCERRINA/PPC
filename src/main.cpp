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
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <instance_file>" << endl;
        return 1;
    }

    string filename = argv[1];

    CSP csp;
    csp = CSP(filename);
    int n = 16;
    // csp = Queens_generator(n);
    // vector<int> domain_last_elts = csp.AC4();
    csp.print();
    // for (int elt : domain_last_elts)
    //    cout << elt << ", ";

    auto start_timer = high_resolution_clock::now();

    // auto result = csp.MAC(&CSP::AC3);
    auto result = csp.fullFC();

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