#include "CSP.h"

CSP::CSP(int n, vector<vector<int>> D, vector<Constraint> C)
{
    // Constructeur à partir de variables

    nVar = n;
    Domaines = D;
    Constraints = C;

    constraintMatrix.assign(nVar, vector<Constraint *>(nVar, nullptr));
    for (auto &c : Constraints)
    {
        int i = c.getX1();
        int j = c.getX2();
        constraintMatrix[i][j] = &c;
    }
}

CSP::CSP(const std::string &filename)
{
    // Constructeur à partir d'un fichier

    ifstream file(filename);
    json instance;
    file >> instance;

    nVar = instance["nVar"].get<int>();

    Domaines.assign(nVar, {});

    for (const auto &group : instance["Domaines"])
    {
        vector<int> vars = group["vars"].get<vector<int>>();
        vector<int> values = group["values"].get<vector<int>>();

        for (int var : vars)
            Domaines[var] = values;
    }

    for (const auto &group : instance["Constraints"])
    {
        vector<vector<int>> vars_list = group["vars"].get<vector<vector<int>>>();
        vector<vector<int>> vals_list = group["allowed"].get<vector<vector<int>>>();

        vector<pair<int, int>> allowed;
        for (const auto &p : vals_list)
            allowed.emplace_back(p[0], p[1]);

        for (const auto &var_pair : vars_list)
        {
            int x1 = var_pair[0];
            int x2 = var_pair[1];

            Constraint c(x1, x2, allowed);
            Constraints.push_back(c);
        }
    }

    constraintMatrix.assign(nVar, vector<Constraint *>(nVar, nullptr));
    for (auto &c : Constraints)
    {
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

int CSP::getnVar() const
{
    return nVar;
}

vector<int> CSP::reorder(vector<int> list, vector<int> order)
{
    // Fonction auxiliaire pour trier une liste d'entier en fonction d'un ordre donné

    vector<int> reordered(order.size());
    for (size_t i = 0; i < order.size(); i++)
    {
        reordered[order[i]] = list[i];
    }
    return reordered;
}

pair<bool, vector<int>> CSP::backtrack(vector<int> instantiation_partielle, vector<int> ordre_variables)
{
    cout << "instantiation actuelle: ";
    for (int i : instantiation_partielle)
    {
        cout << i << ", ";
    }
    cout << endl;

    // On récupère la dernière variable introduite et sa valeur
    int var_introduite = -1;
    int val_introduite = -1;

    if (instantiation_partielle.size() != 0)
    {
        var_introduite = ordre_variables[instantiation_partielle.size() - 1];
        val_introduite = instantiation_partielle[instantiation_partielle.size() - 1];
    }

    cout << "Backtrack, instanciation de la variable " << var_introduite << " a la valeur " << val_introduite << endl;

    // On vérifie que la variable que l'on vient d'introduire ne viole aucune contrainte avec les autres variables déjà instanciées
    for (size_t i = 0; i + 1 < instantiation_partielle.size(); i++)
    {
        cout << i << endl;
        int var_comparaison = ordre_variables[i];
        int val_comparaison = instantiation_partielle[i];

        cout << i + 10 << endl;
        Constraint *contrainte1 = constraintMatrix[var_introduite][var_comparaison];

        cout << i + 20 << endl;
        if (contrainte1 != nullptr && !(contrainte1->verifie(val_introduite, val_comparaison)))
            return {false, {}};

        cout << i + 30 << endl;
        // QUESTION - Est-ce qu'il faut garder? Contraintes directionnelles/symétriques ou non? -> modif apportee a Constraint, sont construites telles que x1 < x2
        Constraint *contrainte2 = constraintMatrix[var_comparaison][var_introduite];
        if (contrainte2 != nullptr && !(contrainte2->verifie(val_comparaison, val_introduite)))
            return {false, {}};
    }

    cout << "checkpoint 1" << endl;

    // Si on a instancié toutes les variables, alors cette instantiation est valide
    if (instantiation_partielle.size() == ordre_variables.size())
        return {true, reorder(instantiation_partielle, ordre_variables)};

    // Sinon, on instancie récursivement une nouvelle variable dans l'ordre spécifiée avec chacune de ses valeurs possibles successivement
    int nouvelle_var = ordre_variables[instantiation_partielle.size()];
    for (int val : Domaines[nouvelle_var])
    {
        vector<int> nv_instantiation_partielle = instantiation_partielle;
        nv_instantiation_partielle.push_back(val);

        auto return_backtrack = backtrack(nv_instantiation_partielle, ordre_variables);

        cout << "checkpoint 2" << endl;
        if (return_backtrack.first)
            return return_backtrack;
    }

    // Si toutes les instantiations précédentes aboutissent à une contradiction, alors on backtrack
    return {false, {}};
}

void CSP::generate_json_instance(const std::string &filename)
{
    json j;

    j["nVar"] = nVar;
    j["Domaines"] = Domaines;
    j["Constraints"] = json::array();
    for (Constraint c : Constraints)
    {
        j["Constraints"].push_back(c.makeJson());
    }

    /*
    json domaine;
    domaine["vars"] = {0, 1};
    domaine["values"] = {0, 1};
    j["Domaines"].push_back(domaine);

    j["Constraints"] = json::array();
    json constraint;
    constraint["vars"] = {{1, 0}};
    constraint["allowed"] = {{0, 1}, {1, 0}};
    j["Constraints"].push_back(constraint);
    */
    std::ofstream o(filename);
    o << std::setw(4) << j << std::endl;
}
