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

    bool load(std::string const& filename);

private:
    int score;
    int lives;

    Paddle paddle;
    std::vector<Ball> balls;
    std::vector<Brick*> bricks;
};

#endif