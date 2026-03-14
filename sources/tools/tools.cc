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

bool contains(Circle c, Point p)
{
    if (distance_carre(c.center, p) <= c.radius * c.radius) return true;
    return false;
}

bool contains(Square s, Point p)
{
    if (std::abs(p.x - s.center.x) <= s.half_size
        && std::abs(p.y - s.center.y) <= s.half_size) return true;
    return false;
}

bool intersects(Circle c1, Circle c2)
{
    double radius_sum(c1.radius + c2.radius);

    if (distance_carre(c1.center, c2.center) <= radius_sum * radius_sum) return true;
    return false;
}

bool intersects(Square s1, Square s2)
{
    if (std::abs(s1.center.x - s2.center.x) <= s1.half_size + s2.half_size
        && std::abs(s1.center.y - s2.center.y) <= s1.half_size + s2.half_size) return true;
    return false;
}

bool intersects(Circle c, Square s)
{
    Point min {s.center.x - s.half_size, s.center.y - s.half_size};
    Point max {s.center.x + s.half_size, s.center.y + s.half_size};
    Point closest {
        std::clamp(c.center.x, min.x, max.x),
        std::clamp(c.center.y, min.y, max.y),
    };

    if (distance_carre(c.center, closest) <= c.radius * c.radius) return true;
    return false;
}
