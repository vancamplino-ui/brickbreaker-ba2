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
    // vérification des collisions initiales entre les entités
    bool check_bricks_collision();
    bool check_paddle_brick();
    bool check_balls_collision();
    bool check_ball_brick();
    bool check_paddle_ball();
    
    int score;
    int lives;

    Paddle paddle;
    std::vector<Ball> balls;
    std::vector<Brick*> bricks;
};

#endif