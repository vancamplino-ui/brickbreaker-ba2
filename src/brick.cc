#include "brick.h"
#include "constants.h"

using namespace std;


// Constructeur Brick
Brick::Brick(double x, double y, double size, BrickType t)
    : square(Point(x, y), size), type(t) 
    {}


// Constructeur Rainbow_brick
Rainbow_brick::Rainbow_brick(double x, double y, double size, int hp)
    : Brick(x, y, size, RAINBOW), hit_points(hp) 
    {}

void Rainbow_brick::draw() const {
    // -1 car énum de 0 à 6 pour les 7 couleurs
    square.draw(static_cast<Color>(hit_points - 1)); 
}

// Retourne true si la destruction de la brique est nécessaire
bool Rainbow_brick::hit() {
    if (hit_points <= 0) { return true; }
    --hit_points;
    if (hit_points == 0) { return true; }
    return false;
}


// Constructeur Ball_brick
Ball_brick::Ball_brick(double x, double y, double size)
    : Brick(x, y, size, BALL) 
    {}

void Ball_brick::draw() const {
    square.draw(RED);
    Circle(square.getCenter(), new_ball_radius).draw(BLACK);
}

bool Ball_brick::hit() {
    return true;
}


// Constructeur Split_brick
Split_brick::Split_brick(double x, double y, double size)
    : Brick(x, y, size, SPLIT) 
    {}

void Split_brick::draw() const {
    draw_recursive(square.getCenter(), square.getSize(), 0);
}

bool Split_brick::hit() {
    return true;
}

void Split_brick::draw_recursive(Point center, double size, int level) const { 
    Square s(center, size);
    // Change la couleur à chaque niveau de récursion
    // % 7 pour faire tourner les 7 couleurs de l'énum
    s.draw(static_cast<Color>(level % 7)); 
 
    double next_size = (size - split_brick_gap) / 2.0;  
    if (next_size >= brick_size_min) {  
        double offset = (size - next_size) / 2.0;  
        draw_recursive(Point(center.getX() - offset, center.getY() - offset), 
                       next_size, level + 1); 
        draw_recursive(Point(center.getX() + offset, center.getY() - offset),
                       next_size, level + 1); 
        draw_recursive(Point(center.getX() - offset, center.getY() + offset), 
                       next_size, level + 1);
        draw_recursive(Point(center.getX() + offset, center.getY() + offset), 
                       next_size, level + 1); 
    }
}
