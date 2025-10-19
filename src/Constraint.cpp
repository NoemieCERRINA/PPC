#include "Constraint.h"

Constraint::Constraint(int vx1, int vx2, vector<pair<int, int>> vals)
{
    if (vx1 < vx2)
    {
        x1 = vx1;
        x2 = vx2;
        valeurs = vals;
        for (auto v : vals)
        {
            map_valeurs.insert({this->key(v.first, v.second), true});
        }
    }
    else
    {
        x1 = vx2;
        x2 = vx1;
        for (auto v : vals)
        {
            valeurs.emplace_back(v.second, v.first);
            map_valeurs.insert({this->key(v.second, v.first), true});
        }
    }
}

Constraint::Constraint()
{
    x1 = 0;
    x2 = 0;
    valeurs = {};
    map_valeurs;
}

void Constraint::print()
{
    cout << "Variables contraintes: " << x1 << " " << x2 << endl;
    cout << "Valeurs autorisees: ";
    for (pair<int, int> val : valeurs)
    {
        cout << "(" << val.first << "," << val.second << "); ";
    }
    cout << endl;
}

int Constraint::getX1() const
{
    return x1;
}

int Constraint::getX2() const
{
    return x2;
}

json Constraint::makeJson() const
{
    json constraint;
    constraint["vars"] = {{x1, x2}};
    constraint["allowed"] = valeurs;
    return constraint;
}

bool Constraint::verifie(int val1, int val2) const
{
    return map_valeurs.count(this->key(val1, val2));
    /*
        for (const auto &p : valeurs)
    {
        if (p.first == val1 && p.second == val2)
            return true;
    }
    return false;
    */
}

bool Constraint::supporte(int val1, vector<int>::const_iterator begin, vector<int>::const_iterator end) const
{
    for (auto it = begin; it != end; it++)
    {
        if (verifie(val1, *it))
            return true;
    }
    return false;
}

bool Constraint::arc_consistant(vector<int>::const_iterator begin1, vector<int>::const_iterator end1, vector<int>::const_iterator begin2, vector<int>::const_iterator end2) const
{
    for (auto it1 = begin1; it1 != end1; it1++)
    {
        if (supporte(*it1, begin2, end2))
            return true;
    }
    return false;
}