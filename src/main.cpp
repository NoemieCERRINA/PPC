#include <iostream>
#include <string.h>
#include "Constraint.h"
#include "CSP.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <instance_file>" << endl;
        return 1;
    }

    string filename = argv[1];

    CSP csp = CSP(filename);
    csp.print();

    auto result = csp.backtrack({},{0,1});
    cout << result.first << "\n";
    for (int v : result.second)
        cout << v << " ";

    return 0;
}