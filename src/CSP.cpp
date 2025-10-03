#include "CSP.h"

CSP::CSP(int n, vector<vector<int>> D, vector<Constraint> C)
{
    nVar = n;
    Domaines = D;
    Constraints = C;

    constraintMatrix.assign(nVar, vector<Constraint*>(nVar, nullptr));
    for (auto &c : Constraints) {
        int i = c.getX1();
        int j = c.getX2();
        constraintMatrix[i][j] = &c;
    }
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

pair<bool, vector<int>> CSP::backtrack(vector<int> instantiation_partielle, vector<int> ordre_variables)
{
    int var_introduite = -1;
    int val_introduite = -1;

    if (instantiation_partielle.size() != 0)
        {
            var_introduite = ordre_variables[instantiation_partielle.size() - 1]; 
            val_introduite = instantiation_partielle[instantiation_partielle.size() - 1];
        }

    for (size_t i = 0; i < instantiation_partielle.size() - 1; i++)
    {
        int var_comparaison = ordre_variables[i];
        int val_comparaison = instantiation_partielle[i];

        Constraint* contrainte1 = constraintMatrix[var_introduite][var_comparaison];
        if (contrainte1 != nullptr && !(contrainte1->verifie(val_introduite, val_comparaison)))
            return {false, {}};

        // QUESTION - Est-ce qu'il faut garder? Contraintes directionnelles/symétriques ou non?
        Constraint* contrainte2 = constraintMatrix[var_comparaison][var_introduite];
        if (contrainte2 != nullptr && !(contrainte2->verifie(val_comparaison, val_introduite)))
            return {false, {}};
    }

    if (instantiation_partielle.size() == ordre_variables.size())
        return {true,instantiation_partielle};

    int nouvelle_var = ordre_variables[instantiation_partielle.size()];
    for (int val : Domaines[nouvelle_var])
    {
        vector<int> nv_instantiation_partielle = instantiation_partielle;
        nv_instantiation_partielle.push_back(val);

        auto return_backtrack = backtrack(nv_instantiation_partielle, ordre_variables);

        if (return_backtrack.first)
            return return_backtrack;
    }

    return {false, {}};
}

