#ifndef BALL_H
#define BALL_H

#include "tools.h"


class Ball {
private:
    Circle circle;
    Point delta;

    // Méthodes privées pour la dynamique du jeu
    void limitSpeed(Point &d);
    bool setReboundWithBrick(double normal_x, double normal_y);

public:
    // Constructeur
    Ball(double x, double y, double r, double dx, double dy);

    // Getters
    Circle getCircle() const { return circle; }
    Point getDelta() const { return delta; }
    Point getCenter() const { return circle.getCenter(); }
    double getRadius() const { return circle.getRadius(); }

    // Setters 
    void setDelta(Point newDelta) { delta = newDelta; }
    void setCenter(Point newCenter) { circle = Circle(newCenter, circle.getRadius()); }
    void setCircle(Circle newCircle) { circle = newCircle; }

    // Dessine la balle
    void draw() const;

    // Méthodes publiques pour la dynamique du jeu
    Circle test_circle() const;
    void update_position(Circle new_circle);
    bool reboundWall(const Square& arena, const Circle& test);
    bool checkCollisionPaddle(const Circle& paddle_circle, Point paddle_delta,
                              const Circle& test);
    bool checkCollisionBrick(const Square& brick_square, const Circle& test);  
    bool checkCollisionBall(Ball& other, const Circle& test);              
};

#endif
