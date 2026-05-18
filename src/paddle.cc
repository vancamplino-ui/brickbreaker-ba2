#include "paddle.h"

using namespace std;

// Constructeur
Paddle::Paddle(double x, double y, double r, double dx, double dy)
    : circle(Point(x, y), r), delta(dx, dy)
    {}

void Paddle::draw() const { 
    circle.draw_Arc(BLACK); 
}

void Paddle::move(double new_x, const Square& arena,
                  const vector<unique_ptr<Brick>>& bricks) {
    // Crée un Paddle fantôme pour tester les collisions
    Circle test_circle(Point(new_x, circle.getCenter().getY()),
                       circle.getRadius());
                   
    // Vérifie les bords de l'arène 
    if (!contains_Arc(arena, test_circle, epsil_zero)) return; 
 
    // Vérifie les briques
    for (const auto& b : bricks) { 
        if (intersect(test_circle, b->getSquare(), epsil_zero)) return;
    }

    circle = test_circle;
}