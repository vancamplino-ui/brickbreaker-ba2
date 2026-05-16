// brick.h  : declaration des classes de briques
//
// Auteur   : Victor Henri Willy Eder
// Version  : 1.0 du 26.04.2026

#ifndef BRICK_H
#define BRICK_H

#include <vector>

#include "../tools/tools.h"
#include "../tools/constants.h"

enum BrickType { RAINBOW, BALL, SPLIT };

class Brick
{
public:
    // constructeur de la brique
    Brick(Square body = {{0.0, 0.0}, 0.0}, BrickType type = RAINBOW);

    Square getBody() const;
    BrickType getType() const;

    // contrôle des données fournies en entrée
    bool is_inside_arena(double eps = 0) const;
    bool is_size_valid() const;
    virtual bool is_valid(double eps = 0) const;
    virtual bool hit(std::vector<Brick*>& new_bricks) = 0;

    virtual ~Brick() = default;

protected:
    Square body;
    BrickType type;

};

class RainbowBrick : public Brick
{
public:
    // constructeur de la Rainbow brick
    RainbowBrick(Square body = {{0.0, 0.0}, 0.0}, int hit_points = 1);

    int getHitPoints() const;

    // vérifie que hit_points est bien dans [1, 7]
    bool is_hit_points_valid() const;
    bool is_valid(double eps = 0) const override;
    bool hit(std::vector<Brick*>& new_bricks) override;

private:
    int hit_points;
};

class BallBrick : public Brick
{
public:
    // constructeur de la Ball brick
    BallBrick(Square body = {{0.0, 0.0}, 0.0});
    bool hit(std::vector<Brick*>& new_bricks) override;
};

class SplitBrick : public Brick
{
public:
    // constructeur de la Split brick
    SplitBrick(Square body = {{0.0, 0.0}, 0.0});
    bool hit(std::vector<Brick*>& new_bricks) override;
};

#endif 
