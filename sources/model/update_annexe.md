# Proposition pour `Game::update()`

Ce fichier est un brouillon de conception, a integrer plus tard dans `game.h` et `game.cc`.

## Idee d'interface a ajouter dans `game.h`

```cpp
void update();
void move_paddle(double dx);
```

Version un peu plus riche possible plus tard :

```cpp
bool is_finished() const;
bool is_won() const;
bool is_lost() const;
```

## Idee generale

`Game::update()` doit representer **un seul pas de simulation**.

Donc a chaque appel :

1. on deplace les balles
2. on gere les collisions
3. on met a jour les briques / le score
4. on gere les pertes de balles / vies
5. on detecte victoire ou defaite

`My_window::loop()` appellera ensuite cette fonction periodiquement.

Le deplacement de la raquette doit rester une action separee :

- `update()` gere l'evolution automatique du jeu
- `move_paddle(double dx)` gere une commande utilisateur

## Proposition simple de squelette

```cpp
void Game::update()
{
    // 1. deplacer chaque balle selon son delta
    // 2. gerer collisions avec murs / paddle / briques
    // 3. appliquer les effets des briques speciales
    // 4. supprimer les balles sorties
    // 5. decrementer une vie si necessaire
    // 6. verifier victoire / defaite
}
```

Et a cote :

```cpp
void Game::move_paddle(double dx)
{
    // deplacer la raquette horizontalement
    // sans la laisser sortir de l'arene
}
```

## Proposition de decomposition en sous-fonctions

Si tu veux garder `update()` courte, elle peut appeler des helpers prives dans `Game` :

```cpp
void Game::update()
{
    move_balls();
    handle_wall_collisions();
    handle_paddle_collisions();
    handle_brick_collisions();
    remove_lost_balls();
    update_lives();
    update_game_state();
}
```

Et dans `game.h`, plus tard :

```cpp
private:
    void move_balls();
    void handle_wall_collisions();
    void handle_paddle_collisions();
    void handle_brick_collisions();
    void remove_lost_balls();
    void update_lives();
    void update_game_state();
```

Et en public :

```cpp
public:
    void update();
    void move_paddle(double dx);
```

## Ce que ferait chaque partie

### `move_balls()`

- pour chaque balle :
  - nouveau centre = ancien centre + delta

Donc conceptuellement :

```cpp
Circle body = ball.getBody();
Point delta = ball.getDelta();
body.center = body.center + delta;
```

Attention : avec ta structure actuelle, il te faudra sans doute des setters ou une reconstruction de `Ball`.

## `handle_wall_collisions()`

Pour chaque balle :

- si elle touche le mur gauche ou droit :
  - inverser `delta.x`
- si elle touche le haut :
  - inverser `delta.y`
- si elle passe sous la zone utile :
  - elle sera retiree plus tard dans `remove_lost_balls()`

## `handle_paddle_collisions()`

Pour chaque balle :

- tester collision avec la raquette
- si collision :
  - adapter la direction de `delta`

Version minimale au debut :
- juste inverser `delta.y`

Version plus fine plus tard :
- faire varier l'angle selon la position d'impact sur l'arc

## `move_paddle(double dx)`

Cette fonction sert a deplacer la raquette quand l'utilisateur appuie sur une touche
ou bouge la souris.

Idee simple :

- lire l'arc actuel avec `paddle.getArc()`
- calculer un nouveau centre horizontal
- reconstruire une nouvelle `Paddle`
- verifier qu'elle reste valide
- remplacer l'ancienne seulement si le mouvement est autorise

Proposition concrete :

```cpp
void Game::move_paddle(double dx)
{
    Circle arc = paddle.getArc();
    Point new_center = arc.center;
    new_center.x += dx;

    Paddle new_paddle(new_center, arc.radius);

    if (new_paddle.is_valid()) {
        paddle = new_paddle;
    }
}
```

Cette approche est pratique avec votre code actuel :

- pas besoin de setter special pour `Paddle`
- on reconstruit juste une nouvelle raquette
- la validation deja presente dans `Paddle::is_valid()` fait le filtre

## `handle_brick_collisions()`

Pour chaque balle et chaque brique :

- si collision :
  - mettre a jour le score
  - changer l'etat de la brique
  - adapter la balle

Selon le type :

- `RAINBOW`
  - decrementer `hit_points`
  - supprimer la brique si `hit_points == 0`

- `BALL`
  - supprimer la brique
  - ajouter une nouvelle balle

- `SPLIT`
  - supprimer la brique
  - creer les sous-briques si leur taille reste valide

## `remove_lost_balls()`

- supprimer de `balls` toutes celles dont le centre est passe sous `y = 0`

## `update_lives()`

- si toutes les balles ont disparu :
  - decrementer `lives`
  - selon vos regles, recreer une balle ou laisser l'etat vide

## `update_game_state()`

- si `bricks` est vide :
  - partie gagnee
- si `lives == 0` et plus de balle :
  - partie perdue

## Version minimale realiste pour commencer

Pour ne pas partir trop large, une premiere vraie `update()` pourrait faire seulement :

1. deplacer les balles
2. rebondir sur les murs
3. supprimer les balles perdues
4. decrementer `lives` si plus de balles

Donc un premier squelette raisonnable serait :

```cpp
void Game::update()
{
    move_balls();
    handle_wall_collisions();
    remove_lost_balls();
    update_lives();
}
```

Puis vous ajoutez :

- deplacement utilisateur de la raquette via `move_paddle(...)`
- paddle
- bricks
- score
- win/lose

## Ce qu'il faudra probablement ajouter au modele

Avec les classes actuelles, pour faire une vraie `update()`, il manquera probablement :

- des setters ou fonctions de modification pour `Ball`
- un moyen de modifier / supprimer proprement les briques
- des fonctions d'etat :
  - `is_finished()`
  - `is_won()`
  - `is_lost()`

Pour la raquette, un setter n'est pas indispensable si vous gardez :

- lecture de l'arc actuel
- reconstruction d'une nouvelle `Paddle`
- reaffectation a `paddle`

## Comment ce sera utilise depuis la GUI

Dans `loop()` :

```cpp
bool My_window::loop()
{
    if (!loop_activated) return false;

    game.update();
    update_infos();
    drawing.queue_draw();
    return true;
}
```

Et dans `step_clicked()` :

```cpp
game.update();
update_infos();
drawing.queue_draw();
```

Pour un deplacement utilisateur de la raquette :

```cpp
game.move_paddle(dx);
drawing.queue_draw();
```

`update_infos()` n'est pas forcement utile apres un simple mouvement de raquette,
car les compteurs affiches sont `score`, `lives`, `bricks` et `balls`.
