#include "game.h"
#include "../tools/tools.h"
#include "../tools/constants.h"
#include <fstream>
#include <string>
#include <iostream>
#include "message.h"


namespace {
    bool read_next_token(std::ifstream& file, std::string& token)
    {
        while (file >> token) {

            if (!token.empty() && token[0] == '#') {
                std::string rest;
                std::getline(file, rest);
                continue;
            }

            return true;
        }

        return false;
    }

    bool read_score(std::ifstream& file, int& score)
    {
        std::string token;

        if (!read_next_token(file, token)) return false;

        score = std::stoi(token);

        if (score < 0) {
            std::cout << message::invalid_score(score);
            return false;
        }

        return true;
    }

    bool read_lives(std::ifstream& file, int& lives)
    {
        std::string token;

        if (!read_next_token(file, token)) return false;

        lives = std::stoi(token);

        if (lives < 0) {
            std::cout << message::invalid_lives(lives);
            return false;
        }

        return true;
    }
    bool read_paddle(std::ifstream& file, Paddle& paddle, double& x, double& y, double& r)
    {
        std::string sx, sy, sr;

        if (!read_next_token(file, sx)) return false;
        if (!read_next_token(file, sy)) return false;
        if (!read_next_token(file, sr)) return false;

        x = std::stod(sx);
        y = std::stod(sy);
        r = std::stod(sr);

        paddle = Paddle({x, y}, r);

        if (!paddle.is_valid()) {
            std::cout << message::paddle_outside(x, y);
            return false;
        }

        return true;
    }

    // fonctions liées à la vérification de brick. 
    // 1 : lecture du nombre de brick
    bool read_bricks_count(std::ifstream& file, int& nb_bricks)
    {
        std::string token;

        if (!read_next_token(file, token)) return false;

        nb_bricks = std::stoi(token);

        if (nb_bricks < 0)  return false;
        return true;
    }

    // vérification lignes par lignes de chaque brick
    bool read_brick_type(std::ifstream& file, int& type)
    {
        std::string token;

        if (!read_next_token(file, token)) return false;

        type = std::stoi(token);

        if (type < RAINBOW || type > SPLIT) {
            std::cout << message::invalid_brick_type(type);
            return false;
        }

        return true;
    }

    bool read_brick_geometry(std::ifstream& file, double& x, double& y, double& c)
    {
        std::string sx, sy, sc;

        if (!read_next_token(file, sx)) return false;
        if (!read_next_token(file, sy)) return false;
        if (!read_next_token(file, sc)) return false;

        x = std::stod(sx);
        y = std::stod(sy);
        c = std::stod(sc);

        return true;
    }

    bool read_hit_points(std::ifstream& file, int& hit_points)
    {
        std::string token;

        if (!read_next_token(file, token)) return false;

        hit_points = std::stoi(token);

        return true;
    }

    Square make_brick_body(double x, double y, double c)
    {
        return {{x, y}, c / 2.0};
    }

    bool validate_brick(Brick* brick, double x, double y, double c)
    {

        if (!brick->is_inside_arena()) {
            std::cout << message::brick_outside(x, y);
            delete brick;
            return false;
        }

        if (!brick->is_size_valid()) {
            std::cout << message::invalid_brick_size(c);
            delete brick;
            return false;
        }

    
        return true;
    }

    //création de chaque type de brick, avec vérification de la validité de chacune d'entre elles
    //la fonction rainbow vérifie également les hit_points. 
    bool create_rainbow_brick(std::ifstream& file, std::vector<Brick*>& bricks,
                              double x, double y, double c)
    {
        int hit_points = 0;

        if (!read_hit_points(file, hit_points)) return false;

        Square body = make_brick_body(x, y, c);
        RainbowBrick* brick = new RainbowBrick(body, hit_points);

        if (!brick->is_hit_points_valid()) {
            std::cout << message::invalid_hit_points(hit_points);
            delete brick;
            return false;
        }

        if (!validate_brick(brick, x, y, c)) return false;
        

        bricks.push_back(brick);
        return true;
    }

    bool create_ball_brick(std::vector<Brick*>& bricks, double x, double y, double c)
    {
        Square body = make_brick_body(x, y, c);
        BallBrick* brick = new BallBrick(body);

        if (!validate_brick(brick, x, y, c)) return false;

        bricks.push_back(brick);
        return true;
    }

    bool create_split_brick(std::vector<Brick*>& bricks, double x, double y, double c)
    {
        Square body = make_brick_body(x, y, c);
        SplitBrick* brick = new SplitBrick(body);

        if (!validate_brick(brick, x, y, c)) return false;

        bricks.push_back(brick);
        return true;
    }

    // vérifie que la brick est bien dans l'arène et que sa taille est valide
    bool read_one_brick(std::ifstream& file, std::vector<Brick*>& bricks)
    {
        int type = 0;
        double x = 0.0;
        double y = 0.0;
        double c = 0.0;

        if (!read_brick_type(file, type)) return false;
        if (!read_brick_geometry(file, x, y, c)) return false;

        if (type == RAINBOW) {
            return create_rainbow_brick(file, bricks, x, y, c);
        }

        if (type == BALL) {
            return create_ball_brick(bricks, x, y, c);
        }

        return create_split_brick(bricks, x, y, c);
    }

    // lie d'abord le nombre de brick, puis vérifie chaque brick une par une
    bool read_bricks(std::ifstream& file, std::vector<Brick*>& bricks)
    {
        int nb_bricks = 0;

        if (!read_bricks_count(file, nb_bricks)) return false;

        for (int i = 0; i < nb_bricks; ++i) {
            if (!read_one_brick(file, bricks)) return false;
        }

        return true;
    }

    // fonctions liées à la vérification de ball.
    // 1 : lecture du nombre de ball
    bool read_balls_count(std::ifstream& file, int& nb_balls)
    {
        std::string token;

        if (!read_next_token(file, token)) return false;

        nb_balls = std::stoi(token);

        if (nb_balls < 0) {
            return false;
        }

        return true;
    }
    // vérification lignes par lignes de chaque ball
    bool read_ball_geometry(std::ifstream& file, double& x, double& y, double& r)
    {
        std::string sx, sy, sr;

        if (!read_next_token(file, sx)) return false;
        if (!read_next_token(file, sy)) return false;
        if (!read_next_token(file, sr)) return false;

        x = std::stod(sx);
        y = std::stod(sy);
        r = std::stod(sr);

        return true;
    }

    bool read_ball_delta(std::ifstream& file, double& dx, double& dy)
    {
        std::string sdx, sdy;

        if (!read_next_token(file, sdx)) return false;
        if (!read_next_token(file, sdy)) return false;

        dx = std::stod(sdx);
        dy = std::stod(sdy);

        return true;
    }
    // vérifie que la ball est bien dans l'arène et que la norme de son delta est valide
    bool validate_ball(Ball const& ball, double x, double y, double dx, double dy)
    {
        if (!ball.is_inside_arena()) {
            std::cout << message::ball_outside(x, y);
            return false;
        }

        if (!ball.is_delta_valid()) {
            std::cout << message::invalid_delta(dx, dy);
            return false;
        }

        return true;
    }
    // vérification de chaque ball
    bool read_one_ball(std::ifstream& file, std::vector<Ball>& balls)
    {
        double x = 0.0;
        double y = 0.0;
        double r = 0.0;
        double dx = 0.0;
        double dy = 0.0;

        if (!read_ball_geometry(file, x, y, r)) return false;
        if (!read_ball_delta(file, dx, dy)) return false;

        Ball ball({{x, y}, r}, {dx, dy});

        if (!validate_ball(ball, x, y, dx, dy)) return false;

        balls.push_back(ball);
        return true;
    }
    // lie d'abord le nombre de ball, puis vérifie chaque ball une par une
    bool read_balls(std::ifstream& file, std::vector<Ball>& balls)
    {
        int nb_balls = 0;

        if (!read_balls_count(file, nb_balls)) return false;

        for (int i = 0; i < nb_balls; ++i) {
            if (!read_one_ball(file, balls)) return false;
        }

        return true;
    }
    // versions strictes pour la lecture de fichier (epsil_zero = 0)
    bool intersects_strict(Square s1, Square s2)
    {
        double dx(std::abs(s1.center.x - s2.center.x)
              - (s1.half_size + s2.half_size));
        double dy(std::abs(s1.center.y - s2.center.y)
              - (s1.half_size + s2.half_size));
        if (dx < 0.0 && dy < 0.0) return true;
        return false;
    }

    bool intersects_strict(Circle c1, Circle c2)
    {
        double d(distance(c1.center, c2.center));
        double gap(d - (c1.radius + c2.radius));
        if (gap < 0.0) return true;
        return false;
    }

    bool intersects_strict(Circle c, Square s)
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
}

Game::Game()
    : score(0), lives(0), paddle(), balls(), bricks()
{
}

Game::~Game()
{
    for (auto b : bricks) delete b;
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
    std::ifstream file(filename);   

    if (!file) {
        return false;
    }

    double px = 0.0, py = 0.0, pr = 0.0;

    if (!read_score(file, score)) return false;
    if (!read_lives(file, lives)) return false;
    if (!read_paddle(file, paddle, px, py, pr)) return false;
    if (!read_bricks(file, bricks)) return false;
    if (!read_balls(file, balls)) return false;

    if (!check_bricks_collision()) return false;
    if (!check_paddle_brick()) return false;
    if (!check_balls_collision()) return false;
    if (!check_ball_brick()) return false;
    if (!check_paddle_ball()) return false;

    std::cout << message::success();
    return true;
}