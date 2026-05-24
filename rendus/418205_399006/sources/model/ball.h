// ball.h  : declaration de la classe Ball
//
// Auteur   : Victor Henri Willy Eder
// Version  : 1.0 du 26.04.2026

#ifndef BALL_H
#define BALL_H

#include "../tools/tools.h"
#include "../tools/constants.h"

class Ball
{
public:
    // constructeur de la balle
    Ball(Circle body = {{0.0, 0.0}, 0.0}, Point delta = {0.0, 0.0});

    Circle getBody() const;
    Point getDelta() const;

    // contrôle des données fournies en entrée
    bool is_inside_arena(double eps = 0) const;
    bool is_delta_valid(double eps = 0) const;
    bool is_valid(double eps = 0) const;

private:
    Circle body;  // on représente la balle par un cercle (centre + rayon)
    Point delta;  // vecteur de déplacement (dx, dy)
};

#endif
