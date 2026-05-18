#include "ball.h"
#include "constants.h" 
#include <algorithm>
#include <cmath>

using namespace std;

// Constructeur Ball
Ball::Ball(double x, double y, double r, double dx, double dy)
    : circle(Point(x, y), r), delta(dx, dy) 
    {}

// Dessine la balle
void Ball::draw() const { 
    circle.draw(BLACK); 
}


// =============================================================================
// FONCTIONS DYNAMIQUE DE BALL
// =============================================================================

// Crée un circle de test avec la nouvelle potentielle position de la balle
Circle Ball::test_circle() const {  
    Point c = circle.getCenter();   
    c.setX(c.getX() + delta.getX()); 
    c.setY(c.getY() + delta.getY());
    
    return Circle(c, circle.getRadius());
} 

// Effectue le vrai déplacement final de la balle (validation)
void Ball::update_position(Circle new_circle) { 
    circle = new_circle;                          
} 

// Plafonne la norme du vecteur vitesse à delta_norm_max
void Ball::limitSpeed(Point &d) {
    double norm = d.norm();
    if (norm == 0.0) { return; }

    if (norm > delta_norm_max) {
        double scale = delta_norm_max / norm;
        d.setX(d.getX() * scale);
        d.setY(d.getY() * scale);
    }
}

// Gère les rebonds avec les murs de l'arène
bool Ball::reboundWall(const Square &arena, const Circle &test) {    
    Point c = test.getCenter(); 
    double r = test.getRadius(); 
    double arena_min = arena.getCenter().getX() - arena.getSize() / 2;
    double arena_max = arena.getCenter().getX() + arena.getSize() / 2;

    // Murs verticaux 
    // delta.getX() < 0 : empêche les rebonds infinis si la balle s'éloigne
    if (c.getX() - r < arena_min + epsil_zero && delta.getX() < 0) {  
        delta.setX(-delta.getX());                                      
        limitSpeed(delta);
        return true;
    } 
    else if (c.getX() + r > arena_max - epsil_zero && delta.getX() > 0) {
        delta.setX(-delta.getX());
        limitSpeed(delta);
        return true;
    }

    // Mur du haut 
    if (c.getY() + r > arena_max - epsil_zero && delta.getY() > 0) { 
        delta.setY(-delta.getY());                                     
        limitSpeed(delta);                                             
        return true;
    } 

    return false;
}

// Gère les collisions avec la raquette
bool Ball::checkCollisionPaddle(const Circle &paddle_circle, Point paddle_delta,
                                const Circle &test) {
    if (intersect(test, paddle_circle, epsil_zero)) { 
        // Choc élastique avec la raquette
        double dx = test.getCenter().getX() - paddle_circle.getCenter().getX();
        double dy = test.getCenter().getY() - paddle_circle.getCenter().getY();
        double dist = sqrt(dx * dx + dy * dy);  

        if (dist > 0) {
            // Crée le vecteur normal au point de contact
            Point normal(dx / dist, dy / dist);    
            double vn = delta.getX() * normal.getX() + delta.getY() * normal.getY(); 
            double vn_paddle = paddle_delta.getX() * normal.getX() +
                               paddle_delta.getY() * normal.getY();
            
            // Empêche les rebonds infinis si la balle est déjà en train de s'éloigner
            if (vn - vn_paddle < 0) {
                double impulse = (-vn + vn_paddle) * 2.0;           
                delta.setX(delta.getX() + impulse * normal.getX());
                delta.setY(delta.getY() + impulse * normal.getY());

                limitSpeed(delta);
                return true;
            }
        } 
    }
    return false; 
} 

// Gère les collisions avec les briques
bool Ball::checkCollisionBrick(const Square &brick_square, const Circle &test) { 
    if (intersect(test, brick_square, epsil_zero)) {   
        // Calcule la différence entre le centre de la balle et le centre du carré
        double dx = test.getCenter().getX() - brick_square.getCenter().getX(); 
        double dy = test.getCenter().getY() - brick_square.getCenter().getY(); 
        double half_size = brick_square.getSize() / 2.0; 

        // Calcule le point du carré le plus proche du centre de la balle
        double clamped_x = max(-half_size, min(half_size, dx)); 
        double clamped_y = max(-half_size, min(half_size, dy)); 

        // Calcule le vecteur normal
        double normal_x = dx - clamped_x; 
        double normal_y = dy - clamped_y; 

        // Calcule la longueur du vecteur normal
        double length = sqrt(normal_x * normal_x + normal_y * normal_y); 

        // Cas particulier où la balle est à l'intérieur du carré (effet tunnel)
        if (length == 0.0) { 
            if (abs(dx) > abs(dy)) { 
                if (dx > 0) { normal_x = 1.0; } 
                else { normal_x = -1.0; }
                normal_y = 0.0;
            } else {
                normal_x = 0.0; 
                if (dy > 0) { normal_y = 1.0; } 
                else { normal_y = -1.0; }
            }
        } else { 
            // Normalise le vecteur normal
            normal_x = normal_x / length;    
            normal_y = normal_y / length;    
        }

        // Applique le rebond à la balle
        if (setReboundWithBrick(normal_x, normal_y)) { return true; }
    } 
    return false; 
} 

bool Ball::setReboundWithBrick(double normal_x, double normal_y) {
    // Calcule le produit scalaire entre la vitesse et la normale
    double prod_scal = delta.getX() * normal_x + delta.getY() * normal_y;

    // Déclenche le rebond que si la balle se dirige vers l'intérieur de la brique
    if (prod_scal < 0) { 
        double new_delta_x = delta.getX() - 2.0 * prod_scal * normal_x; 
        double new_delta_y = delta.getY() - 2.0 * prod_scal * normal_y; 

        delta.setX(new_delta_x); 
        delta.setY(new_delta_y); 
        limitSpeed(delta); 

        return true; 
    } else {
        return false;
    }
}

// Gère les collisions avec les balles
bool Ball::checkCollisionBall(Ball &other, const Circle &test) {  
    if (intersect(test, other.circle, epsil_zero)) { 
        double r1 = test.getRadius();  
        double r2 = other.circle.getRadius();  
        double dx = test.getCenter().getX() - other.circle.getCenter().getX(); 
        double dy = test.getCenter().getY() - other.circle.getCenter().getY(); 
        double dist = sqrt(dx * dx + dy * dy);  

        if (dist > 0) {     
            Point normal(dx / dist, dy / dist); 

            // Calcule les vitesses nominales 
            double vn1 = delta.getX() * normal.getX() + delta.getY() * normal.getY(); 
            double vn2 = other.delta.getX() * normal.getX() + 
                         other.delta.getY() * normal.getY(); 

            // Calcule les impulsions
            if (vn1 - vn2 < 0) {
                double impulse1 = (-vn1 + vn2) * (r2 * r2) * 2.0 / (r1 * r1 + r2 * r2); 
                double impulse2 = (vn1 - vn2) * (r1 * r1) * 2.0 / (r1 * r1 + r2 * r2); 

                // Change le nouveau delta des deux balles 
                delta.setX(delta.getX() + impulse1 * normal.getX()); 
                delta.setY(delta.getY() + impulse1 * normal.getY()); 
                other.delta.setX(other.delta.getX() + impulse2 * normal.getX()); 
                other.delta.setY(other.delta.getY() + impulse2 * normal.getY()); 

                // Limite la vitesse pour les deux balles
                limitSpeed(delta);
                limitSpeed(other.delta);

                return true; 
            }
        } 
    } 
    return false; 
} 