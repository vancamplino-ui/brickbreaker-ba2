#ifndef BRICK_H
#define BRICK_H

#include "tools.h"


enum BrickType {
    RAINBOW = 0,
    BALL = 1,
    SPLIT = 2
};

class Brick {
protected:
    Square square;
    BrickType type;

public:
    // Constructeur et Destructeur
    Brick(double x, double y, double size, BrickType t);
    virtual ~Brick() = default;

    // Getters
    Square getSquare() const { return square; }
    BrickType getType() const { return type; }
    Point getCenter() const { return square.getCenter(); }
    double getSize() const { return square.getSize(); }

    // Par défaut, une brique a un Hitpoints de 0. Sinon voir chez Rainbow_brick
    virtual int getHitPoints() const { return 0; }

    // Méthode virtuelle pure, à redéfinir chez les enfants
    virtual void draw() const = 0;
    virtual bool hit() = 0;
};

class Rainbow_brick : public Brick {
private:
    int hit_points;

public:
    // Constructeur
    Rainbow_brick(double x, double y, double size, int hp);

    // Getters
    int getHitPoints() const override { return hit_points; }

    // Dessine la brique
    void draw() const override;

    // Collision
    bool hit() override;
};

class Ball_brick : public Brick {
public:
    // Constructeur
    Ball_brick(double x, double y, double size);
    
    // Dessine la brique
    void draw() const override;

    // Collision
    bool hit() override;
};

class Split_brick : public Brick {
public:
    Split_brick(double x, double y, double size);

    // Dessine la brique
    void draw() const override;

    // Collision
    bool hit() override;

private:
    // Dessine récursivement des carrés imbriqués 
    void draw_recursive(Point center, double size, int level) const;
};

#endif
