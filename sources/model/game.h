// game.h  : declaration de la classe Game
//
// Auteurs  : Liam Van Camp, Victor Henri Willy Eder
// Version  : 1.0 du 24.05.2026

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
    Game(Game const&) = delete;
    Game& operator=(Game const&) = delete;
    Game(Game&&) = delete;
    Game& operator=(Game&&) = delete;

    bool load(std::string const& filename);
    void save(std::string const& filename) const;
    void update();
    bool is_finished() const;
    bool is_won() const;
    bool is_lost() const;
    void reset();
    int get_score() const;
    int get_lives() const;
    Paddle const& get_paddle() const;
    std::vector<Ball> const& get_balls() const;
    std::vector<Brick*> const& get_bricks() const;
    void set_paddle_target(double x);
    void add_ball_on_paddle();
    void move_paddle_to(double x);

private:
    // méthodes privées de game
    // libere les briques actuellement stockees dans Game.
    void clear_bricks();
    // detection des intersections initiales entre les entites
    bool bricks_intersect();
    bool paddle_intersects_brick(Paddle const* tested_paddle = nullptr) const;
    bool balls_intersect();
    bool ball_intersects_brick();
    bool paddle_intersects_ball();

    // déclaration des attributs
    int score;
    int lives;
    Paddle paddle;
    std::vector<Ball> balls;
    // pointeurs necessaires pour stocker plusieurs types derives de Brick (R3)
    std::vector<Brick*> bricks;
};

#endif