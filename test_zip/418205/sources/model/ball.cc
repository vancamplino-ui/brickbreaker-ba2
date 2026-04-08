// ball.cc  : definition des methodes de la classe Ball
//
// Auteur   : Victor Henri Willy Eder
// Version  : 1.0 du 27.03.2026
//

#include "ball.h"
 
Ball::Ball(Circle body, Point delta)
    : body{body}, delta{delta}
{
}
 
Circle Ball::getBody() const
{
    return body;
}
 
Point Ball::getDelta() const
{
    return delta;
}
 
// vérifie que la balle est bien dans l'arène
// le bord inférieur ignore le rayon (on teste juste le centre pour y=0)
// note : pas d'epsil_zero ici car c'est utilisé lors de la lecture de fichier
bool Ball::is_inside_arena() const
{
    double xmin(body.center.x - body.radius);
    double xmax(body.center.x + body.radius);
    double ymax(body.center.y + body.radius);
 
    if (xmin < 0) return false;
    if (xmax > arena_size) return false;
    if (ymax > arena_size) return false;
    if (body.center.y < 0) return false;  // bord inférieur : centre seulement
    return true;
}
 
// vérifie que la norme du vecteur delta est <= delta_norm_max
// note : pas d'epsil_zero ici car c'est utilisé lors de la lecture de fichier
bool Ball::is_delta_valid() const
{
    return norm(delta) <= delta_norm_max;
}
 
bool Ball::is_valid() const
{
    if (!is_inside_arena()) return false;
    if (!is_delta_valid()) return false;
    return true;
}
