# Structure de `Game::load`

Ce fichier décrit la structure réelle de la fonction `Game::load` dans `game.cc`.

Le but n'est pas de lister tout le fichier, mais de montrer ce que fait `load`
et dans quel ordre.

---

## Vue générale

`Game::load(filename)` suit ce chemin :

```text
Game::load
├── 1. créer des variables temporaires
├── 2. appeler load_game_data(filename, ...)
│   ├── read_score
│   ├── read_lives
│   ├── read_paddle
│   ├── read_bricks
│   │   └── create_brick
│   │       └── make_brick
│   └── read_balls
│       └── create_ball
├── 3. copier les données lues dans l'objet Game
├── 4. vérifier les collisions initiales
│   ├── bricks_intersect
│   ├── paddle_intersects_brick
│   ├── balls_intersect
│   ├── ball_intersects_brick
│   └── paddle_intersects_ball
└── 5. afficher le message de succès
```

---

## Arbre d'appel de fonctions

```text
Game::load
├── load_game_data
│   ├── read_score
│   │   ├── read_int
│   │   │   └── read_next_token
│   ├── read_lives
│   │   ├── read_int
│   │   │   └── read_next_token
│   ├── read_paddle
│   │   ├── read_double
│   │   │   └── read_next_token
│   │   ├── read_double
│   │   │   └── read_next_token
│   │   ├── read_double
│   │   │   └── read_next_token
│   │   └── Paddle::is_valid
│   ├── read_bricks
│   │   ├── read_int
│   │   │   └── read_next_token
│   │   └── create_brick
│   │       ├── read_int
│   │       │   └── read_next_token
│   │       ├── read_double
│   │       │   └── read_next_token
│   │       ├── read_double
│   │       │   └── read_next_token
│   │       ├── read_double
│   │       │   └── read_next_token
│   │       ├── make_brick
│   │       │   ├── read_int
│   │       │   │   └── read_next_token
│   │       │   └── RainbowBrick::is_hit_points_valid
│   │       ├── Brick::is_inside_arena
│   │       └── Brick::is_size_valid
│   └── read_balls
│       ├── read_int
│       │   └── read_next_token
│       └── create_ball
│           ├── read_double
│           │   └── read_next_token
│           ├── read_double
│           │   └── read_next_token
│           ├── read_double
│           │   └── read_next_token
│           ├── read_double
│           │   └── read_next_token
│           ├── read_double
│           │   └── read_next_token
│           ├── Ball::is_inside_arena
│           └── Ball::is_delta_valid
├── Game::clear_bricks
│   └── clear_bricks
├── Game::bricks_intersect
│   └── Game::intersects
├── Game::paddle_intersects_brick
│   └── Game::intersects
├── Game::balls_intersect
│   └── Game::intersects
├── Game::ball_intersects_brick
│   └── Game::intersects
├── Game::paddle_intersects_ball
│   └── Game::intersects
└── message::success
```

---

## 1. Variables temporaires créées dans `load`

Avant de modifier l'objet `Game`, la fonction crée :

- `new_score`
- `new_lives`
- `new_paddle`
- `new_balls`
- `new_bricks`

Ces variables servent à lire tout le fichier sans casser l'état courant du jeu
si une erreur apparaît pendant la lecture.

---

## 2. Lecture du fichier : `load_game_data`

`load()` délègue toute la lecture à :

```cpp
load_game_data(filename, new_score, new_lives, new_paddle, new_balls, new_bricks)
```

Cette fonction :

1. ouvre le fichier
2. lit les données dans l'ordre exact demandé
3. retourne `false` dès qu'une erreur est détectée

Ordre réel de lecture :

```text
load_game_data
├── read_score
├── read_lives
├── read_paddle
├── read_bricks
└── read_balls
```

---

## 3. Structure de la lecture des briques

La partie briques est structurée comme ceci :

```text
read_bricks
├── lire nb_bricks
└── boucle sur nb_bricks
    └── create_brick
        ├── lire type
        ├── lire x
        ├── lire y
        ├── lire c
        ├── make_brick
        │   ├── si type == RAINBOW
        │   │   ├── lire hit_points
        │   │   └── vérifier RainbowBrick::is_hit_points_valid
        │   ├── si type == BALL
        │   │   └── créer BallBrick
        │   └── sinon
        │       └── créer SplitBrick
        ├── vérifier Brick::is_inside_arena
        └── vérifier Brick::is_size_valid
```

Ensuite la brique créée est validée directement dans `create_brick`.

---

## 4. Structure de la lecture des balles

La partie balles est structurée comme ceci :

```text
read_balls
├── lire nb_balls
└── boucle sur nb_balls
    └── create_ball
        ├── lire x
        ├── lire y
        ├── lire r
        ├── lire dx
        ├── lire dy
        ├── construire Ball
        ├── vérifier Ball::is_inside_arena
        └── vérifier Ball::is_delta_valid
```

La validation d'une balle est faite directement dans `create_ball`.

---

## 5. Transfert des données temporaires dans `Game`

Si `load_game_data(...)` réussit, alors `load()` recopie les données dans
l'objet `Game` :

```text
score  = new_score
lives  = new_lives
paddle = new_paddle
balls  = new_balls
clear_bricks()
bricks = move(new_bricks)
```

Donc l'état courant du jeu n'est remplacé qu'après une lecture complète.

---

## 6. Validations globales après la lecture

Après avoir chargé les données dans `Game`, `load()` vérifie les collisions
initiales dans cet ordre exact :

```text
1. bricks_intersect()
2. paddle_intersects_brick()
3. balls_intersect()
4. ball_intersects_brick()
5. paddle_intersects_ball()
```

Chaque fonction :

- parcourt les objets concernés
- utilise `Game::intersects(...)`
- affiche un message d'erreur si une collision est trouvée
- retourne `true` si un problème existe

Donc dans `load()` :

```text
si l'une de ces fonctions retourne true
=> load retourne false
```

---

## 7. Fin de la fonction

Si :

- la lecture du fichier s'est bien passée
- les objets sont valides
- aucune collision initiale n'est détectée

alors :

```text
message::success()
return true
```

---

## Résumé court

La structure réelle de `load` est donc :

```text
load
├── lire tout dans des variables temporaires
├── copier dans Game
├── tester les collisions initiales
└── valider le chargement
```

Et la partie lecture interne est :

```text
score -> lives -> paddle -> bricks -> balls
```
