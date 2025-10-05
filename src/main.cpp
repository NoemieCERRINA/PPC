#include <iostream>
#include <string.h>
#include "Constraint.h"
#include "CSP.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <instance_file>" << endl;
        return 1;
    }

    string filename = argv[1];

    CSP csp = CSP(filename);
    csp.print();

    vector<int> ordre_initial = {};
    for (int i = 0; i < csp.getnVar(); i++)
    {
        ordre_initial.emplace_back(i);
    }

    auto result = csp.backtrack({}, ordre_initial);

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