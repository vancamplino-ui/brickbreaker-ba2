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
 
    // contrôle des données fournies en entrée pour voir si la balle est valide ou pas
    bool is_inside_arena() const;
    bool is_delta_valid() const;
    bool is_valid() const;
 
private:
    Circle body;  // on représente la balle par un cercle (centre + rayon)
    Point delta;  // vecteur de déplacement (dx, dy)
};
 
#endif