#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Constraint
{
private:
    int x1, x2;                     // Indice des variables concernees par la contrainte
    vector<pair<int, int>> valeurs; // Vecteur des valeurs autorisees pour la paire

public:
    Constraint(int vx1, int vx2, vector<pair<int, int>> vals);
    Constraint();

    void print();
};

#endif