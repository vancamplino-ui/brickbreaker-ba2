# brickbreaker-ba2

Projet de Brick Breaker realise par Victore et Liam dans le cadre du cours ICC 2.



## Organisation du projet

- `sources/tools` : fonctions utilitaires de geometrie.
- `sources/model/ball.*` : logique liee a la balle.
- `sources/model/brick.*` : logique liee aux briques.
- `sources/model/paddle.*` : logique liee a la raquette.
- `sources/model/game.*` : coordination generale du jeu.
- `sources/project/main.cc` : point d'entree du programme.

## Module tools

Le fichier `tools.h` definit trois structures principales :

- `Point` : represente soit un point du plan, soit un vecteur.
- `Circle` : represente un cercle avec un centre et un rayon.
- `Square` : represente un carre avec un centre et une demi-taille (`half_size`).

Le fichier `tools.cc` implemente ensuite les operations utiles sur ces structures.

### Operations sur les points et vecteurs

- `operator+` : additionne deux points ou deux vecteurs.
- `operator-` : calcule la difference entre deux points ou vecteurs.
- `operator*` : multiplie un vecteur par un scalaire.
- `operator/` : divise un vecteur par un scalaire.
- `dot(a, b)` : calcule le produit scalaire de deux vecteurs.
- `norm_carre(p)` : calcule la norme au carre d'un vecteur.
- `norm(p)` : calcule la longueur d'un vecteur.
- `distance_carre(a, b)` : calcule la distance au carre entre deux points.
- `distance(a, b)` : calcule la distance entre deux points.
- `normalized(p)` : renvoie le vecteur normalise, donc de longueur 1, sauf si le vecteur est trop petit.

### Tests geometriques

- `contains(Circle c, Point p)` : indique si un point est a l'interieur d'un cercle.
- `contains(Square s, Point p)` : indique si un point est a l'interieur d'un carre.
- `intersects(Circle c1, Circle c2)` : indique si deux cercles se touchent ou se chevauchent.
- `intersects(Square s1, Square s2)` : indique si deux carres se touchent ou se chevauchent.
- `intersects(Circle c, Square s)` : indique si un cercle touche un carre ou se trouve en collision avec lui.

## Utilisation pour le module ball

Le module `ball` peut reutiliser directement les fonctions de `tools` pour toute la partie mouvement et collision.

Exemples d'utilisation utiles :

- representer la balle par un `Circle`.
- representer la direction ou la vitesse par un `Point`.
- mettre a jour la position avec une formule du type `position = position + vitesse`.
- calculer la vitesse de la balle avec `norm(vitesse)`.
- recuperer une direction unitaire avec `normalized(vitesse)`.
- tester si la balle touche une brique avec `intersects(cercle_balle, carre_brique)`.
- tester si la balle sort d'une zone ou atteint un point particulier avec `distance(...)` ou `contains(...)`.

En pratique, si la balle possede un centre et un rayon, il suffit souvent de construire un `Circle` temporaire et d'utiliser les fonctions deja ecrites plutot que de refaire les calculs geometriques dans `ball.cc`.

## Utilisation pour le module brick

Le module `brick` peut utiliser `Square` pour representer la zone occupee par une brique.

Exemples d'utilisation utiles :

- representer chaque brique par un `Square`.
- verifier si un point appartient a la brique avec `contains(square, point)`.
- verifier si deux briques se chevauchent avec `intersects(square1, square2)`.
- verifier si une balle touche une brique avec `intersects(cercle_balle, square_brique)`.
- reutiliser `Point` pour manipuler les positions des centres des briques.

Cela permet de centraliser toute la geometrie dans `tools` et d'eviter de dupliquer des calculs dans plusieurs fichiers.


## include : 

    attention au include surtout pour tools.h et constants.h pcq ils se trouve dans un autre dossier donc --> 

        {
            #include "../tools/tools.h"
            #include "../tools/constants.h"
        }




## lundi 16.03 
    -  j'ai finalisé 2-3 fonctions dans tools mais il n'est pas fini a 100% dit moi si tu as besoin d'autres fonctions
    -  j'ai commencer paddle et mis certaines de réstriction lié seulement a paddle et a l'arène donc il me reste a faire celles liée a l'intersection entre une balle et la raquette   ou la raquette et une brique 

    - j'ai fait deux trois teste et ajouter un dossier ou ranger nos teste comme ca on copie colle ca dans main lors que l'on veut tester. 
    
## Workflow git

A faire a chaque fois :

1. Avant de coder :

```bash
git pull
```

2. Coder les modifications necessaires.

3. A la fin de toute modification de fichier ou dossier :

```bash
git status
git add .
git commit -m "Description courte"
git push
```
 
