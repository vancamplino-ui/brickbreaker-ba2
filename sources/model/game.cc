#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include "game.h"
#include "message.h"
#include "../tools/tools.h"
#include "../tools/constants.h"

static bool read_next_token(std::ifstream& file, std::string& token);
static bool read_score(std::ifstream& file, int& score);
static bool read_lives(std::ifstream& file, int& lives);
static bool read_paddle(std::ifstream& file, Paddle& paddle);
static bool read_bricks_count(std::ifstream& file, int& nb_bricks);
static bool read_brick_type(std::ifstream& file, int& type);
static bool read_brick_geometry(std::ifstream& file,
                                double& x,
                                double& y,
                                double& c);
static bool read_hit_points(std::ifstream& file, int& hit_points);
static Square make_brick_body(double x, double y, double c);
static bool validate_brick(Brick* brick, double x, double y, double c);
static bool create_rainbow_brick(std::ifstream& file,
                                 std::vector<Brick*>& bricks,
                                 double x,
                                 double y,
                                 double c);
static bool create_ball_brick(std::vector<Brick*>& bricks,
                              double x,
                              double y,
                              double c);
static bool create_split_brick(std::vector<Brick*>& bricks,
                               double x,
                               double y,
                               double c);
static bool read_one_brick(std::ifstream& file, std::vector<Brick*>& bricks);
static bool read_bricks(std::ifstream& file, std::vector<Brick*>& bricks);
static bool read_balls_count(std::ifstream& file, int& nb_balls);
static bool read_ball_geometry(std::ifstream& file,
                               double& x,
                               double& y,
                               double& r);
static bool read_ball_delta(std::ifstream& file, double& dx, double& dy);
static bool validate_ball(Ball const& ball,
                          double x,
                          double y,
                          double dx,
                          double dy);
static bool read_one_ball(std::ifstream& file, std::vector<Ball>& balls);
static bool read_balls(std::ifstream& file, std::vector<Ball>& balls);
namespace
{
// Les fonctions de lecture restent locales a game.cc pour ne pas surcharger
// la classe Game avec de nombreuses methodes de parsing qui ne servent qu'a load().
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

void clear_bricks(std::vector<Brick*>& bricks)
{
    for (Brick* brick : bricks) {
        delete brick;
    }

    bricks.clear();
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

bool read_paddle(std::ifstream& file, Paddle& paddle)
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

static bool read_brick_geometry(std::ifstream& file,
                                double& x,
                                double& y,
                                double& c)
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

bool create_rainbow_brick(std::vector<Brick*>& bricks,
                          double x,
                          double y,
                          double c,
                          int hit_points)
{
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

bool create_ball_brick(std::vector<Brick*>& bricks,
                              double x,
                              double y,
                              double c)
{
    BallBrick* brick = new BallBrick(make_brick_body(x, y, c));

    if (!validate_brick(brick, x, y, c)) return false;

    bricks.push_back(brick);
    return true;
}

bool create_split_brick(std::vector<Brick*>& bricks,
                               double x,
                               double y,
                               double c)
{
    SplitBrick* brick = new SplitBrick(make_brick_body(x, y, c));

    if (!validate_brick(brick, x, y, c)) return false;

    bricks.push_back(brick);
    return true;
}

bool read_one_brick(std::ifstream& file, std::vector<Brick*>& bricks)
{
    std::string stype, sx, sy, sc;
    int type(0), hit_points(0);
    double x(0.0), y(0.0), c(0.0);

    if (!read_next_token(file, stype)) return false;
    if (!read_next_token(file, sx)) return false;
    if (!read_next_token(file, sy)) return false;
    if (!read_next_token(file, sc)) return false;

    type = std::stoi(stype);
    x = std::stod(sx);
    y = std::stod(sy);
    c = std::stod(sc);

    if (type < RAINBOW || type > SPLIT) {
        std::cout << message::invalid_brick_type(type);
        return false;
    }

    if (type == RAINBOW) {
        std::string token;

        if (!read_next_token(file, token)) return false;

        hit_points = std::stoi(token);
        return create_rainbow_brick(bricks, x, y, c, hit_points);
    }

    if (type == BALL) return create_ball_brick(bricks, x, y, c);
    return create_split_brick(bricks, x, y, c);
}

bool read_bricks(std::ifstream& file, std::vector<Brick*>& bricks)
{
    std::string token;
    int nb_bricks = 0;

    if (!read_next_token(file, token)) return false;

    nb_bricks = std::stoi(token);
    if (nb_bricks < 0) return false;

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

static bool read_ball_geometry(std::ifstream& file,
                               double& x,
                               double& y,
                               double& r)
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

bool validate_ball(Ball const& ball,
                          double x,
                          double y,
                          double dx,
                          double dy)
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

bool read_one_ball(std::ifstream& file, std::vector<Ball>& balls)
{
    std::string sx, sy, sr, sdx, sdy;
    double x = 0.0;
    double y = 0.0;
    double r = 0.0;
    double dx = 0.0;
    double dy = 0.0;

    if (!read_next_token(file, sx)) return false;
    if (!read_next_token(file, sy)) return false;
    if (!read_next_token(file, sr)) return false;
    if (!read_next_token(file, sdx)) return false;
    if (!read_next_token(file, sdy)) return false;

    x = std::stod(sx);
    y = std::stod(sy);
    r = std::stod(sr);
    dx = std::stod(sdx);
    dy = std::stod(sdy);

    Ball ball({{x, y}, r}, {dx, dy});

    if (!validate_ball(ball, x, y, dx, dy)) return false;

    balls.push_back(ball);
    return true;
}

bool read_balls(std::ifstream& file, std::vector<Ball>& balls)
{
    std::string token;
    int nb_balls = 0;

    if (!read_next_token(file, token)) return false;

    nb_balls = std::stoi(token);
    if (nb_balls < 0) return false;

    for (int i = 0; i < nb_balls; ++i) {
        if (!read_one_ball(file, balls)) return false;
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

bool Game::intersects(Square s1, Square s2)
{
    double dx(std::abs(s1.center.x - s2.center.x)
          - (s1.half_size + s2.half_size));
    double dy(std::abs(s1.center.y - s2.center.y)
          - (s1.half_size + s2.half_size));
    return dx < 0.0 && dy < 0.0;
}

bool Game::intersects(Circle c1, Circle c2)
{
    double d(distance(c1.center, c2.center));
    double gap(d - (c1.radius + c2.radius));
    return gap < 0.0;
}

bool Game::intersects(Circle c, Square s)
{
    Point min{s.center.x - s.half_size, s.center.y - s.half_size};
    Point max{s.center.x + s.half_size, s.center.y + s.half_size};
    Point closest{
        std::clamp(c.center.x, min.x, max.x),
        std::clamp(c.center.y, min.y, max.y)
    };
    double d(distance(c.center, closest));
    double gap(d - c.radius);
    return gap < 0.0;
}

bool Game::bricks_intersect()
{
    for (size_t i = 0; i < bricks.size(); ++i) {
        for (size_t j = i + 1; j < bricks.size(); ++j) {
            if (intersects(bricks[i]->getBody(), bricks[j]->getBody())) {
                std::cout << message::collision_bricks(i, j);
                return true;
            }
        }
    }
    return false;
}

bool Game::paddle_intersects_brick()
{
    for (size_t i = 0; i < bricks.size(); ++i) {
        if (intersects(paddle.getArc(), bricks[i]->getBody())) {
            std::cout << message::collision_paddle_brick(i);
            return true;
        }
    }
    return false;
}

bool Game::balls_intersect()
{
    for (size_t i = 0; i < balls.size(); ++i) {
        for (size_t j = i + 1; j < balls.size(); ++j) {
            if (intersects(balls[i].getBody(), balls[j].getBody())) {
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
            if (intersects(balls[i].getBody(), bricks[j]->getBody())) {
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
        if (intersects(paddle.getArc(), balls[i].getBody())) {
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