#include <iostream>
#include <string.h>
#include "Constraint.h"
#include "CSP.h"

int main(int argc, char *argv[])
{
    vector<Constraint> constraints;
    constraints.push_back(Constraint(0, 1, {pair<int, int>(1, 1)})); // Initialiser variables à 0 pour simplifier indexation
    CSP Queen = CSP(2, {{1, 2}, {1, 2}}, constraints);
    Queen.print();

    vector<int> D1 = {1, 2};
    vector<int> D2 = {1, 2};
    cout << constraints[0].verifie(1,1) << "    " << constraints[0].verifie(2,1) << "\n";
    cout << constraints[0].supporte(1,D2.begin(),D2.end()) << "    " << constraints[0].supporte(2,D2.begin(),D2.end()) << "\n";
    cout << constraints[0].arc_consistant(D1.begin(),D1.end(),D2.begin(),D2.end()) << "    " << constraints[0].arc_consistant(D1.begin()+1,D1.end(),D2.begin(),D2.end()) << "\n";
    
    auto result = Queen.backtrack({},{0,1});
    cout << result.first << "\n";
    for (int v : result.second)
        cout << v << " ";

    return 0;
}