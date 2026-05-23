// ball.cc  : definition des methodes de la classe Ball
//
// Auteur   : Victor Henri Willy Eder
// Version  : 1.0 du 24.05.2026

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
// eps = 0 pour la lecture de fichier, epsil_zero pendant le jeu
bool Ball::is_inside_arena(double eps) const
{
    double xmin(body.center.x - body.radius);
    double xmax(body.center.x + body.radius);
    double ymax(body.center.y + body.radius);

    if (xmin < -eps) return false;
    if (xmax > arena_size + eps) return false;
    if (ymax > arena_size + eps) return false;
    if (body.center.y < -eps) return false;  // bord inférieur : centre seulement
    return true;
}

// vérifie que la norme du vecteur delta est <= delta_norm_max
// eps = 0 pour la lecture de fichier, epsil_zero pendant le jeu
bool Ball::is_delta_valid(double eps) const
{
    return norm(delta) <= delta_norm_max + eps;
}

bool Ball::is_valid(double eps) const
{
    if (!is_inside_arena(eps)) return false;
    if (!is_delta_valid(eps)) return false;
    return true;
}

void Ball::translate(Point d)
{
    body.center = body.center + d;
}

void Ball::setDelta(Point new_delta)
{
    delta = new_delta;
}