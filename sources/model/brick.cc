// brick.cc  : definition des methodes des classes de briques
//
// Auteur   : Victor Henri Willy Eder
// Version  : 1.0 du 26.04.2026

#include "../tools/constants.h"

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
// eps = 0 pour la lecture de fichier, epsil_zero pendant le jeu
bool Brick::is_inside_arena(double eps) const
{
    double xmin(body.center.x - body.half_size);
    double xmax(body.center.x + body.half_size);
    double ymin(body.center.y - body.half_size);
    double ymax(body.center.y + body.half_size);

    if (xmin < -eps) return false;
    if (xmax > arena_size + eps) return false;
    if (ymin < -eps) return false;
    if (ymax > arena_size + eps) return false;
    return true;
}

// vérifie que la taille de la brique est >= brick_size_min
bool Brick::is_size_valid() const
{
    return body.half_size * 2 >= brick_size_min;
}

bool Brick::is_valid(double eps) const
{
    if (!is_size_valid()) return false;
    if (!is_inside_arena(eps)) return false;
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
    return hit_points >= 1 && hit_points <= 7;
}

bool RainbowBrick::is_valid(double eps) const
{
    if (!Brick::is_valid(eps)) return false;
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

bool RainbowBrick::hit(std::vector<Brick*>& /*new_bricks*/)
{
    --hit_points;
    return hit_points <= 0;
}

bool BallBrick::hit(std::vector<Brick*>& /*new_bricks*/)
{
    return true;
}

bool SplitBrick::hit(std::vector<Brick*>& new_bricks)
{
    double new_half = (2.0 * body.half_size - split_brick_gap) / 4.0;

    if (new_half * 2 < brick_size_min) return true;

    double offset = body.half_size / 2.0 + new_half / 2.0;
    double cx = body.center.x;
    double cy = body.center.y;

    new_bricks.push_back(new SplitBrick({{cx - offset, cy + offset}, new_half}));
    new_bricks.push_back(new SplitBrick({{cx + offset, cy + offset}, new_half}));
    new_bricks.push_back(new SplitBrick({{cx - offset, cy - offset}, new_half}));
    new_bricks.push_back(new SplitBrick({{cx + offset, cy - offset}, new_half}));

    return true;
}