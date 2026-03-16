#include "paddle.h"

Paddle::Paddle(Point center, double radius)
    : arc{center, radius}
{
}

Circle Paddle::getArc() const
{
    return arc;
}

bool Paddle::is_y_valid() const
{
    if (arc.center.y <= 0) return true;
    return false;
}

bool Paddle::is_visible() const
{
    if (arc.center.y + arc.radius > 0) return true;
    return false;
}

bool Paddle::valid_extremities() const
{
    double left(arc.center.x - arc.radius);
    double right(arc.center.x + arc.radius);

    if (left >= 0 && right <= arena_size) return true;
    return false;
}

bool Paddle::is_x_valid() const
{
    if (valid_extremities()) return true;
    return false;
}

bool Paddle::is_valid() const
{
    if (!is_y_valid()) return false;
    if (!is_visible()) return false;
    if (!valid_extremities()) return false;
    return true;
}