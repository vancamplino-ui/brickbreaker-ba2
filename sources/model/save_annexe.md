# Proposition pour `Game::save(...)`

Ce fichier est un brouillon lisible a recopier ensuite dans `game.h` et `game.cc`.

Idee d'interface a ajouter dans `game.h` :

```cpp
void save(std::string const& filename) const;
```

Proposition d'implementation pour `game.cc` :

```cpp
void Game::save(std::string const& filename) const
{
    std::ofstream file(filename);

    file << "# score\n";
    file << score << "\n\n";

    file << "# lives\n";
    file << lives << "\n\n";

    Circle paddle_arc = paddle.getArc();
    file << "# paddle\n";
    file << paddle_arc.center.x << ' '
         << paddle_arc.center.y << ' '
         << paddle_arc.radius << "\n\n";

    file << "# bricks\n";
    file << bricks.size() << '\n';

    for (Brick const* brick : bricks) {
        Square body = brick->getBody();
        double size = 2.0 * body.half_size;

        file << brick->getType() << ' '
             << body.center.x << ' '
             << body.center.y << ' '
             << size;

        if (brick->getType() == RAINBOW) {
            RainbowBrick const* rainbow =
                dynamic_cast<RainbowBrick const*>(brick);
            file << ' ' << rainbow->getHitPoints();
        }

        file << '\n';
    }

    file << "\n# balls\n";
    file << balls.size() << '\n';

    for (Ball const& ball : balls) {
        Circle body = ball.getBody();
        Point delta = ball.getDelta();

        file << body.center.x << ' '
             << body.center.y << ' '
             << body.radius << ' '
             << delta.x << ' '
             << delta.y << '\n';
    }
}
```

## Format vise

La sauvegarde doit ressembler a un fichier de test, par exemple :

```text
# score
0

# lives
3

# paddle
50 -20 20

# bricks
5
0 65 55 5 1
0 85 45 5 1
0 75 25 5 1
0 40 95 5 1
0 30 95 5 1

# balls
1
50 50 1 0.5 0.5
```

## Attention sur `# output:`

La ligne :

```text
# output: ...
```

ne doit pas etre ecrite par `save(...)`.

Elle appartient aux fichiers de test pour documenter le resultat attendu, pas a l'etat du jeu lui-meme.

## Pourquoi cette structure

- On ecrit `# score`, puis la valeur du score.
- On laisse une ligne vide pour garder la meme lisibilite que les fichiers de test.
- Meme idee pour `lives`, `paddle`, `bricks` et `balls`.
- L'ordre doit rester compatible avec `load(...)`.

## Format des briques

Pour chaque brique, on ecrit :

```text
type x y c
```

et pour une `RainbowBrick`, on ajoute :

```text
hit_points
```

Donc :

- `type` = `brick->getType()`
- `x` et `y` = centre du `Square`
- `c` = taille complete du carre, donc `2 * half_size`
- `hit_points` seulement pour `RainbowBrick`

## Pourquoi `dynamic_cast`

`bricks` stocke des `Brick*`.

Quand le type est `RAINBOW`, on doit recuperer l'information specifique de `RainbowBrick` :

```cpp
RainbowBrick const* rainbow =
    dynamic_cast<RainbowBrick const*>(brick);
```

## Format des balles

Chaque balle est ecrite sous la forme :

```text
x y r dx dy
```

avec :

- centre `x`
- centre `y`
- rayon
- delta `x`
- delta `y`

## A prevoir au moment de l'integration

- declaration dans `game.h`
- implementation dans `game.cc`
- ajouter l'include necessaire pour `std::ofstream`
- brancher `game.save(file_name.string())` dans `case SAVE_FILE`
