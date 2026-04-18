// paddle.cc  : definition des methodes de la classe Paddle
//
// Auteur     : Liam Van Camp
// Version    : 1.0 du 27.03.2026
//

#include "paddle.h"

Paddle::Paddle(Point center, double radius)
    : arc{center, radius}
{
}

Circle Paddle::getArc() const
{
    return arc;
}

bool Paddle::is_y_valid(double eps) const
{
    return arc.center.y <= eps;
}

bool Paddle::is_visible(double eps) const
{
    return arc.center.y + arc.radius > eps;
}

bool Paddle::valid_extremities(double eps) const
{
    double left(arc.center.x - arc.radius);
    double right(arc.center.x + arc.radius);

    return left >= -eps && right <= arena_size + eps;
}

bool Paddle::is_x_valid(double eps) const
{
    return valid_extremities(eps);
}

bool Paddle::is_valid(double eps) const
{
    if (!is_y_valid(eps)) return false;
    if (!is_visible(eps)) return false;
    if (!is_x_valid(eps)) return false;
    return true;
}

void Paddle::move_to(double x)
{
    arc.center.x = x;
}
