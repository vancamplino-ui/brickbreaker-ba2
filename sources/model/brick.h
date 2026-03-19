#ifndef BRICK_H
#define BRICK_H
 
#include "../tools/tools.h"
#include "../tools/constants.h"
 
enum BrickType { RAINBOW = 0, BALL = 1, SPLIT = 2 };
 
class Brick
{
public:
    // constructeur de la brique
    Brick(Square body = {{0.0, 0.0}, 0.0}, BrickType type = RAINBOW);
 
    Square getBody() const;
    BrickType getType() const;
 
    // contrôle des données fournies en entrée pour voir si la brique est valide ou pas
    bool is_inside_arena() const;
    bool is_size_valid() const;
    virtual bool is_valid() const;
 
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
    bool is_valid() const override;
 
private:
    int hit_points;
};
 
class BallBrick : public Brick
{
public:
    // constructeur de la Ball brick
    BallBrick(Square body = {{0.0, 0.0}, 0.0});
};
 
class SplitBrick : public Brick
{
public:
    // constructeur de la Split brick
    SplitBrick(Square body = {{0.0, 0.0}, 0.0});
};
#endif 