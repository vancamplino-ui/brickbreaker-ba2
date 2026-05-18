#include "tools.h"
#include <algorithm> // pour min() et max()
#include <cmath>

using namespace std;


// Fonction utilitaire pour calculer la distance au carré entre deux points
namespace { 
    // Evite le calcul de la racine carrée pour les comparaisons de distances
    double distance_squared(const Point& p1, const Point& p2) { 
        double dx = p1.getX() - p2.getX();
        double dy = p1.getY() - p2.getY();
        
        return dx * dx + dy * dy;
    }
}

// Intersection si distance entre leurs centres < la somme de leurs rayons 
bool intersect(const Circle &c1, const Circle &c2, double epsil) {
    double dist_Sq = distance_squared(c1.getCenter(), c2.getCenter());
    double radius_Sum = c1.getRadius() + c2.getRadius();

    return (dist_Sq - (radius_Sum * radius_Sum) < epsil); 
} 

// Intersection si les bords se touchent ou se chevauchent
bool intersect(const Square &s1, const Square &s2, double epsil){ 
    double h1 = s1.getSize() / 2.0;
    double h2 = s2.getSize() / 2.0;

    double dx = abs(s1.getCenter().getX() - s2.getCenter().getX()) - (h1 + h2);
    double dy = abs(s1.getCenter().getY() - s2.getCenter().getY()) - (h1 + h2);

    return (dx < epsil && dy < epsil);
}

// Intersection si la distance entre le centre du cercle et 
// le point le plus proche du carré < au rayon du cercle
bool intersect(const Circle &c, const Square &s, double epsil){  
    double h = s.getSize() / 2.0;
    // Trouve le point du carré le plus proche du centre du cercle
    double closest_X = max(s.getCenter().getX() - h, 
                        min(c.getCenter().getX(), s.getCenter().getX() + h)); 
    double closest_Y = max(s.getCenter().getY() - h, 
                        min(c.getCenter().getY(), s.getCenter().getY() + h)); 

    double dist_Sq = distance_squared(c.getCenter(), Point(closest_X, closest_Y));

    return (dist_Sq - (c.getRadius() * c.getRadius()) < epsil);
}

// Outer contient inner si les bords de inner sont tous à l'intérieur de outer
bool contains(const Square &outer, const Square &inner, double epsil){  
    double h_outer = outer.getSize() / 2.0;
    double h_inner = inner.getSize() / 2.0;

    double ox = outer.getCenter().getX();
    double oy = outer.getCenter().getY();
    double ix = inner.getCenter().getX();
    double iy = inner.getCenter().getY();
    
    // Formule : minimum + epsil_zero <= valeur <= maximum − epsil_zero.
    bool contains_x = (ox - h_outer + epsil <= ix - h_inner) 
                        && (ix + h_inner <= ox + h_outer - epsil); 
    bool contains_y = (oy - h_outer + epsil <= iy - h_inner) 
                        && (iy + h_inner <= oy + h_outer - epsil);

    return (contains_x && contains_y);
}

// Outer contient inner si les bords du cercle sont tous à l'intérieur du carré
bool contains(const Square &outer, const Circle &inner, double epsil){ 
    double h_outer = outer.getSize() / 2.0;
    double r_inner = inner.getRadius();

    double ox = outer.getCenter().getX();
    double oy = outer.getCenter().getY();
    double ix = inner.getCenter().getX();
    double iy = inner.getCenter().getY();
    
    // Formule : minimum + epsil_zero <= valeur <= maximum − epsil_zero.
    bool contains_x = (ox - h_outer + epsil <= ix - r_inner) 
                        && (ix + r_inner <= ox + h_outer - epsil); 
    // Rayon inférieur de la balle supprimé car négligée dans le bord inf de l'arène
    bool contains_y = (oy - h_outer + epsil <= iy) 
                        && (iy + r_inner <= oy + h_outer - epsil);

    return (contains_x && contains_y);
}

// Verifier que les bords d'un Arc (Paddle) soient à l'intérieur d'un carré (l'arène)
bool contains_Arc(const Square &outer, const Circle &inner, double epsil) {
    double h_outer = outer.getSize() / 2.0;
    double ox = outer.getCenter().getX();
    double oy = outer.getCenter().getY();   
    
    double Arc_x = inner.getCenter().getX();
    double Arc_y = inner.getCenter().getY();
    double Arc_r = inner.getRadius(); 


    // Vérifie si l'Arc atteint l'axe X, sinon impossible de calculer sqrt !
    if (Arc_r * Arc_r - Arc_y * Arc_y < oy - h_outer) {
        return false;
    }

    // Demi-largeur de l'arc au niveau du sol (y=0)
    double half_w = sqrt(Arc_r * Arc_r - Arc_y * Arc_y); 
    double x_min = Arc_x - half_w;
    double x_max = Arc_x + half_w;
    
    return (!(x_min < ox - h_outer + epsil || x_max > ox + h_outer - epsil)) ; 
}

// Fonctions de dessin 
void Circle::draw(Color color) const {
    graphic_draw_circle(center.getX(), center.getY(), radius, color);
}    

void Circle::draw_Arc(Color color) const { 
    graphic_draw_arc(center.getX(), center.getY(), radius, color);
}

void Square::draw(Color color) const { 
    graphic_draw_rectangle(center.getX(), center.getY(), size, color);
}

void Square::draw_Border(Color color) const {
    graphic_draw_border(center.getX(),center.getY(), size, color);
}
