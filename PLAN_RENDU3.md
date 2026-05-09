# Plan d'implémentation — Rendu 3 : Dynamiques du jeu

> **Objectif du Rendu 3 :** Implémenter la dynamique complète du jeu — déplacement des balles, gestion des rebonds sur l'arène/briques/raquette/autres balles, destruction des briques, et conditions de fin de partie (WON/LOST).

---

## Vue d'ensemble : état actuel vs ce qu'il faut faire

| Composant | Fichier | État actuel | À faire pour R3 |
|---|---|---|---|
| `Ball` | `ball.h / ball.cc` | ✅ Lecture, validation | ❌ Pas de setters (immutable) |
| `Paddle` | `paddle.h / paddle.cc` | ✅ Mouvement souris | ❌ Pas de `last_delta` |
| `Brick` (héritage) | `brick.h / brick.cc` | ✅ Polymorphisme | ❌ Pas de méthode `hit()` |
| `Game::update()` | `game.cc` | ❌ Quasi vide | ❌ Toute la physique |
| Score victoire | `game.cc` | ❌ Non calculé | ❌ `score_per_life * lives` |

---

## Étape 1 — Rendre `Ball` mutable

> **Pourquoi :** La classe `Ball` est actuellement 100% immutable — `body` et `delta` sont privés sans setter. Impossible de déplacer ou faire rebondir une balle sans ça.

### 1.1 — Dans `ball.h` — Ajouter les déclarations

```cpp
// ball.h — AJOUTER ces 3 méthodes publiques

class Ball
{
public:
    Ball(Circle body = {{0.0, 0.0}, 0.0}, Point delta = {0.0, 0.0});

    Circle getBody() const;
    Point getDelta() const;

    // === NOUVEAU (R3) ===
    void translate(Point d);        // déplace le centre de la balle de d
    void setDelta(Point new_delta); // met à jour le vecteur de déplacement
    // =====================

    bool is_inside_arena(double eps = 0) const;
    bool is_delta_valid(double eps = 0) const;
    bool is_valid(double eps = 0) const;

private:
    Circle body;
    Point delta;
};
```

### 1.2 — Dans `ball.cc` — Implémenter les nouvelles méthodes

```cpp
// ball.cc — AJOUTER en bas du fichier

void Ball::translate(Point d)
{
    body.center = body.center + d;
    // utilise operator+ déjà défini dans tools.cc
}

void Ball::setDelta(Point new_delta)
{
    delta = new_delta;
}
```

---

## Étape 2 — Ajouter `last_delta` à `Paddle`

> **Pourquoi :** La collision balle-raquette (section 4.1.4) utilise le *dernier déplacement enregistré* de la raquette comme son vecteur `delta`. Sans ça, impossible de calculer l'impulsion correctement.

### 2.1 — Dans `paddle.h` — Ajouter l'attribut et le getter

```cpp
// paddle.h — MODIFIER

class Paddle
{
public:
    Paddle(Point center = {0.0, 0.0}, double radius = 0.0);
    Circle getArc() const;
    double get_target_mouse() const;
    void set_target_mouse(double x);

    // === NOUVEAU (R3) ===
    Point getDelta() const;   // retourne le dernier déplacement
    // =====================

    bool is_y_valid(double eps = 0) const;
    bool is_x_valid(double eps = 0) const;
    bool is_visible(double eps = 0) const;
    bool is_valid(double eps = 0) const;
    void move_to(double x);

private:
    double visible_half_width(double eps = 0) const;
    Circle arc;
    double target_mouse;

    // === NOUVEAU (R3) ===
    Point last_delta = {0.0, 0.0}; // dernier vecteur de déplacement
    // =====================
};
```

### 2.2 — Dans `paddle.cc` — Modifier `move_to` et ajouter `getDelta`

```cpp
// paddle.cc — MODIFIER move_to pour enregistrer last_delta

void Paddle::move_to(double x)
{
    last_delta = {x - arc.center.x, 0.0}; // ← NOUVEAU : enregistre le déplacement
    arc.center.x = x;
}

// paddle.cc — AJOUTER getDelta

Point Paddle::getDelta() const
{
    return last_delta;
}
```

---

## Étape 3 — Ajouter `hit()` aux briques (polymorphisme)

> **Pourquoi :** Quand une balle touche une brique, l'effet dépend du type :
> - `RainbowBrick` → perd 1 `hit_point`, détruite à 0
> - `BallBrick` → génère une nouvelle balle (hérite de la position et du delta de la balle incidente)
> - `SplitBrick` → se divise en 4 SplitBrick plus petites (si taille suffisante)
>
> Le polymorphisme via `virtual hit()` permet de traiter tous les types uniformément dans `game.cc`.

### 3.1 — Dans `brick.h` — Déclarer `hit()` virtuelle

```cpp
// brick.h — MODIFIER la classe Brick

class Brick
{
public:
    Brick(Square body = {{0.0, 0.0}, 0.0}, BrickType type = RAINBOW);

    Square getBody() const;
    BrickType getType() const;

    bool is_inside_arena() const;
    bool is_size_valid() const;
    virtual bool is_valid() const;

    // === NOUVEAU (R3) ===
    // Retourne true si la brique est détruite après l'impact.
    // new_balls  : balles à ajouter au jeu (pour BallBrick)
    // new_bricks : briques à ajouter au jeu (pour SplitBrick)
    // ball_delta : delta de la balle qui a frappé (pour BallBrick)
    virtual bool hit(std::vector<Ball>& new_balls,
                     std::vector<Brick*>& new_bricks,
                     Point ball_delta) = 0;  // pure virtuelle
    // =====================

    virtual ~Brick() = default;

protected:
    Square body;
    BrickType type;
};
```

> ⚠️ Ajouter `#include "ball.h"` et `#include <vector>` en haut de `brick.h`

### 3.2 — Dans `brick.h` — Déclarer `hit()` pour chaque sous-classe

```cpp
class RainbowBrick : public Brick
{
public:
    RainbowBrick(Square body = {{0.0, 0.0}, 0.0}, int hit_points = 1);
    int getHitPoints() const;
    bool is_hit_points_valid() const;
    bool is_valid() const override;

    // === NOUVEAU (R3) ===
    bool hit(std::vector<Ball>& new_balls,
             std::vector<Brick*>& new_bricks,
             Point ball_delta) override;
    // =====================

private:
    int hit_points;
};

class BallBrick : public Brick
{
public:
    BallBrick(Square body = {{0.0, 0.0}, 0.0});

    // === NOUVEAU (R3) ===
    bool hit(std::vector<Ball>& new_balls,
             std::vector<Brick*>& new_bricks,
             Point ball_delta) override;
    // =====================
};

class SplitBrick : public Brick
{
public:
    SplitBrick(Square body = {{0.0, 0.0}, 0.0});

    // === NOUVEAU (R3) ===
    bool hit(std::vector<Ball>& new_balls,
             std::vector<Brick*>& new_bricks,
             Point ball_delta) override;
    // =====================
};
```

### 3.3 — Dans `brick.cc` — Implémenter `hit()` pour chaque type

#### `RainbowBrick::hit()`

```cpp
// brick.cc — AJOUTER

bool RainbowBrick::hit(std::vector<Ball>& /*new_balls*/,
                        std::vector<Brick*>& /*new_bricks*/,
                        Point /*ball_delta*/)
{
    --hit_points;
    return hit_points <= 0; // true = brique détruite
}
```

#### `BallBrick::hit()`

```cpp
// brick.cc — AJOUTER

bool BallBrick::hit(std::vector<Ball>& new_balls,
                     std::vector<Brick*>& /*new_bricks*/,
                     Point ball_delta)
{
    // La nouvelle balle hérite de la position du centre de la brique
    // et du delta de la balle incidente (avec rayon new_ball_radius)
    new_balls.push_back(Ball({body.center, new_ball_radius}, ball_delta));
    return true; // la BallBrick est détruite immédiatement
}
```

#### `SplitBrick::hit()`

```cpp
// brick.cc — AJOUTER

bool SplitBrick::hit(std::vector<Ball>& /*new_balls*/,
                      std::vector<Brick*>& new_bricks,
                      Point /*ball_delta*/)
{
    // Taille des sous-briques : moitié de la taille actuelle
    double new_half = body.half_size / 2.0;

    // Condition : la taille des sous-briques doit être >= brick_size_min
    // new_half * 2 >= brick_size_min  ↔  body.half_size >= brick_size_min
    if (body.half_size < brick_size_min) {
        return true; // trop petite pour se diviser → détruite sans enfants
    }

    // Offset depuis le centre : gap/2 + new_half
    double offset = split_brick_gap / 2.0 + new_half;

    double cx = body.center.x;
    double cy = body.center.y;

    // Les 4 sous-briques aux 4 coins
    //
    //   TL ──── TR
    //    |  gap  |
    //   BL ──── BR
    //
    new_bricks.push_back(new SplitBrick({{cx - offset, cy + offset}, new_half})); // haut-gauche
    new_bricks.push_back(new SplitBrick({{cx + offset, cy + offset}, new_half})); // haut-droite
    new_bricks.push_back(new SplitBrick({{cx - offset, cy - offset}, new_half})); // bas-gauche
    new_bricks.push_back(new SplitBrick({{cx + offset, cy - offset}, new_half})); // bas-droite

    return true; // la SplitBrick originale est détruite
}
```

> ⚠️ Ne pas oublier d'ajouter les `#include` nécessaires en haut de `brick.cc` :
> `#include "../tools/constants.h"` et `#include "ball.h"`

---

## Étape 4 — Implémenter la boucle principale `Game::update()`

> **Contexte :** La fonction `update()` dans `game.cc` ne fait actuellement que bouger la raquette. C'est ici que tout le moteur physique du jeu doit vivre. Elle est appelée à chaque tick du timer (toutes les `dt` ms).

Le pseudocode officiel des consignes (page 7) se décompose en **3 phases** :

```
Phase A : Déplacement des balles + rebonds (sans raquette)
Phase B : Déplacement de la raquette
Phase C : Collision balle-raquette + rebonds post-raquette
          + vérification état final (LOST / WON)
```

### 4.1 — Vue d'ensemble de `update()` (à écrire dans `game.cc`)

```cpp
// game.cc — REMPLACER update() entièrement

void Game::update()
{
    // === PHASE A : Déplacement des balles + rebonds ===
    std::vector<size_t> balls_to_remove; // indices à supprimer (tombées en bas)

    for (size_t i = 0; i < balls.size(); ++i) {
        Point old_pos = balls[i].getBody().center;
        balls[i].translate(balls[i].getDelta()); // déplacement

        // Balle tombée sous le bord inférieur → à supprimer
        if (balls[i].getBody().center.y < 0.0) {
            balls_to_remove.push_back(i);
            continue;
        }

        // Boucle de rebond (arène + briques + autres balles, PAS la raquette)
        unsigned nb_rebonds = 0;
        while (has_collision_no_paddle(i) && nb_rebonds < nb_bounce_max) {
            balls[i].translate(old_pos - balls[i].getBody().center); // annulation
            apply_bounce_no_paddle(i, old_pos);                       // nouveau delta
            old_pos = balls[i].getBody().center;
            balls[i].translate(balls[i].getDelta());
            ++nb_rebonds;
        }
        // Si encore en collision après nb_bounce_max : la balle reste sur place
    }

    // Supprimer les balles tombées (en ordre inverse pour ne pas décaler les indices)
    for (int k = (int)balls_to_remove.size() - 1; k >= 0; --k) {
        balls.erase(balls.begin() + balls_to_remove[k]);
    }

    // === PHASE B : Déplacement de la raquette ===
    move_paddle_to(paddle.get_target_mouse());

    // === PHASE C : Collision balle-raquette + état final ===
    for (size_t i = 0; i < balls.size(); ++i) {
        if (intersects(paddle.getArc(), balls[i].getBody())) {
            Point old_pos = balls[i].getBody().center;
            apply_paddle_bounce(i, old_pos); // rebond sur la raquette (non compté)

            unsigned nb_rebonds = 0;
            while (has_collision_no_paddle(i) && nb_rebonds < nb_bounce_max) {
                balls[i].translate(old_pos - balls[i].getBody().center);
                apply_bounce_no_paddle(i, old_pos);
                old_pos = balls[i].getBody().center;
                balls[i].translate(balls[i].getDelta());
                ++nb_rebonds;
            }
        }
    }

    // Appliquer les effets des briques touchées (hit, destroy, spawn)
    apply_brick_hits();

    // Vérification état final
    if (balls.empty() && lives == 0) {
        // LOST — rien à faire de plus ici, is_lost() le détecte
    } else if (bricks.empty()) {
        // WON — ajuster le score
        score += score_per_life * lives;
    }
}
```

> 💡 Les fonctions `has_collision_no_paddle`, `apply_bounce_no_paddle`, `apply_paddle_bounce` et `apply_brick_hits` sont des **méthodes privées** à déclarer dans `game.h` et à implémenter dans `game.cc`. Elles sont décrites dans les sections suivantes.

---

### 4.2 — Collision balle-arène (section 4.1.1 des consignes)

> **Principe :** Après déplacement, si la balle dépasse un bord (gauche, droit, haut), on inverse la composante du `delta` perpendiculaire au bord. Si la balle touche deux bords à la fois (coin), on regarde quelle coordonnée est la *plus éloignée* du centre de l'arène pour décider quel bord est prioritaire.

```
Arène (0 à arena_size) :
  ┌──────────────────────────────┐  ← bord haut  : inverser delta.y
  │                              │
  │                              │
  │    balle → ●                 │
  │                              │
  │                              │
  └──────────────────────────────┘
  ↑ bord gauche                    ↑ bord droit
  inverser delta.x                 inverser delta.x

  Bord bas : détruire la balle (centre.y < 0)
```

```cpp
// game.cc — méthode privée à déclarer dans game.h et implémenter ici

bool Game::bounce_arena(size_t i)
{
    Circle body = balls[i].getBody();
    Point delta = balls[i].getDelta();
    bool bounced = false;

    bool hit_left  = (body.center.x - body.radius < epsil_zero);
    bool hit_right = (body.center.x + body.radius > arena_size - epsil_zero);
    bool hit_top   = (body.center.y + body.radius > arena_size - epsil_zero);

    if (!hit_left && !hit_right && !hit_top) return false;

    // Cas coin : priorité au bord dont la coordonnée est la plus éloignée
    if ((hit_left || hit_right) && hit_top) {
        double dist_x = std::max(body.radius - body.center.x,
                                 body.center.x + body.radius - arena_size);
        double dist_y = body.center.y + body.radius - arena_size;
        if (dist_x > dist_y) hit_top = false;
        else { hit_left = false; hit_right = false; }
    }

    if (hit_left || hit_right) { delta.x = -delta.x; bounced = true; }
    if (hit_top)               { delta.y = -delta.y; bounced = true; }

    balls[i].setDelta(delta);
    return bounced;
}
```

---

### 4.3 — Collision balle-brique (section 4.1.3 des consignes)

> **Principe :**
> 1. Calculer la **différence** entre le centre de la brique et le centre de la balle.
> 2. Borner cette différence à `[-half_size, +half_size]` → c'est la **différence bornée**.
> 3. `direction_nominale = différence - différence_bornée` (direction de la normale à la surface touchée).
> 4. Calculer la composante nominale du delta : `v_n = dot(delta, direction_nominale_normalisée)`.
> 5. Nouveau delta : `delta_new = delta - 2 * v_n * direction_nominale_normalisée`.

```
     Centre brique
          │
difference│
          ▼
     [bornée max à half_size]
          │
différence│ bornée
     ┌────┴────┐
     │  BRIQUE │
     └─────────┘
                ← direction nominale = difference - difference_bornée
                  (pointe vers la surface touchée depuis la balle)
```

```cpp
// game.cc — méthode privée

// Retourne l'index de la première brique en collision avec la balle i, ou -1
int Game::find_brick_collision(size_t i) const
{
    for (size_t j = 0; j < bricks.size(); ++j) {
        if (intersects(balls[i].getBody(), bricks[j]->getBody(), epsil_zero))
            return (int)j;
    }
    return -1;
}

bool Game::bounce_brick(size_t ball_idx, int brick_idx)
{
    Circle body   = balls[ball_idx].getBody();
    Point  delta  = balls[ball_idx].getDelta();
    Square sq     = bricks[brick_idx]->getBody();

    // Différence entre centre brique et centre balle
    Point diff = sq.center - body.center;

    // Différence bornée (clamp à [-half_size, half_size])
    Point diff_borne = {
        std::clamp(diff.x, -sq.half_size, sq.half_size),
        std::clamp(diff.y, -sq.half_size, sq.half_size)
    };

    // Direction nominale (normale à la surface d'impact)
    Point dir_nom = diff - diff_borne;

    if (norm(dir_nom) < epsil_zero) return false; // balle trop au centre → ignorer

    Point n = normalized(dir_nom);

    // Composante nominale du delta
    double v_n = dot(delta, n);

    // Nouveau delta par réflexion : v_new = v - 2*v_n*n
    balls[ball_idx].setDelta(delta - n * (2.0 * v_n));

    // Marquer la brique comme touchée (pour apply_brick_hits plus tard)
    bricks_hit.insert(brick_idx); // voir section 4.6

    return true;
}
```

---

### 4.4 — Collision balle-balle (section 4.1.2 des consignes)

> **Principe :** Choc élastique. La formule de l'impulsion (scalaire, projetée sur l'axe reliant les centres) :
>
> `impulsion = (-v_n + v_autre_n) * (2 * r_autre² / (r² + r_autre²))`
>
> Puis : `delta_new = delta + impulsion * axe_normalisé`
>
> Brider la norme à `delta_norm_max`.

```
   Avant collision :          Après collision :
   ●──→ delta               ●  ←──●
      ←──●                  delta_new calculé avec l'impulsion
```

```cpp
// game.cc — méthode privée

bool Game::bounce_ball_ball(size_t i)
{
    bool bounced = false;

    for (size_t j = 0; j < balls.size(); ++j) {
        if (i == j) continue;
        if (!intersects(balls[i].getBody(), balls[j].getBody(), epsil_zero)) continue;

        Circle ci = balls[i].getBody();
        Circle cj = balls[j].getBody();
        Point  di = balls[i].getDelta();
        Point  dj = balls[j].getDelta();

        // Axe reliant les deux centres (normalisé)
        Point axis = normalized(cj.center - ci.center);

        // Projections des vitesses sur cet axe
        double v_n       = dot(di, axis);   // balle i
        double v_autre_n = dot(dj, axis);   // balle j

        double ri = ci.radius;
        double rj = cj.radius;

        // Impulsion (scalaire)
        double masse_factor = 2.0 * rj * rj / (ri * ri + rj * rj);
        double impulsion = (-v_n + v_autre_n) * masse_factor;

        // Nouveau delta pour la balle i
        Point new_delta = di + axis * impulsion;

        // Brider la norme à delta_norm_max
        double n = norm(new_delta);
        if (n > delta_norm_max) new_delta = new_delta * (delta_norm_max / n);

        balls[i].setDelta(new_delta);
        bounced = true;
        break; // une seule collision par tour de boucle while
    }

    return bounced;
}
```

---

### 4.5 — Collision balle-raquette (section 4.1.4 des consignes)

> **Principe :** Même formule que balle-balle **mais le rayon de la raquette est infini**, ce qui simplifie le facteur de masse :
>
> `2 * r_raquette² / (r² + r_raquette²)` → **2** quand r_raquette → ∞
>
> Donc : `impulsion = (-v_n + v_paddle_n) * 2`

```cpp
// game.cc — méthode privée

void Game::apply_paddle_bounce(size_t i, Point old_pos)
{
    Circle ball_body = balls[i].getBody();
    Circle paddle_arc = paddle.getArc();

    // Axe entre centre raquette et centre balle (normalisé)
    Point axis = normalized(ball_body.center - paddle_arc.center);

    Point di = balls[i].getDelta();
    Point dp = paddle.getDelta(); // dernier déplacement enregistré de la raquette

    double v_n       = dot(di, axis); // composante nominale de la balle
    double v_paddle_n = dot(dp, axis); // composante nominale de la raquette

    // Rayon infini → facteur de masse = 2
    double impulsion = (-v_n + v_paddle_n) * 2.0;

    Point new_delta = di + axis * impulsion;

    // Brider la norme
    double n = norm(new_delta);
    if (n > delta_norm_max) new_delta = new_delta * (delta_norm_max / n);

    balls[i].setDelta(new_delta);

    // Redéplacer depuis l'ancienne position avec le nouveau delta
    balls[i].translate(old_pos - balls[i].getBody().center); // revenir à old_pos
    balls[i].translate(balls[i].getDelta());                  // redéplacer
}
```

---

### 4.6 — Application des effets des briques (`apply_brick_hits`)

> **Principe :** Après que toutes les balles ont été déplacées et ont rebondi, on applique les effets des briques touchées. On maintient un ensemble `bricks_hit` des indices touchés. Les briques détruites sont supprimées du vecteur, les nouvelles balles/briques sont ajoutées.

**Dans `game.h` — Ajouter :**

```cpp
// game.h — AJOUTER dans la section privée

#include <set>

private:
    // ...attributs existants...
    std::set<int> bricks_hit; // indices des briques touchées ce step

    void apply_brick_hits();
    bool has_collision_no_paddle(size_t i) const;
    void apply_bounce_no_paddle(size_t i, Point old_pos);
    bool bounce_arena(size_t i);
    bool bounce_brick(size_t ball_idx, int brick_idx);
    bool bounce_ball_ball(size_t i);
    int  find_brick_collision(size_t i) const;
    void apply_paddle_bounce(size_t i, Point old_pos);
```

**Dans `game.cc` — Implémenter `apply_brick_hits` :**

```cpp
void Game::apply_brick_hits()
{
    std::vector<Ball>   new_balls;
    std::vector<Brick*> new_bricks;

    // Traiter les briques touchées en ordre inverse pour ne pas décaler les indices
    std::vector<int> sorted_hits(bricks_hit.begin(), bricks_hit.end());
    std::sort(sorted_hits.rbegin(), sorted_hits.rend());

    for (int idx : sorted_hits) {
        // Trouver le delta de la balle qui a touché cette brique
        // (simplification : utiliser une balle qui est en intersection)
        Point ball_delta = {0.0, 1.0}; // fallback
        for (auto const& ball : balls) {
            if (intersects(ball.getBody(), bricks[idx]->getBody())) {
                ball_delta = ball.getDelta();
                break;
            }
        }

        bool destroyed = bricks[idx]->hit(new_balls, new_bricks, ball_delta);
        score += score_per_hit;

        if (destroyed) {
            delete bricks[idx];
            bricks.erase(bricks.begin() + idx);
        }
    }

    bricks_hit.clear();

    // Ajouter les nouvelles balles et briques
    for (Ball& b : new_balls)   balls.push_back(b);
    for (Brick* b : new_bricks) bricks.push_back(b);
}
```

---

### 4.7 — Fonctions utilitaires de collision dans `update()`

```cpp
// game.cc — has_collision_no_paddle : détecte toute collision sauf raquette

bool Game::has_collision_no_paddle(size_t i) const
{
    Circle body = balls[i].getBody();

    // Collision avec l'arène
    if (body.center.x - body.radius < epsil_zero) return true;
    if (body.center.x + body.radius > arena_size - epsil_zero) return true;
    if (body.center.y + body.radius > arena_size - epsil_zero) return true;

    // Collision avec une brique
    for (auto const* brick : bricks) {
        if (intersects(body, brick->getBody(), epsil_zero)) return true;
    }

    // Collision avec une autre balle
    for (size_t j = 0; j < balls.size(); ++j) {
        if (i == j) continue;
        if (intersects(body, balls[j].getBody(), epsil_zero)) return true;
    }

    return false;
}

// game.cc — apply_bounce_no_paddle : applique le bon type de rebond

void Game::apply_bounce_no_paddle(size_t i, Point old_pos)
{
    // Priorité : brique > arène > balle
    int brick_idx = find_brick_collision(i);
    if (brick_idx >= 0) {
        bounce_brick(i, brick_idx);
        balls[i].translate(balls[i].getDelta());
        return;
    }
    if (bounce_arena(i)) {
        balls[i].translate(balls[i].getDelta());
        return;
    }
    bounce_ball_ball(i);
    balls[i].translate(balls[i].getDelta());
}
```

---

## Étape 5 — Finaliser le score à la victoire

> **État actuel :** `is_won()` retourne `bricks.empty()` mais ne calcule pas le bonus de vies.  
> **À faire :** Dans la phase C de `update()`, quand `bricks.empty()`, ajouter `score += score_per_life * lives`.

Ce code est déjà inclus dans le squelette de `update()` à la section 4.1. Il suffit de s'assurer que `lives` est correct au moment de la victoire.

---

## Récapitulatif — Checklist d'implémentation

```
[ ] Étape 1 : Ball — ajouter translate() et setDelta() dans ball.h / ball.cc
[ ] Étape 2 : Paddle — ajouter last_delta et getDelta(), modifier move_to()
[ ] Étape 3 : Brick — ajouter hit() virtuelle pure dans Brick, implémenter dans les 3 sous-classes
              [ ] RainbowBrick::hit()  — décrémenter hit_points
              [ ] BallBrick::hit()     — créer une Ball au centre de la brique
              [ ] SplitBrick::hit()    — créer 4 SplitBrick si taille >= brick_size_min
[ ] Étape 4 : Game::update() — réécrire entièrement
              [ ] 4.2 bounce_arena()           — rebond sur les 3 bords
              [ ] 4.3 bounce_brick()           — direction nominale + réflexion
              [ ] 4.4 bounce_ball_ball()       — choc élastique avec formule d'impulsion
              [ ] 4.5 apply_paddle_bounce()    — rayon infini → facteur 2
              [ ] 4.6 apply_brick_hits()       — appliquer hit(), score, destroy, spawn
              [ ] 4.7 has_collision_no_paddle() + apply_bounce_no_paddle()
[ ] Étape 5 : Score victoire — score += score_per_life * lives dans update()
[ ] Vérifier que les déclarations dans game.h sont à jour (toutes les nouvelles méthodes privées)
[ ] Vérifier les #include manquants dans brick.cc (ball.h, constants.h)
```

---

## Ordre de test recommandé

1. Compiler avec les nouvelles méthodes de `Ball` et `Paddle` vides → vérifier que ça compile.
2. Tester `bounce_arena()` seul avec une balle qui va vers un bord.
3. Tester `bounce_brick()` avec une balle qui va vers une Rainbow brick.
4. Tester `RainbowBrick::hit()` (la brique doit perdre des hit_points et changer de couleur).
5. Tester `BallBrick::hit()` (une balle doit apparaître).
6. Tester `SplitBrick::hit()` (4 briques filles doivent apparaître).
7. Tester `bounce_ball_ball()` avec 2 balles en collision.
8. Tester `apply_paddle_bounce()` avec la raquette mobile.
9. Tester la condition de fin (LOST / WON).
