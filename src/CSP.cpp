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

vector<int> CSP::getDomainSizes() const 
{
    vector<int> sizes;
    for (const auto& d : Domaines)
        sizes.push_back(static_cast<int>(d.size()));
    return sizes;
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

int CSP::selectNextVar(const vector<int>& remaining, const vector<int>& domain_last_elts, int heuristic)
{
    if (heuristic == 0) // IN_ORDER
        return remaining.front();
    else if (heuristic == 1) // SMALLEST_DOMAIN
    {
        return *min_element(remaining.begin(), remaining.end(),
                [&](int a, int b){ return domain_last_elts[a] < domain_last_elts[b]; });
    }
    else if (heuristic == 2) // LARGEST_DOMAIN
    {
        return *max_element(remaining.begin(), remaining.end(),
                [&](int a, int b){ return domain_last_elts[a] < domain_last_elts[b]; });
    }
    return remaining[rand() % remaining.size()]; //RANDOM
}


vector<int> CSP::AC3(vector<int> domain_last_elts, int x)
{
    vector<pair<int,int>> aTester;

    for (const auto &c : Constraints)
    {
        int x1 = c.getX1();
        int x2 = c.getX2();
        aTester.push_back({x1,x2});
        aTester.push_back({x2,x1});
    }

    while (!aTester.empty())
    {
        int x1 = aTester.back().first;
        int x2 = aTester.back().second;
        aTester.pop_back();

        bool update = false;

        Constraint *contrainte1 = constraintMatrix[x1][x2];
        if (contrainte1 != nullptr)
        {

            for (int i = 0; i < domain_last_elts[x1]; i++)
            {
                int v1 = Domaines[x1][i];
                bool supported = false;

                for (int j = 0; j < domain_last_elts[x2]; j++)
                {
                    int v2 = Domaines[x2][j];
                    if (contrainte1->verifie(v1, v2))
                    {
                        supported = true;
                        break;
                    }
                }

                if (!supported)
                {
                    swap(Domaines[x1][i], Domaines[x1][domain_last_elts[x1] - 1]);
                    domain_last_elts[x1]--;
                    i--;
                    update = true;
                }
            }
        }

        Constraint *contrainte2 = constraintMatrix[x2][x1];
        if (contrainte2 != nullptr)
        {
            for (int i = 0; i < domain_last_elts[x1]; i++)
            {
                int v1 = Domaines[x1][i];
                bool supported = false;

                for (int j = 0; j < domain_last_elts[x2]; j++)
                {
                    int v2 = Domaines[x2][j];
                    if (contrainte2->verifie(v2, v1))
                    {
                        supported = true;
                        break;
                    }
                }

                if (!supported)
                {
                    swap(Domaines[x1][i], Domaines[x1][domain_last_elts[x1] - 1]);
                    domain_last_elts[x1]--;
                    i--;
                    update = true;
                }
            }
        }

        if (update){
            for (int k = 0; k < nVar; k++)
            {
                Constraint *contrainte1 = constraintMatrix[x1][k];
                if (contrainte1 != nullptr)
                    aTester.push_back({k,x1});
                Constraint *contrainte2 = constraintMatrix[k][x1];
                if (contrainte2 != nullptr)
                    aTester.push_back({k,x1});
            }
        }
    }

    return domain_last_elts;
}

vector<int> CSP::AC4(vector<int> domain_last_elts, int x1)
{
    // Remarques:
    // Q est implémenté comme une pile ici
    // J'ai un petit doute sur la correction de la méthode de retrait d'une valeur d'un domaine dans l'initialisation
    // La vérification que v1 appartient à d1 dans la phase de propagation est-elle nécessaire?

    vector<int> new_domain_last_elts = domain_last_elts;

    /*cout << "Domain Last Elts : ";
    for (int i : domain_last_elts)
        cout << i << ", ";
    cout << "\n";

    cout << "Domaines : " << "\n";
    for (int i = 0; i < nVar; i++)
    {
        cout << "Domaine: " << i << " : ";
        for (int j : Domaines[i])
            cout << j << ", ";
        cout << "\n";
    }*/

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
                //cout << "Direct Support nul: <" << x1 << ", " << v1 << "> - " << x2 << "\n";
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
                //cout << "Indirect Support nul: <" << x2 << ", " << v2 << "> - " << x1 << "\n";
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

            if (Count[{x1,x2,v1}] == 0 && it != d1.begin() + new_domain_last_elts[x1])
            {
                //cout << "Propagate Support nul: <" << x1 << ", " << v1 << "> - <" << x2 << ", " << v2 << ">\n";
                auto index = distance(d1.begin(), it);
                swap(d1[index], d1[new_domain_last_elts[x1] - 1]);
                new_domain_last_elts[x1]--;
                Q.push_back({x1,v1});
            }
        }
    }

    return new_domain_last_elts;
}

vector<int> CSP::FC(vector<int> domain_last_elts, int x1)
{
    if (x1 < 0)
        return domain_last_elts;

    int v1 = Domaines[x1][0];

    for (int x2 = 0; x2 < nVar; x2++)
    {
        Constraint *contrainte1 = constraintMatrix[x1][x2];
        Constraint *contrainte2 = constraintMatrix[x2][x1];

        if (contrainte1 != nullptr)
        {
            for (int i = 0; i < domain_last_elts[x2]; i++)
            {
                int v2 = Domaines[x2][i];
                if (!(contrainte1->verifie(v1, v2)))
                {
                    swap(Domaines[x2][i], Domaines[x2][domain_last_elts[x2]-1]);
                    domain_last_elts[x2]--;
                    i--;
                }
            }
        }

        if (contrainte2 != nullptr)
        {
            for (int i = 0; i < domain_last_elts[x2]; i++)
            {
                int v2 = Domaines[x2][i];
                if (!(contrainte2->verifie(v2, v1)))
                {
                    swap(Domaines[x2][i], Domaines[x2][domain_last_elts[x2]-1]);
                    domain_last_elts[x2]--;
                    i--;
                }
            }
        }
    }

    return domain_last_elts;
}

pair<bool, vector<int>> CSP::backtrack(vector<int> instantiation_partielle, vector<int> ordre_variables)
{
    if (ordre_variables.empty())
    {
        for (int i = 0; i < nVar; i++)
            ordre_variables.push_back(i);
    }

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

pair<bool, vector<int>> CSP::new_backtrack(int heuristic, vector<int> assigned_vars, vector<int> instantiation_partielle)
{

    if (!assigned_vars.empty())
    {
        int var_introduite = assigned_vars.back();
        int val_introduite = instantiation_partielle.back();

        for (size_t i = 0; i + 1 < instantiation_partielle.size(); i++)
        {
            int var_comparaison = assigned_vars[i];
            int val_comparaison = instantiation_partielle[i];

            Constraint *contrainte1 = constraintMatrix[var_introduite][var_comparaison];
            if (contrainte1 != nullptr && !(contrainte1->verifie(val_introduite, val_comparaison)))
                return {false, {}};

            Constraint *contrainte2 = constraintMatrix[var_comparaison][var_introduite];
            if (contrainte2 != nullptr && !(contrainte2->verifie(val_comparaison, val_introduite)))
                return {false, {}};
        }
    }

    if (instantiation_partielle.size() == nVar)
    {
        vector<int> instance_valide(nVar);
        for (int i = 0; i < nVar; i++)
            instance_valide[assigned_vars[i]] = instantiation_partielle[i];
        return {true, instance_valide};
    }

    vector<int> remaining;
    for (int v = 0; v < nVar; ++v)
        if (find(assigned_vars.begin(), assigned_vars.end(), v) == assigned_vars.end())
            remaining.push_back(v);

    int next_var = selectNextVar(remaining, getDomainSizes(), heuristic);

    for (int val : Domaines[next_var]) {
        vector<int> new_assigned = assigned_vars;
        vector<int> new_inst = instantiation_partielle;

        new_assigned.push_back(next_var);
        new_inst.push_back(val);

        auto result = new_backtrack(heuristic, new_assigned, new_inst);
        if (result.first)
            return result;
    }

    return {false, {}};
}

pair<bool, vector<int>> CSP::fullFC(int heuristic, vector<int> domain_last_elts, vector<int> assigned_vars)
{  
    if (domain_last_elts.empty())
    {
        for (const auto& domain : Domaines)
            domain_last_elts.push_back(static_cast<int>(domain.size()));
    }

    int var_introduite = -1;
    if (!assigned_vars.empty())
        var_introduite = assigned_vars.back();

    domain_last_elts = FC(domain_last_elts, var_introduite);

    for (int s : domain_last_elts)
    {
        if (s < 1)
            return {false, {}};
    }

    if (assigned_vars.size() == nVar) {
        vector<int> instance_valide(nVar);
        for (int var : assigned_vars)
            instance_valide[var] = Domaines[var][0];
        return {true, instance_valide};
    }


    vector<int> remaining;
    for (int v = 0; v < nVar; ++v)
        if (find(assigned_vars.begin(), assigned_vars.end(), v) == assigned_vars.end())
            remaining.push_back(v);

    int next_var = selectNextVar(remaining, domain_last_elts, heuristic);
    //cout << "Next var : " << next_var << "\n";

    vector<int> new_assigned = assigned_vars;

    for (int i = 0; i < domain_last_elts[next_var]; ++i) {
        vector<int> new_domain_last_elts = domain_last_elts;
        new_domain_last_elts[next_var] = 1;
        swap(Domaines[next_var][0], Domaines[next_var][i]);

        auto result = fullFC(heuristic, new_domain_last_elts, new_assigned);
        if (result.first)
            return result;
    }

    return {false, {}};
}   

pair<bool, vector<int>> CSP::MAC(PropagationFct propagate, vector<int> domain_last_elts, vector<int> ordre_variables, int x1)
{
    if (ordre_variables.empty())
    {
        for (int i = 0; i < nVar; i++)
            ordre_variables.push_back(i);
    }

    if (domain_last_elts.empty())
    {
        for (const auto& domain : Domaines)
            domain_last_elts.push_back(static_cast<int>(domain.size()));
    }

    domain_last_elts = (this->*propagate)(domain_last_elts, x1);

    for (int s : domain_last_elts)
    {
        if (s < 1)
            return {false, {}};
    }

    for (int i : ordre_variables)
    {
        if (domain_last_elts[i] > 1)
        {
            vector<int> new_domain_last_elts = domain_last_elts;
            int old_size = domain_last_elts[i];
            new_domain_last_elts[i] = 1;
            for (int k = 0; k < old_size; k++)
            {
                swap(Domaines[i][0],Domaines[i][k]);
                auto result = MAC(propagate,new_domain_last_elts,ordre_variables,i);
                //cout << "i : " << i << " | k : " << k << " | result : " << result.first << "\n";
                if (result.first)
                    return result;
            }
        }
    }

    for (int s : domain_last_elts)
    {
        if (s != 1)
            return {false, {}};
    }

    domain_last_elts = AC4(domain_last_elts);

    for (int s : domain_last_elts)
    {
        if (s != 1)
            return {false, {}};
    }

    vector<int> instance_valide;
    for (int i : ordre_variables)
        instance_valide.push_back(Domaines[i][0]);
    return {true, instance_valide};
}

void CSP::generate_json_instance(const std::string &filename)
{  
    cout << filename << "\n";
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
