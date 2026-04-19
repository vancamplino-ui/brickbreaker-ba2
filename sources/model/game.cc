// game.cc  : lecture du fichier de jeu et validations initiales
//
// Auteurs  : Liam Van Camp, Victor Henri Willy Eder
// Version  : 1.0 du 27.03.2026
//

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include "game.h"
#include "message.h"
#include "../tools/tools.h"
#include "../tools/constants.h"

namespace
{
    // Les fonctions de lecture restent locales a game.cc pour ne pas surcharger
    // la classe Game avec de nombreuses methodes qui ne servent qu'a load().
    
    bool read_next_token(std::ifstream& file, std::string& token);
    bool read_int(std::ifstream& file, int& value);
    bool read_double(std::ifstream& file, double& value);
    void clear_bricks(std::vector<Brick*>& bricks);
    bool read_score(std::ifstream& file, int& score);
    bool read_lives(std::ifstream& file, int& lives);
    bool read_paddle(std::ifstream& file, Paddle& paddle);
    Brick* make_brick(std::ifstream& file, int type, double x, double y, double c);
    bool create_brick(std::ifstream& file, std::vector<Brick*>& bricks);
    bool read_bricks(std::ifstream& file, std::vector<Brick*>& bricks);
    bool create_ball(std::ifstream& file, std::vector<Ball>& balls);
    bool read_balls(std::ifstream& file, std::vector<Ball>& balls);
    bool load_game_data(std::string const& filename,
                        int& score,
                        int& lives,
                        Paddle& paddle,
                        std::vector<Ball>& balls,
                        std::vector<Brick*>& bricks);
    void write_score(std::ofstream& file, int score);
    void write_lives(std::ofstream& file, int lives);
    void write_paddle(std::ofstream& file, Paddle const& paddle);
    void write_bricks(std::ofstream& file, std::vector<Brick*> const& bricks);
    void write_balls(std::ofstream& file, std::vector<Ball> const& balls);
} // namespace

Game::Game()
    : score(0), lives(0), paddle(), balls(), bricks()
{
}

Game::~Game()
{
    clear_bricks();
}


void Game::clear_bricks()
{
    ::clear_bricks(bricks);
}

bool Game::bricks_intersect()
{
    for (size_t i = 0; i < bricks.size(); ++i) {
        for (size_t j = i + 1; j < bricks.size(); ++j) {
            if (::intersects(bricks[i]->getBody(), bricks[j]->getBody())) {
                std::cout << message::collision_bricks(i, j);
                return true;
            }
        }
    }
    return false;
}

bool Game::paddle_intersects_brick(Paddle const* tested_paddle) const
{
    Paddle const* paddle_to_test = &paddle;
    if (tested_paddle != nullptr) paddle_to_test = tested_paddle;

    for (size_t i = 0; i < bricks.size(); ++i) {
        if (::intersects(paddle_to_test->getArc(), bricks[i]->getBody())) {
            if (tested_paddle == nullptr) {
                std::cout << message::collision_paddle_brick(i);
            }
            return true;
        }
    }
    return false;
}

bool Game::balls_intersect()
{
    for (size_t i = 0; i < balls.size(); ++i) {
        for (size_t j = i + 1; j < balls.size(); ++j) {
            if (::intersects(balls[i].getBody(), balls[j].getBody())) {
                std::cout << message::collision_balls(i, j);
                return true;
            }
        }
    }
    return false;
}

bool Game::ball_intersects_brick()
{
    for (size_t i = 0; i < balls.size(); ++i) {
        for (size_t j = 0; j < bricks.size(); ++j) {
            if (::intersects(balls[i].getBody(), bricks[j]->getBody())) {
                std::cout << message::collision_ball_brick(i, j);
                return true;
            }
        }
    }
    return false;
}

bool Game::paddle_intersects_ball()
{
    for (size_t i = 0; i < balls.size(); ++i) {
        if (::intersects(paddle.getArc(), balls[i].getBody())) {
            std::cout << message::collision_paddle_ball(i);
            return true;
        }
    }
    return false;
}


bool Game::load(std::string const& filename)
{
    int new_score = 0;
    int new_lives = 0;
    Paddle new_paddle;
    std::vector<Ball> new_balls;
    std::vector<Brick*> new_bricks;

    if (!load_game_data(filename,
                        new_score,
                        new_lives,
                        new_paddle,
                        new_balls,
                        new_bricks)) return false;

    score = new_score;
    lives = new_lives;
    paddle = new_paddle;
    balls = new_balls;
    clear_bricks();
    bricks = std::move(new_bricks);

    if (bricks_intersect()) return false;
    if (paddle_intersects_brick()) return false;
    if (balls_intersect()) return false;
    if (ball_intersects_brick()) return false;
    if (paddle_intersects_ball()) return false;

    std::cout << message::success();
    return true;
}

void Game::save(std::string const& filename) const
{
    std::ofstream file(filename);

    if (!file) return;

    write_score(file, score);
    write_lives(file, lives);
    write_paddle(file, paddle);
    write_bricks(file, bricks);
    write_balls(file, balls);
}

void Game::update()
{
    move_paddle_to(paddle.get_target_mouse());
}

bool Game::is_finished() const
{
    return is_won() || is_lost();
}

bool Game::is_won() const
{
    return bricks.empty();
}

bool Game::is_lost() const
{
    return balls.empty() && lives == 0;
}

void Game::reset()
{
    score = 0;
    lives = 0;
    paddle = Paddle();
    balls.clear();
    clear_bricks();
}

int Game::get_score() const
{
    return score;
}

int Game::get_lives() const
{
    return lives;
}

Paddle const& Game::get_paddle() const
{
    return paddle;
}

std::vector<Ball> const& Game::get_balls() const
{
    return balls;
}

std::vector<Brick*> const& Game::get_bricks() const
{
    return bricks;
}

void Game::set_paddle_target(double x)
{
    paddle.set_target_mouse(x);
}

void Game::move_paddle_to(double x)
{
    double current_x = paddle.getArc().center.x;
    double dx = x - current_x;
    double new_x = x;

    if (std::abs(dx) > delta_norm_max) {
        new_x = current_x + ((dx > 0) ? delta_norm_max : -delta_norm_max);
    }

    Paddle candidate(paddle);
    candidate.move_to(new_x);

    if (!candidate.is_valid(epsil_zero)) return;
    if (paddle_intersects_brick(&candidate)) return;

    paddle = candidate;
}

void Game::add_ball_on_paddle()
{
    double top_x = paddle.getArc().center.x;
    double top_y = paddle.getArc().center.y + paddle.getArc().radius;
    balls.push_back(Ball({{top_x, top_y + epsil_zero}, new_ball_radius},
                         {0.0, new_ball_delta_norm}));
    --lives;
}

namespace
{
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

    bool read_int(std::ifstream& file, int& value)
    {
        std::string token;

        if (!read_next_token(file, token)) return false;

        value = std::stoi(token);
        return true;
    }

    bool read_double(std::ifstream& file, double& value)
    {
        std::string token;

        if (!read_next_token(file, token)) return false;

        value = std::stod(token);
        return true;
    }

    void clear_bricks(std::vector<Brick*>& bricks)
    {
        for (Brick* brick : bricks) {
            delete brick;
        }

        bricks.clear();
    }

    bool read_score(std::ifstream& file, int& score)
    {
        if (!read_int(file, score)) return false;

        if (score < 0) {
            std::cout << message::invalid_score(score);
            return false;
        }

        return true;
    }

    bool read_lives(std::ifstream& file, int& lives)
    {
        if (!read_int(file, lives)) return false;

        if (lives < 0) {
            std::cout << message::invalid_lives(lives);
            return false;
        }

        return true;
    }

    bool read_paddle(std::ifstream& file, Paddle& paddle)
    {
        double x = 0.0;
        double y = 0.0;
        double r = 0.0;

        if (!read_double(file, x)) return false;
        if (!read_double(file, y)) return false;
        if (!read_double(file, r)) return false;

        paddle = Paddle({x, y}, r);

        if (!paddle.is_valid()) {
            std::cout << message::paddle_outside(x, y);
            return false;
        }

        return true;
    }

    Brick* make_brick(std::ifstream& file, int type, double x, double y, double c)
    {
        if (type == RAINBOW) {
            int hit_points = 0;

            if (!read_int(file, hit_points)) return nullptr;
            RainbowBrick* brick = new RainbowBrick({{x, y}, c / 2.0}, hit_points);

            if (!brick->is_hit_points_valid()) {
                std::cout << message::invalid_hit_points(hit_points);
                delete brick;
                return nullptr;
            }

            return brick;
        }

        if (type == BALL) return new BallBrick({{x, y}, c / 2.0});
        return new SplitBrick({{x, y}, c / 2.0});
    }

    bool create_brick(std::ifstream& file, std::vector<Brick*>& bricks)
    {
        int type = 0;
        double x = 0.0;
        double y = 0.0;
        double c = 0.0;

        if (!read_int(file, type)) return false;
        if (!read_double(file, x)) return false;
        if (!read_double(file, y)) return false;
        if (!read_double(file, c)) return false;

        if (type < RAINBOW || type > SPLIT) {
            std::cout << message::invalid_brick_type(type);
            return false;
        }

        Brick* brick = make_brick(file, type, x, y, c);

        if (brick == nullptr) return false;

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

        bricks.push_back(brick);
        return true;
    }

    bool read_bricks(std::ifstream& file, std::vector<Brick*>& bricks)
    {
        int nb_bricks = 0;

        if (!read_int(file, nb_bricks)) return false;
        if (nb_bricks < 0) return false;

        for (int i = 0; i < nb_bricks; ++i) {
            if (!create_brick(file, bricks)) return false;
        }

        return true;
    }

    bool create_ball(std::ifstream& file, std::vector<Ball>& balls)
    {
        double x = 0.0;
        double y = 0.0;
        double r = 0.0;
        double dx = 0.0;
        double dy = 0.0;

        if (!read_double(file, x)) return false;
        if (!read_double(file, y)) return false;
        if (!read_double(file, r)) return false;
        if (!read_double(file, dx)) return false;
        if (!read_double(file, dy)) return false;

        Ball ball({{x, y}, r}, {dx, dy});

        if (!ball.is_inside_arena()) {
            std::cout << message::ball_outside(x, y);
            return false;
        }

        if (!ball.is_delta_valid()) {
            std::cout << message::invalid_delta(dx, dy);
            return false;
        }

        balls.push_back(ball);
        return true;
    }

    bool read_balls(std::ifstream& file, std::vector<Ball>& balls)
    {
        int nb_balls = 0;

        if (!read_int(file, nb_balls)) return false;
        if (nb_balls < 0) return false;

        for (int i = 0; i < nb_balls; ++i) {
            if (!create_ball(file, balls)) return false;
        }

        return true;
    }

    bool load_game_data(std::string const& filename,
                        int& score,
                        int& lives,
                        Paddle& paddle,
                        std::vector<Ball>& balls,
                        std::vector<Brick*>& bricks)
    {
        std::ifstream file(filename);

        if (!file) return false;

        if (!read_score(file, score)) return false;
        if (!read_lives(file, lives)) return false;
        if (!read_paddle(file, paddle)) return false;

        if (!read_bricks(file, bricks)) {
            clear_bricks(bricks);
            return false;
        }

        if (!read_balls(file, balls)) {
            clear_bricks(bricks);
            return false;
        }

        return true;
    }

    void write_score(std::ofstream& file, int score)
    {
        file << "# score\n";
        file << score << "\n\n";
    }

    void write_lives(std::ofstream& file, int lives)
    {
        file << "# lives\n";
        file << lives << "\n\n";
    }

    void write_paddle(std::ofstream& file, Paddle const& paddle)
    {
        Circle paddle_arc = paddle.getArc();

        file << "# paddle\n";
        file << paddle_arc.center.x << ' '
             << paddle_arc.center.y << ' '
             << paddle_arc.radius << "\n\n";
    }

    void write_bricks(std::ofstream& file, std::vector<Brick*> const& bricks)
    {
        file << "# bricks\n";
        file << bricks.size() << '\n';

        for (Brick const* brick : bricks) {
            Square body = brick->getBody();
            double size = 2.0 * body.half_size;

            file << brick->getType() << ' '
                 << body.center.x << ' '
                 << body.center.y << ' '
                 << size;

            if (brick->getType() == RAINBOW) {
                RainbowBrick const* rainbow =
                    dynamic_cast<RainbowBrick const*>(brick);

                if (rainbow != nullptr) {
                    file << ' ' << rainbow->getHitPoints();
                }
            }

            file << '\n';
        }
    }

    void write_balls(std::ofstream& file, std::vector<Ball> const& balls)
    {
        file << "\n# balls\n";
        file << balls.size() << '\n';

        for (Ball const& ball : balls) {
            Circle body = ball.getBody();
            Point delta = ball.getDelta();

            file << body.center.x << ' '
                 << body.center.y << ' '
                 << body.radius << ' '
                 << delta.x << ' '
                 << delta.y << '\n';
        }
    }
} // namespace
