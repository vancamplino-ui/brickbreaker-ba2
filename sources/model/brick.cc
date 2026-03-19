#include "brick.h"
 

Brick::Brick(Square body, BrickType type)
    : body{body}, type{type}
{
}
 
Square Brick::getBody() const
{
    return body;
}
 
BrickType Brick::getType() const
{
    return type;
}
 
// vérifie que la brique est intégralement comprise dans l'arène
// note : pas d'epsil_zero ici car c'est utilisé lors de la lecture de fichier
bool Brick::is_inside_arena() const
{
    double xmin(body.center.x - body.half_size);
    double xmax(body.center.x + body.half_size);
    double ymin(body.center.y - body.half_size);
    double ymax(body.center.y + body.half_size);
 
    if (xmin < 0)          return false;
    if (xmax > arena_size) return false;
    if (ymin < 0)          return false;
    if (ymax > arena_size) return false;
    return true;
}
 
// vérifie que la taille de la brique est >= brick_size_min
bool Brick::is_size_valid() const
{
    if (body.half_size * 2 >= brick_size_min) return true;
    return false;
}
 
bool Brick::is_valid() const
{
    if (!is_size_valid())   return false;
    if (!is_inside_arena()) return false;
    return true;
}
 
RainbowBrick::RainbowBrick(Square body, int hit_points)
    : Brick{body, RAINBOW}, hit_points{hit_points}
{
}
 
int RainbowBrick::getHitPoints() const
{
    return hit_points;
}
 
// vérifie que hit_points est bien dans [1, 7]
bool RainbowBrick::is_hit_points_valid() const
{
    if (hit_points >= 1 && hit_points <= 7) return true;
    return false;
}
 
bool RainbowBrick::is_valid() const
{
    if (!Brick::is_valid())     return false;
    if (!is_hit_points_valid()) return false;
    return true;
}
 
BallBrick::BallBrick(Square body)
    : Brick{body, BALL}
{
}

SplitBrick::SplitBrick(Square body)
    : Brick{body, SPLIT}
{
}
