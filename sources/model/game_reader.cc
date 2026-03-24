#include "game_reader.h"

#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include "message.h"
#include "../tools/tools.h"

static bool read_next_token(std::ifstream& file, std::string& token)
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

static bool read_score(std::ifstream& file, int& score)
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

static bool read_lives(std::ifstream& file, int& lives)
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

static bool read_paddle(std::ifstream& file, Paddle& paddle)
{
    std::string sx, sy, sr;

    if (!read_next_token(file, sx)) return false;
    if (!read_next_token(file, sy)) return false;
    if (!read_next_token(file, sr)) return false;

    double x = std::stod(sx);
    double y = std::stod(sy);
    double r = std::stod(sr);

    paddle = Paddle({x, y}, r);

    if (!paddle.is_valid()) {
        std::cout << message::paddle_outside(x, y);
        return false;
    }

    return true;
}

static bool read_bricks_count(std::ifstream& file, int& nb_bricks)
{
    std::string token;

    if (!read_next_token(file, token)) return false;

    nb_bricks = std::stoi(token);
    return nb_bricks >= 0;
}

static bool read_brick_type(std::ifstream& file, int& type)
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

static bool read_brick_geometry(std::ifstream& file, double& x, double& y, double& c)
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

static bool read_hit_points(std::ifstream& file, int& hit_points)
{
    std::string token;

    if (!read_next_token(file, token)) return false;

    hit_points = std::stoi(token);
    return true;
}

static Square make_brick_body(double x, double y, double c)
{
    return {{x, y}, c / 2.0};
}

static bool validate_brick(Brick* brick, double x, double y, double c)
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

static bool create_rainbow_brick(std::ifstream& file,
                                 std::vector<Brick*>& bricks,
                                 double x,
                                 double y,
                                 double c)
{
    int hit_points = 0;

    if (!read_hit_points(file, hit_points)) return false;

    RainbowBrick* brick = new RainbowBrick(make_brick_body(x, y, c), hit_points);

    if (!brick->is_hit_points_valid()) {
        std::cout << message::invalid_hit_points(hit_points);
        delete brick;
        return false;
    }

    if (!validate_brick(brick, x, y, c)) return false;

    bricks.push_back(brick);
    return true;
}

static bool create_ball_brick(std::vector<Brick*>& bricks, double x, double y, double c)
{
    BallBrick* brick = new BallBrick(make_brick_body(x, y, c));

    if (!validate_brick(brick, x, y, c)) return false;

    bricks.push_back(brick);
    return true;
}

static bool create_split_brick(std::vector<Brick*>& bricks, double x, double y, double c)
{
    SplitBrick* brick = new SplitBrick(make_brick_body(x, y, c));

    if (!validate_brick(brick, x, y, c)) return false;

    bricks.push_back(brick);
    return true;
}

static bool read_one_brick(std::ifstream& file, std::vector<Brick*>& bricks)
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

static bool read_bricks(std::ifstream& file, std::vector<Brick*>& bricks)
{
    int nb_bricks = 0;

    if (!read_bricks_count(file, nb_bricks)) return false;

    for (int i = 0; i < nb_bricks; ++i) {
        if (!read_one_brick(file, bricks)) return false;
    }

    return true;
}

static bool read_balls_count(std::ifstream& file, int& nb_balls)
{
    std::string token;

    if (!read_next_token(file, token)) return false;

    nb_balls = std::stoi(token);
    return nb_balls >= 0;
}

static bool read_ball_geometry(std::ifstream& file, double& x, double& y, double& r)
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

static bool read_ball_delta(std::ifstream& file, double& dx, double& dy)
{
    std::string sdx, sdy;

    if (!read_next_token(file, sdx)) return false;
    if (!read_next_token(file, sdy)) return false;

    dx = std::stod(sdx);
    dy = std::stod(sdy);
    return true;
}

static bool validate_ball(Ball const& ball, double x, double y, double dx, double dy)
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

static bool read_one_ball(std::ifstream& file, std::vector<Ball>& balls)
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

static bool read_balls(std::ifstream& file, std::vector<Ball>& balls)
{
    int nb_balls = 0;

    if (!read_balls_count(file, nb_balls)) return false;

    for (int i = 0; i < nb_balls; ++i) {
        if (!read_one_ball(file, balls)) return false;
    }

    return true;
}

GameData::GameData(GameData&& other) noexcept
    : score(other.score), lives(other.lives), paddle(other.paddle),
      balls(std::move(other.balls)), bricks(std::move(other.bricks))
{
    other.bricks.clear();
}

GameData& GameData::operator=(GameData&& other) noexcept
{
    if (this != &other) {
        clear();
        score = other.score;
        lives = other.lives;
        paddle = other.paddle;
        balls = std::move(other.balls);
        bricks = std::move(other.bricks);
        other.bricks.clear();
    }

    return *this;
}

GameData::~GameData()
{
    clear();
}

void GameData::clear()
{
    for (Brick* brick : bricks) {
        delete brick;
    }

    bricks.clear();
    balls.clear();
    score = 0;
    lives = 0;
    paddle = Paddle();
}

bool load_file(std::string const& filename, GameData& data)
{
    std::ifstream file(filename);

    if (!file) {
        return false;
    }

    data.clear();

    if (!read_score(file, data.score)) {
        data.clear();
        return false;
    }

    if (!read_lives(file, data.lives)) {
        data.clear();
        return false;
    }

    if (!read_paddle(file, data.paddle)) {
        data.clear();
        return false;
    }

    if (!read_bricks(file, data.bricks)) {
        data.clear();
        return false;
    }

    if (!read_balls(file, data.balls)) {
        data.clear();
        return false;
    }

    return true;
}
