#include "CSP.h"

CSP::CSP(int n, vector<vector<int>> D, vector<Constraint> C)
{
    nVar = n;
    Domaines = D;
    Constraints = C;
}

void CSP::print()
{
    int i = 1;
    cout << "Nombre de variables: " << nVar << endl;
    cout << "Domaine des variables: " << endl;
    for (vector<int> domaine : Domaines)
    {
        cout << "V" + to_string(i) + ": ";
        for (int d : domaine)
        {
            cout << d << ", ";
        }
        cout << endl;
        i++;
    }
    cout << "Contraintes: " << endl;
    i = 1;
    for (Constraint contrainte : Constraints)
    {
        cout << "C" + to_string(i) + ": ";
        contrainte.print();
        i++;
    }
}