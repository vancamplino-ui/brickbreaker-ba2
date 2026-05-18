#ifndef PADDLE_H
#define PADDLE_H

#include <vector>
#include <memory>
#include "tools.h"
#include "brick.h"


class Paddle {
private:
    Circle circle;
    Point delta;

public:
    // Constructeur
    Paddle(double x, double y, double r, double dx = 0, double dy = 0);

    // Getters
    Circle getCircle() const { return circle; }
    Point getDelta() const { return delta; }
    Point getCenter() const { return circle.getCenter(); }
    double getRadius() const { return circle.getRadius(); }

    // Setters 
    void setDelta(Point new_delta) { delta = new_delta; }

    // Dessine la raquette
    void draw() const;

    // Déplace la raquette s'il n'y a aucune collision avec les briques ou les bords
    void move(double new_x, const Square& arena,
              const std::vector<std::unique_ptr<Brick>>& bricks);
};

#endif
