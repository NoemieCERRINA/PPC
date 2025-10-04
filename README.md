# PCC
Projet de Programmation par contraintes du MPRO

### TODO

- [x] - Définition des structures de données pour représentation d'un CSP.
- [x] - Implémentation de la méthode de backtrack.
- [x] - Définition d'une syntaxe pour représentation fichier d'un CSP.
- [x] - Codage d'un parser fichier -> objets.
- [] - Implémentation de AC3.
- [] - Implémentation de AC4.
- [] - Implémentation de FC.
- [] - Génération de fichiers tests n-reines.
- [] - Génération de fichiers tests nombre chromatique.
- [] - Génération d'autres fichiers tests.
- [] - Codage des fonctions d'évaluations du moteur.
- [] - Approfondissement (to be defined).
- [] - Rapport de soutenance.
- [] - Slides de soutenance.

### Exécution

Pour compiler le code (génère un exécutable **solver**): `make`

Pour exécuter le programme: `./solver <instance_file>`

### Format de fichiers d'instances

Les fichiers d'instances créés pour benchmarking sont stockés ici: [./instances](./instances/).

Le format sélectionné est `json` (parser existant en C++ simple à intégrer).

Un exemple simple de fichier d'instance est donné ici: [./instances/example.json](./instances/example.json)

| Nom  | Type | Définition |
| :--------------- | :--------------- | :----- |
| nVar | int | Nombre de variables du CSP (indexées de 0 à n-1) |
| Domaines | liste(json) | Définition des domaines |
| Domaines.vars | liste(int) | Liste de variables partageant le même domaine |
| Domaines.values | liste(int) | Liste des valeurs du domaine |
| Contraintes | liste(json) | Définition des contraintes |
| Domaines.vars | liste(pair<int,int>) | Liste de paires ordonnées de variables possédant la même contrainte définie en extension |
| Domaines.allowed | liste(pair<int,int>) | Liste des paires ordonnées de valeurs autorisées par la contrainte |
