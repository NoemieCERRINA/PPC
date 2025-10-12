#ifndef CSP_H
#define CSP_H

#include "Constraint.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <algorithm>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

/*
classe CSP
Representation du CSP
*/

class CSP
{
private:
    int nVar;                     // nombre de variables
    vector<vector<int>> Domaines; // ensemble des domaines de definition
    vector<Constraint> Constraints;
    vector<vector<Constraint *>> constraintMatrix; // pour accès en tant constant à la contrainte entre x1 et x2
    typedef vector<int> (CSP::*PropagationFct)(vector<int>, int);

    vector<int> reorder(vector<int> list, vector<int> order);

public:
    CSP(int n, vector<vector<int>> D, vector<Constraint> C);
    CSP(const std::string &filename);
    CSP();
    void print();
    int getnVar() const;

    vector<int> AC3(vector<int> domain_last_elts = {}, int x = -1);
    vector<int> AC4(vector<int> domain_last_elts = {}, int x1 = -1);
    vector<int> FC(vector<int> domain_last_elts = {}, int x1 = -1);

    pair<bool, vector<int>> backtrack(vector<int> instantiation_partielle = {}, vector<int> ordre_variables = {});
    pair<bool, vector<int>> fullFC(vector<int> domain_last_elts = {}, vector<int> ordre_variables = {}, int idx_var_introduite = -1);
    pair<bool, vector<int>> MAC(PropagationFct propagate = &CSP::AC4, vector<int> domain_last_elts = {}, vector<int> ordre_variables = {}, int x1 = -1);
    void generate_json_instance(const std::string &filename);
};

#endif