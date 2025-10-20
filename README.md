# PCC
Projet de Programmation par contraintes du MPRO

### TODO

- [x] - Définition des structures de données pour représentation d'un CSP.
- [x] - Implémentation de la méthode de backtrack.
- [x] - Définition d'une syntaxe pour représentation fichier d'un CSP.
- [x] - Codage d'un parser fichier -> objets.
- [x] - Implémentation de AC3.
- [x] - Implémentation de AC4.
- [x] - Implémentation de FC.
- [x] - Codage d'un générateur de fichier à partir d'un CSP objet
- [x] - Génération de fichiers tests n-reines.
- [x] - Génération de fichiers tests nombre chromatique.
- [ ] - Implémentation d'options pour choisir l'ordre de sélection des variables - l'ordre de sélection des valeurs.
- [ ] - Génération d'autres fichiers tests.
- [x] - Codage des fonctions d'évaluations du moteur.
- [ ] - Approfondissement (to be defined).
- [ ] - Rapport de soutenance.
- [ ] - Slides de soutenance.

### Exécution

Pour compiler le code (génère un exécutable **solver**): `make`

Pour exécuter le programme: `./solver <instance_file> <solving_algorithm>`

Il existe 4 options pour l'algorithme de résolution:
- Backtrack
- FC
- MAC3
- MAC4

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
