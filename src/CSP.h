#ifndef CSP_H
#define CSP_H

#include "Constraint.h"
#include <iostream>
#include <string>
#include <vector>
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

public:
    CSP(int n, vector<vector<int>> D, vector<Constraint> C);
    void print();

    pair<bool, vector<int>> solve();
};

#endif