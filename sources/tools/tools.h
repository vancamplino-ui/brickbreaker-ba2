#ifndef TOOLS_H
#define TOOLS_H

constexpr double epsil_zero = 0.125;

// définition d'un point (coordonées x et y) , 
// mais aussi d'un vecteur (x = déplacement horizontal, y = déplacement vertical)

struct Point
{
    double x;
    double y;
};

// définition d'un cercle par son centre et son rayon 

struct Circle
{
    Point center;
    double radius;
};

// définition d'un carré par son centre et la moitié de sa taille (pour faciliter les calculs)
// coins :
// coin_haut_gauche  = (center.x - half_size , center.y + half_size)
// coin_haut_droit   = (center.x + half_size , center.y + half_size)
// coin_bas_droit    = (center.x + half_size , center.y - half_size)
// coin_bas_gauche   = (center.x - half_size , center.y - half_size)
struct Square
{
    Point center;
    double half_size;
};




Point operator+(Point a, Point b);
Point operator-(Point a, Point b);
Point operator*(Point p, double k);
Point operator*(double k, Point p);
Point operator/(Point p, double k);

// produit scalaire entre deux vecteurs
double dot(Point a, Point b);

//norme de vecteurs
double norm_carre(Point p);
double norm(Point p);

//distance entre deux points
double distance_carre(Point a, Point b);
double distance(Point a, Point b);

// normalisation d'un vecteur
Point normalized(Point p);


// inclusion et intersection 
bool contains(Circle c, Point p);
bool contains(Square s, Point p);

bool intersects(Circle c1, Circle c2);
bool intersects(Square s1, Square s2);
bool intersects(Circle c, Square s);
#endif

