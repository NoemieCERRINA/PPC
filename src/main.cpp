#include <iostream>
#include <string.h>
#include "Constraint.h"
#include "CSP.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <instance_file>" << endl;
        return 1;
    }

    string filename = argv[1];

    CSP csp;
    // csp = CSP(filename);
    int n = 15;
    csp = Queens_generator(n);
    //vector<int> domain_last_elts = csp.AC4();
    csp.print();
    // for (int elt : domain_last_elts)
    //    cout << elt << ", ";

    auto result = csp.MAC4();

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