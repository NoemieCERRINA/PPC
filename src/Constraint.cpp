#include "Constraint.h"

Constraint::Constraint(int vx1, int vx2, vector<pair<int, int>> vals)
{
    x1 = vx1;
    x2 = vx2;
    valeurs = vals;
}

Constraint::Constraint()
{
    x1 = 0;
    x2 = 0;
    valeurs = {};
}

void Constraint::print()
{
    cout << "Variables contraintes: " << x1 << " " << x2 << endl;
    cout << "Valeurs autorisees: ";
    for (pair<int, int> val : valeurs)
    {
        cout << val.first << ", " << val.second << "; ";
    }
    cout << endl;
}