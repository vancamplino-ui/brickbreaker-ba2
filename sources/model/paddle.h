// paddle.h  : declaration de la classe Paddle
//
// Auteur    : Liam Van Camp
// Version   : 1.0 du 27.03.2026
//

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

    // contrôle des données fournies en entrée 
    bool is_y_valid(double eps = 0) const;
    bool is_x_valid(double eps = 0) const;
    bool is_visible(double eps = 0) const;
    bool valid_extremities(double eps = 0) const;
    bool is_valid(double eps = 0) const;
    void move_to(double x); 

private:
    Circle arc; // on représente l'arc de la raquette visible par un cercle entier
};

#endif
