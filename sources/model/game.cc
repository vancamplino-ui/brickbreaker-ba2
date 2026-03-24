#include "game.h"
#include "game_reader.h"
#include "../tools/tools.h"
#include "../tools/constants.h"
#include <string>
#include <iostream>
#include "message.h"

Game::Game()
    : score(0), lives(0), paddle(), balls(), bricks()
{
}

Game::~Game()
{
    clear_bricks();
}

void Game::apply_loaded_data(GameData& data)
{
    clear_bricks();

    score = data.score;
    lives = data.lives;
    paddle = data.paddle;
    balls = data.balls;
    bricks = std::move(data.bricks);
}

void Game::clear_bricks()
{
    for (Brick* brick : bricks) {
        delete brick;
    }

    bricks.clear();
}

bool Game::intersects_strict(Square s1, Square s2)
{
    double dx(std::abs(s1.center.x - s2.center.x)
          - (s1.half_size + s2.half_size));
    double dy(std::abs(s1.center.y - s2.center.y)
          - (s1.half_size + s2.half_size));
    if (dx < 0.0 && dy < 0.0) return true;
    return false;
}

bool Game::intersects_strict(Circle c1, Circle c2)
{
    double d(distance(c1.center, c2.center));
    double gap(d - (c1.radius + c2.radius));
    if (gap < 0.0) return true;
    return false;
}

bool Game::intersects_strict(Circle c, Square s)
{
    Point min{s.center.x - s.half_size, s.center.y - s.half_size};
    Point max{s.center.x + s.half_size, s.center.y + s.half_size};
    Point closest{
        std::clamp(c.center.x, min.x, max.x),
        std::clamp(c.center.y, min.y, max.y)
    };
    double d(distance(c.center, closest));
    double gap(d - c.radius);
    if (gap < 0.0) return true;
    return false;
}

bool Game::check_bricks_collision()
{
    for (size_t i = 0; i < bricks.size(); ++i) {
        for (size_t j = i + 1; j < bricks.size(); ++j) {
            if (intersects_strict(bricks[i]->getBody(), bricks[j]->getBody())) {
                std::cout << message::collision_bricks(i, j);
                return false;
            }
        }
    }
    return true;
}

bool Game::check_paddle_brick()
{
    for (size_t i = 0; i < bricks.size(); ++i) {
        if (intersects_strict(paddle.getArc(), bricks[i]->getBody())) {
            std::cout << message::collision_paddle_brick(i);
            return false;
        }
    }
    return true;
}

bool Game::check_balls_collision()
{
    for (size_t i = 0; i < balls.size(); ++i) {
        for (size_t j = i + 1; j < balls.size(); ++j) {
            if (intersects_strict(balls[i].getBody(), balls[j].getBody())) {
                std::cout << message::collision_balls(i, j);
                return false;
            }
        }
    }
    return true;
}

bool Game::check_ball_brick()
{
    for (size_t i = 0; i < balls.size(); ++i) {
        for (size_t j = 0; j < bricks.size(); ++j) {
            if (intersects_strict(balls[i].getBody(), bricks[j]->getBody())) {
                std::cout << message::collision_ball_brick(i, j);
                return false;
            }
        }
    }
    return true;
}

bool Game::check_paddle_ball()
{
    for (size_t i = 0; i < balls.size(); ++i) {
        if (intersects_strict(paddle.getArc(), balls[i].getBody())) {
            std::cout << message::collision_paddle_ball(i);
            return false;
        }
    }
    return true;
}

bool Game::load(std::string const& filename)
{
    GameData data;

    if (!load_file(filename, data)) return false;

    apply_loaded_data(data);

    if (!check_bricks_collision()) return false;
    if (!check_paddle_brick()) return false;
    if (!check_balls_collision()) return false;
    if (!check_ball_brick()) return false;
    if (!check_paddle_ball()) return false;

    std::cout << message::success();
    return true;
}
