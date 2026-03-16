#ifndef PADDLE_H
#define PADDLE_H

#include "../tools/tools.h"
#include "../tools/constants.h"

class Paddle
{
public:
    //constructeur de la raquette. 
    Paddle(Point center = {0.0, 0.0}, double radius = 0.0); 
    Circle getArc() const;

    // contrôle des donnée fournie en entrée pour voir si la raquette est valide ou pas 
    bool is_y_valid() const;
    bool is_x_valid() const;
    bool is_visible() const;
    bool valid_extremities() const;
    bool is_valid() const;  

private:
    Circle arc; // on représente l'arc de la raquette visible par un cercle entier
};

#endif