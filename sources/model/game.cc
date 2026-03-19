#include "game.h"
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
}

Game::Game()
    : score(0), lives(0), paddle(), bricks(), balls()
{
}

bool Game::load(std::string const& filename)
{
    std::ifstream file(filename);   

    if (!file) {
        return false;
    }

    int tmp_score = 0;
    int tmp_lives = 0;
    double px = 0.0, py = 0.0, pr = 0.0;

    // read score
    if (!read_score(file, score)) return false;

    // read lives
    if (!read_lives(file, lives)) return false;

    // read paddle
    if (!read_paddle(file, paddle, px, py, pr)) return false;

    return true;
}