#ifndef UTILS_H
#define UTILS_H

#include "Constraint.h"
#include "CSP.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

CSP Queens_generator(int n);

vector<Constraint> build_alldif(int nVar, vector<vector<int>> Domaines);
vector<Constraint> build_queens_constraints(int nVar, vector<vector<int>> Domaines);

#endif