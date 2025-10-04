#include "CSP.h"

CSP::CSP(int n, vector<vector<int>> D, vector<Constraint> C)
{
    // Constructeur à partir de variables

    nVar = n;
    Domaines = D;
    Constraints = C;

    constraintMatrix.assign(nVar, vector<Constraint*>(nVar, nullptr));
    for (auto &c : C) {
        int i = c.getX1();
        int j = c.getX2();
        constraintMatrix[i][j] = &c;
    }
}

CSP::CSP(const std::string& filename)
{
    // Constructeur à partir d'un fichier

    ifstream file(filename);
    json instance;
    file >> instance;

    nVar = instance["nVar"].get<int>();

    Domaines.assign(nVar, {});

    for (const auto& group : instance["Domaines"])
    {
        vector<int> vars = group["vars"].get<vector<int>>();
        vector<int> values = group["values"].get<vector<int>>();

        for (int var : vars)
            Domaines[var] = values;
    }

    constraintMatrix.assign(nVar, vector<Constraint*>(nVar, nullptr));

    for (const auto& group : instance["Constraints"])
    {
        vector<vector<int>> vars_list = group["vars"].get<vector<vector<int>>>();
        vector<vector<int>> vals_list = group["allowed"].get<vector<vector<int>>>();

        vector<pair<int,int>> allowed;
        for (const auto& p : vals_list)
            allowed.emplace_back(p[0], p[1]);

        for (const auto& var_pair : vars_list)
        {
            int x1 = var_pair[0];
            int x2 = var_pair[1];

            Constraint c(x1, x2, allowed);
            Constraints.push_back(c);
            constraintMatrix[x1][x2] = &Constraints.back();
        }
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

vector<int> CSP::reorder(vector<int> list, vector<int> order)
{
    // Fonction auxiliaire pour trier une liste d'entier en fonction d'un ordre donné
    
    vector<int> reordered(order.size());
    for (size_t i = 0; i < order.size(); i++) {
        reordered[order[i]] = list[i];
    }
    return reordered;
}

pair<bool, vector<int>> CSP::backtrack(vector<int> instantiation_partielle, vector<int> ordre_variables)
{

    // On récupère la dernière variable introduite et sa valeur
    int var_introduite = -1;
    int val_introduite = -1;

    if (instantiation_partielle.size() != 0)
        {
            var_introduite = ordre_variables[instantiation_partielle.size() - 1]; 
            val_introduite = instantiation_partielle[instantiation_partielle.size() - 1];
        }

    // On vérifie que la variable que l'on vient d'introduire ne viole aucune contrainte avec les autres variables déjà instanciée
    for (size_t i = 0; i + 1 < instantiation_partielle.size(); i++)
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

    // Si on a instancié toutes les variables, alors cette instantiation est valide
    if (instantiation_partielle.size() == ordre_variables.size())
        return {true,reorder(instantiation_partielle,ordre_variables)};

    // Sinon, on instancie récursivement une nouvelle variable dans l'ordre spécifiée avec chacune de ses valeurs possibles successivement
    int nouvelle_var = ordre_variables[instantiation_partielle.size()];
    for (int val : Domaines[nouvelle_var])
    {
        vector<int> nv_instantiation_partielle = instantiation_partielle;
        nv_instantiation_partielle.push_back(val);

        auto return_backtrack = backtrack(nv_instantiation_partielle, ordre_variables);

        if (return_backtrack.first)
            return return_backtrack;
    }

    // Si toutes les instantiations précédentes aboutissent à une contradiction, alors on backtrack
    return {false, {}};
}

