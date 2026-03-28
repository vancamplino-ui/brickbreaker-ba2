// game.h  : declaration de la classe Game
//
// Auteurs   : Liam Van Camp, Victor Henri Willy Eder
// Version   : 1.0 du 27.03.2026
//

#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>

#include "paddle.h"
#include "ball.h"
#include "brick.h"
#include "../tools/constants.h"

class Game
{
public:
    Game();
    ~Game();

    bool load(std::string const& filename);

private:
    // libere les briques actuellement stockees dans Game.
    void clear_bricks();

    // detection des intersections initiales entre les entites
    bool bricks_intersect();
    bool paddle_intersects_brick();
    bool balls_intersect();
    bool ball_intersects_brick();
    bool paddle_intersects_ball();

    int score;
    int lives;

    Paddle paddle;
    std::vector<Ball> balls;
    // pointeurs necessaires pour stocker plusieurs types derives de Brick (R3)
    std::vector<Brick*> bricks;
};

#endif
