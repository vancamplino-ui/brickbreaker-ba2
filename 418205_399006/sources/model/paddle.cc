// paddle.cc  : definition des methodes de la classe Paddle
//
// Auteur   : Liam Van Camp
// Version  : 1.0 du 26.04.2026

#include <cmath>

#include "paddle.h"

Paddle::Paddle(Point center, double radius)
    : arc{center, radius}, target_mouse(center.x)
{
}

Circle Paddle::getArc() const
{
    return arc;
}

double Paddle::get_target_mouse() const
{
    return target_mouse;
}

void Paddle::set_target_mouse(double x)
{
    target_mouse = x;
}

bool Paddle::is_y_valid(double eps) const
{
    return arc.center.y <= eps;
}

bool Paddle::is_visible(double eps) const
{
    return arc.center.y + arc.radius > eps;
}

double Paddle::visible_half_width(double eps) const
{
    if (arc.center.y >= eps) return arc.radius;

    double dy(eps - arc.center.y);
    double radius_sq(arc.radius * arc.radius);
    double width_sq(radius_sq - dy * dy);

    if (width_sq <= 0.0) return 0.0;

    return std::sqrt(width_sq);
}

bool Paddle::is_x_valid(double eps) const
{
    double half_width(visible_half_width(eps));
    double left(arc.center.x - half_width);
    double right(arc.center.x + half_width);

    return left >= -eps && right <= arena_size + eps;
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
