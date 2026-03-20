# Arbre des classes `Brick`

Ce document montre uniquement :
- la super-classe `Brick`
- ses variables et ses méthodes
- les sous-classes qui héritent de `Brick`
- leurs variables et leurs méthodes propres

---

## Arbre visuel

```mermaid
flowchart TD

    Brick["<div style='width:500px; text-align:left; white-space:normal;'>
    <b>Brick</b><br/><br/>
    Variables protegees :<br/>
    • Square body<br/>
    • BrickType type<br/><br/>
    Methodes :<br/>
    • Brick(Square body = {{0.0, 0.0}, 0.0}, BrickType type = RAINBOW)<br/>
    • Square getBody() const<br/>
    • BrickType getType() const<br/>
    • bool is_inside_arena() const<br/>
    • bool is_size_valid() const<br/>
    • virtual bool is_valid() const<br/>
    • virtual ~Brick() = default
    </div>"]

    RainbowBrick["<div style='width:500px; text-align:left; white-space:normal;'>
    <b>RainbowBrick : public Brick</b><br/><br/>
    Variable privee :<br/>
    • int hit_points<br/><br/>
    Methodes :<br/>
    • RainbowBrick(Square body = {{0.0, 0.0}, 0.0}, int hit_points = 1)<br/>
    • int getHitPoints() const<br/>
    • bool is_hit_points_valid() const<br/>
    • bool is_valid() const override
    </div>"]

    BallBrick["<div style='width:500px; text-align:left; white-space:normal;'>
    <b>BallBrick : public Brick</b><br/><br/>
    Variable propre :<br/>
    • aucune<br/><br/>
    Methode propre :<br/>
    • BallBrick(Square body = {{0.0, 0.0}, 0.0})
    </div>"]

    SplitBrick["<div style='width:500px; text-align:left; white-space:normal;'>
    <b>SplitBrick : public Brick</b><br/><br/>
    Variable propre :<br/>
    • aucune<br/><br/>
    Methode propre :<br/>
    • SplitBrick(Square body = {{0.0, 0.0}, 0.0})
    </div>"]

    Brick --> RainbowBrick
    Brick --> BallBrick
    Brick --> SplitBrick

    classDef root fill:#1d4ed8,color:#ffffff,stroke:#1e3a8a,stroke-width:3px
    classDef child fill:#dbeafe,color:#0f172a,stroke:#60a5fa,stroke-width:2px

    class Brick root
    class RainbowBrick,BallBrick,SplitBrick child
```

---

## Version simple

- `Brick`
  - Variables protegees
    - `Square body`
    - `BrickType type`
  - Methodes
    - `Brick(Square body = {{0.0, 0.0}, 0.0}, BrickType type = RAINBOW)`
    - `Square getBody() const`
    - `BrickType getType() const`
    - `bool is_inside_arena() const`
    - `bool is_size_valid() const`
    - `virtual bool is_valid() const`
    - `virtual ~Brick() = default`
  - Sous-classes
    - `RainbowBrick : public Brick`
      - Variable privee
        - `int hit_points`
      - Methodes
        - `RainbowBrick(Square body = {{0.0, 0.0}, 0.0}, int hit_points = 1)`
        - `int getHitPoints() const`
        - `bool is_hit_points_valid() const`
        - `bool is_valid() const override`
    - `BallBrick : public Brick`
      - Variable propre
        - `aucune`
      - Methode propre
        - `BallBrick(Square body = {{0.0, 0.0}, 0.0})`
    - `SplitBrick : public Brick`
      - Variable propre
        - `aucune`
      - Methode propre
        - `SplitBrick(Square body = {{0.0, 0.0}, 0.0})`

---

## Affichage dans VS Code

- ouvrir [brick_class_dependencies.md](/Users/liamvancamp/Desktop/EPFL/BA2/ICC_2/ICC_2_proj/PROJET_BRICK_BREAKER/Structuration_arbreOrganisation/brick_class_dependencies.md)
- `Cmd + Shift + V` pour voir l'arbre visuel
- le texte dans les cases est maintenant aligne a gauche
- ou clic droit puis `Open Preview`

---

## Rappel rapide

- `Brick` est la super-classe.
- `RainbowBrick`, `BallBrick` et `SplitBrick` héritent de `Brick`.
- `RainbowBrick` ajoute `hit_points` et des méthodes de validation propres.
- `BallBrick` et `SplitBrick` n’ajoutent pas de variable, seulement leur constructeur.
