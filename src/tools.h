#ifndef TOOLS_H
#define TOOLS_H

#include <cmath>
#include "graphic.h"


constexpr double epsil_zero = 0.125;

class Point {
private:
    double x, y;

public:
    // Constructeur
    Point(double x_val = 0, double y_val = 0)
        : x(x_val), y(y_val) {}
    
    // Getters
    double getX() const { return x; }
    double getY() const { return y; }
    // Pour Vector2D
    double norm() const { return std::sqrt(x * x + y * y); }

    // Setters
    void setX(double x_val) { x = x_val; }
    void setY(double y_val) { y = y_val; }
};

class Circle {
private:
    Point center;
    double radius;

public:
    // Constructeur
    Circle(Point c = Point(), double r = 0)
        : center(c), radius(r) {}

    // Getters
    Point getCenter() const { return center; }
    double getRadius() const { return radius; }

    // Fonctions de dessin
    void draw(Color color) const;
    void draw_Arc(Color color) const;
};

class Square {
private:
    Point center;
    double size;

public:
    // Constructeur
    Square(Point c = Point(), double s = 0)
        : center(c), size(s) {}
    
    // Getters
    Point getCenter() const { return center; }
    double getSize() const { return size; }

    // Fonctions de dessin
    void draw(Color color) const;
    void draw_Border(Color color) const;
};

// Fonctions géométriques

// Circle intersect Circle (Balle avec paddle ou balle avec balle)
bool intersect(const Circle &c1, const Circle &c2, double epsil = epsil_zero);
// Square intersect Square (Brick avec brick ou brick avec arène)
bool intersect(const Square &s1, const Square &s2, double epsil = epsil_zero);
// Circle intersect Square (Balle avec brick ou Balle avec arène)
bool intersect(const Circle &c, const Square &s, double epsil = epsil_zero);
// Square contient Square (Arène contient brick)
bool contains(const Square &outer, const Square &inner, double epsil = epsil_zero);
// Square contient Circle (Arène contient balle)
bool contains(const Square &outer, const Circle &inner, double epsil = epsil_zero);
// Pour que le paddle puisse être maintenu dans l'arène lorsqu'on bouge la souris
bool contains_Arc(const Square &outer, const Circle &inner, 
                     double epsil = epsil_zero);

#endif