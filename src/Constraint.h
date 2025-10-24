#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

class Constraint
{
private:
    int x1, x2;                     // Indice des variables concernees par la contrainte
    vector<pair<int, int>> valeurs; // Vecteur des valeurs autorisees pour la paire
    unordered_set<uint64_t> map_valeurs;

public:
    Constraint(int vx1, int vx2, vector<pair<int, int>> vals);
    Constraint();

    uint64_t key(int a, int b) const { return (static_cast<uint64_t>(a) << 32) | static_cast<uint64_t>(b); }

    void print();

    int getX1() const;
    int getX2() const;

    json makeJson() const;

    bool verifie(int val1, int val2) const;                                                            // Vérifie si le couple <x1=val1, x2=val2> vérifie la contrainte
    bool supporte(int val1, vector<int>::const_iterator begin, vector<int>::const_iterator end) const; // Vérifie si il existe val2 tel que <val1,val2> vérifie la contrainte
    bool arc_consistant(vector<int>::const_iterator begin1, vector<int>::const_iterator end1, vector<int>::const_iterator begin2, vector<int>::const_iterator end2) const;
    // Vérifie si la contrainte est arc-consistante
};

#endif