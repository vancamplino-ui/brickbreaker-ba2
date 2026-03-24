#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>

#include "paddle.h"
#include "ball.h"
#include "brick.h"
#include "../tools/constants.h"

struct GameData;

class Game
{
public:
    Game();
    ~Game();

    bool load(std::string const& filename);

private:
    // recopie dans Game les donnees lues par le lecteur.
    void apply_loaded_data(GameData& data);
    // libere les briques actuellement stockees dans Game.
    void clear_bricks();

    // versions strictes des tests d'intersection pour la validation initiale
    bool intersects_strict(Square s1, Square s2);
    bool intersects_strict(Circle c1, Circle c2);
    bool intersects_strict(Circle c, Square s);

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
