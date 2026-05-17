// paddle.h  : declaration de la classe Paddle
//
// Auteur   : Liam Van Camp
// Version  : 1.0 du 26.04.2026

#ifndef PADDLE_H
#define PADDLE_H

#include "../tools/tools.h"
#include "../tools/constants.h"

class Paddle
{
public:
    // constructeur de la raquette.
    Paddle(Point center = {0.0, 0.0}, double radius = 0.0);
    Circle getArc() const;
    double get_target_mouse() const;
    void set_target_mouse(double x);
    Point getDelta() const;

    // contrôle des données fournies en entrée
    bool is_y_valid(double eps = 0) const;
    bool is_x_valid(double eps = 0) const;
    bool is_visible(double eps = 0) const;    
    bool is_valid(double eps = 0) const;
    void move_to(double x);

private:
    double visible_half_width(double eps = 0) const;
    Circle arc; // on représente l'arc de la raquette visible par un cercle entier
    double target_mouse;
    Point last_delta = {0.0, 0.0};
};

#endif
