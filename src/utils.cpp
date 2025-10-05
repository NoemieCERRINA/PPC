#include "utils.h"

CSP Queens_generator(int n)
{
    // EN COURS DE DEVELOPPEMENT
    vector<int> domaine = {};

    // Domaines des variables, correspond a la colonne de la reine dans la ligne i
    for (int i = 0; i < n; i++)
    {
        domaine.push_back(i);
    }
    vector<vector<int>> Domaines;
    Domaines.assign(n, domaine);

    vector<Constraint> Constraints = build_alldif(n, Domaines);

    return CSP(n, Domaines, Constraints);
}

vector<Constraint> build_alldif(int nVar, vector<vector<int>> Domaines)
{
    vector<Constraint> Constraints;
    Constraint c;
    vector<pair<int, int>> c_extension;

    for (int i = 0; i < nVar; i++)
    {
        for (int j = i + 1; j < nVar; j++)
        {
            for (int vXi : Domaines[i])
            {
                for (int vXj : Domaines[j])
                {
                    if (vXi != vXj)
                    {
                        c_extension.emplace_back(vXi, vXj);
                    }
                }
            }
            Constraints.emplace_back(i, j, c_extension);
        }
    }
    return Constraints;
}