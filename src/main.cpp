#include <iostream>
#include <string.h>
#include "Constraint.h"
#include "CSP.h"

int main(int argc, char *argv[])
{
    vector<Constraint> constraints;
    constraints.push_back(Constraint(1, 2, {pair<int, int>(1, 1)}));
    CSP Queen = CSP(2, {{1, 2}, {1, 2}}, constraints);
    Queen.print();
    return 0;
}