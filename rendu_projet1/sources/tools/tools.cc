// tools.cc  : definition des fonctions utilitaires geometriques
//
// Auteur    : Liam Van Camp
// Version   : 1.0 du 27.03.2026
//

#include <algorithm>
#include <cmath>

#include "tools.h"

// Définition des fonctions liées aux différentes opérations sur des points 
// et de vecteurs. 

Point operator+(Point a, Point b) { return {a.x + b.x, a.y + b.y}; }
Point operator-(Point a, Point b) { return {a.x - b.x, a.y - b.y}; }
Point operator*(Point p, double k) { return {p.x * k, p.y * k}; }
Point operator*(double k, Point p) { return p * k; }
Point operator/(Point p, double k) { return {p.x / k, p.y / k}; }

double dot(Point a, Point b) { return a.x * b.x + a.y * b.y; }
double norm_carre(Point p) { return dot(p, p); }
double norm(Point p) { return std::sqrt(norm_carre(p)); }
double distance_carre(Point a, Point b) { return norm_carre(a - b); }
double distance(Point a, Point b) { return norm(a - b); }

Point normalized(Point p)
{
    double p_norm(norm(p));

    if (p_norm <= epsil_zero) {
        return {0.0, 0.0};
    }

    return p / p_norm;
}


bool is_inside(Circle c, Point p)
{
    if (distance(c.center, p) < c.radius - epsil_zero) {
        return true;
    }
    return false;
}

bool is_inside(Square s, Point p)
{
    double xmin(s.center.x - s.half_size);
    double xmax(s.center.x + s.half_size);
    double ymin(s.center.y - s.half_size);
    double ymax(s.center.y + s.half_size);

    if (p.x > xmin + epsil_zero && p.x < xmax - epsil_zero
        && p.y > ymin + epsil_zero && p.y < ymax - epsil_zero) {
        return true;
    }
    return false;
}

bool intersects(Circle c1, Circle c2)
{
    double d(distance(c1.center, c2.center));
    double gap(d - (c1.radius + c2.radius));

    if (gap < epsil_zero) {
        return true;
    }
    return false;
}

bool intersects(Square s1, Square s2)
{
    double dx(std::abs(s1.center.x - s2.center.x)
              - (s1.half_size + s2.half_size));
    double dy(std::abs(s1.center.y - s2.center.y)
              - (s1.half_size + s2.half_size));

    double gap_x(std::max(dx, 0.0));
    double gap_y(std::max(dy, 0.0));

    double min_dist(std::sqrt(gap_x * gap_x + gap_y * gap_y));

    if (min_dist < epsil_zero) {
        return true;
    }
    return false;
}

bool intersects(Circle c, Square s)
{
    Point min{s.center.x - s.half_size, s.center.y - s.half_size};
    Point max{s.center.x + s.half_size, s.center.y + s.half_size};

    Point closest{
        std::clamp(c.center.x, min.x, max.x),
        std::clamp(c.center.y, min.y, max.y)
    };

    double d(distance(c.center, closest));
    double gap(d - c.radius);

    if (gap < epsil_zero) {
        return true;
    }
    return false;
}
