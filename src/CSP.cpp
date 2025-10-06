#include "CSP.h"

CSP::CSP()
{
    nVar = 0;
    Domaines = {};
    Constraints = {};
}

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
    int i = 0;
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
    i = 0;
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
    // On récupère la dernière variable introduite et sa valeur
    int var_introduite = -1;
    int val_introduite = -1;

    if (instantiation_partielle.size() != 0)
    {
        var_introduite = ordre_variables[instantiation_partielle.size() - 1];
        val_introduite = instantiation_partielle[instantiation_partielle.size() - 1];
    }

    // On vérifie que la variable que l'on vient d'introduire ne viole aucune contrainte avec les autres variables déjà instanciées
    for (size_t i = 0; i + 1 < instantiation_partielle.size(); i++)
    {
        int var_comparaison = ordre_variables[i];
        int val_comparaison = instantiation_partielle[i];

        Constraint *contrainte1 = constraintMatrix[var_introduite][var_comparaison];
        if (contrainte1 != nullptr && !(contrainte1->verifie(val_introduite, val_comparaison)))
            return {false, {}};

        // QUESTION - Est-ce qu'il faut garder? Contraintes directionnelles/symétriques ou non? -> modif apportee a Constraint, sont construites telles que x1 < x2
        Constraint *contrainte2 = constraintMatrix[var_comparaison][var_introduite];
        if (contrainte2 != nullptr && !(contrainte2->verifie(val_comparaison, val_introduite)))
            return {false, {}};
    }

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

        if (return_backtrack.first)
            return return_backtrack;
    }

    // Si toutes les instantiations précédentes aboutissent à une contradiction, alors on backtrack
    return {false, {}};
}

vector<int> CSP::AC4(vector<int> domain_last_elts)
{
    // Remarques:
    // Q est implémenté comme une pile ici
    // J'ai un petit doute sur la correction de la méthode de retrait d'une valeur d'un domaine dans l'initialisation
    // La vérification que v1 appartient à d1 dans la phase de propagation est-elle nécessaire?

    if (domain_last_elts.empty())
    {
        for (const auto& domain : Domaines)
            domain_last_elts.push_back(static_cast<int>(domain.size()));
    }
    vector<int> new_domain_last_elts = domain_last_elts;

    // Phase d'initialisation
    map<tuple<int,int,int>, int> Count;
    map<pair<int,int>, vector<pair<int,int>>> S;
    vector<pair<int,int>> Q;

    for (const auto &c : Constraints)
    {
        int x1 = c.getX1();
        int x2 = c.getX2();
        auto &d1 = Domaines[x1];
        auto &d2 = Domaines[x2];

        // Sens direct
        for (int i = 0; i < new_domain_last_elts[x1]; i++)
        {
            int v1 = d1[i];
            int total = 0;
            for (int j = 0; j < new_domain_last_elts[x2]; j++)
            {
                int v2 = d2[j];
                if (c.verifie(v1,v2))
                {
                    total++;
                    S[{x2,v2}].push_back({x1,v1});
                }
            }
            Count[{x1,x2,v1}] = total;
            if (total == 0)
            {
                swap(d1[i], d1[new_domain_last_elts[x1] - 1]);
                new_domain_last_elts[x1]--;
                i--;
                Q.push_back({x1,v1});
            }
        }

        // Sens indirect
        for (int i = 0; i < new_domain_last_elts[x2]; i++)
        {
            int v2 = d2[i];
            int total = 0;
            for (int j = 0; j < new_domain_last_elts[x1]; j++)
            {
                int v1 = d1[j];
                if (c.verifie(v1,v2))
                {
                    total++;
                    S[{x1,v1}].push_back({x2,v2});
                }
            }
            Count[{x2,x1,v2}] = total;
            if (total == 0)
            {
                swap(d2[i], d2[new_domain_last_elts[x2] - 1]);
                new_domain_last_elts[x2]--;
                i--;
                Q.push_back({x2,v2});
            }
        }
    }

    // Phase de propagation
    while (Q.size() > 0)
    {
        int x2 = Q.back().first;
        int v2 = Q.back().second;
        Q.pop_back();

        for (auto &couple1 : S[{x2,v2}])
        {
            int x1 = couple1.first;
            int v1 = couple1.second;
            vector<int> &d1 = Domaines[x1];

            Count[{x1,x2,v1}]--;

            auto it = find(d1.begin(), d1.begin() + new_domain_last_elts[x1], v1);

            if (it != d1.begin() + new_domain_last_elts[x1])
            {
                auto index = distance(d1.begin(), it);
                swap(d1[index], d1[new_domain_last_elts[x1] - 1]);
                new_domain_last_elts[x1]--;
                Q.push_back({x1,v1});
            }
        }
    }

    return new_domain_last_elts;
}

void CSP::generate_json_instance(const std::string &filename)
{
    json j;

    j["nVar"] = nVar;
    j["Domaines"] = json::array();

    json d;
    for (int i = 0; i < nVar; i++)
    {
        d["vars"] = {i};
        d["values"] = Domaines[i];
        j["Domaines"].push_back(d);
    }

    j["Constraints"] = json::array();
    for (Constraint c : Constraints)
    {
        j["Constraints"].push_back(c.makeJson());
    }

    std::ofstream o(filename);
    o << j << std::endl;
}
