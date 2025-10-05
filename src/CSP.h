#ifndef CSP_H
#define CSP_H

#include "Constraint.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
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

    vector<int> reorder(vector<int> list, vector<int> order);

public:
    CSP(int n, vector<vector<int>> D, vector<Constraint> C);
    CSP(const std::string &filename);
    void print();

    int getnVar() const;
    pair<bool, vector<int>> backtrack(vector<int> instantiation_partielle, vector<int> ordre_variables);
    void generate_json_instance(const std::string &filename);
};

#endif