# PCC
Projet de Programmation par contraintes du MPRO

### Exécution

Pour compiler le code (génère un exécutable **solver**): `make`

Pour exécuter le programme: `./solver <instance_file> <solving_algorithm> <variable_heuristic=optional>`

Il existe 4 options pour l'algorithme de résolution:
- Backtrack
- FC
- MAC3
- MAC4

Il existe 4 options pour l'heuristique de choix de sélection de variables
- 0 - Les variables sont traitées dans l'ordre
- 1 - Les variables sont traitées dans l'ordre de plus petit domaine
- 2 - Les variables sont traitées dans l'ordre de plus grand domaine
- 3 - Les variables sont traitées dans un ordre aléatoire [Défaut - en général le plus performant]

### Format de fichiers d'instances

Les fichiers d'instances créés pour benchmarking sont stockés ici: [./instances](./instances/).

Le format choisi est `json` (parser existant en C++ simple à intégrer).

Un exemple simple de fichier d'instance est donné ici: [./instances/example.json](./instances/example.json)

| Nom  | Type | Définition |
| :--------------- | :--------------- | :----- |
| nVar | int | Nombre de variables du CSP (indexées de 0 à n-1) |
| Domaines | liste(json) | Définition des domaines |
| Domaines.vars | liste(int) | Liste de variables partageant le même domaine |
| Domaines.values | liste(int) | Liste des valeurs du domaine |
| Contraintes | liste(json) | Définition des contraintes |
| Contraintes.vars | liste(pair<int,int>) | Liste des paires ordonnées de variables possédant la même contrainte définie en extension |
| Contraintes.allowed | liste(pair<int,int>) | Liste des paires ordonnées de valeurs autorisées par la contrainte |
