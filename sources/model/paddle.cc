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
    return arc.center.y <= 0;
}

bool Paddle::is_visible() const
{
    return arc.center.y + arc.radius > 0;
}

bool Paddle::valid_extremities() const
{
    double left(arc.center.x - arc.radius);
    double right(arc.center.x + arc.radius);

    return left >= 0 && right <= arena_size;
}

bool Paddle::is_x_valid() const
{
    return valid_extremities();
}

bool Paddle::is_valid() const
{
    if (!is_y_valid()) return false;
    if (!is_visible()) return false;
    if (!is_x_valid()) return false;
    return true;
}