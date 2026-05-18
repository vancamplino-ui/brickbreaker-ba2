#ifndef GAME_H
#define GAME_H

#include "ball.h"
#include "brick.h"
#include "paddle.h"
#include "message.h"
#include "constants.h"
#include <memory>
#include <string>
#include <vector>
#include <fstream>

enum Status {
    LOST = 0,
    WON = 1,
    ONGOING = 2
};

// Gère la logique principale du jeu, le chargement et la validation des entités.
class Game {
private:
    // Attributs
    Status status;
    int score;
    int lives;
    Square arena;
    Point mousePosition;
    std::unique_ptr<Paddle> paddle;
    std::vector<std::unique_ptr<Brick>> bricks;
    std::vector<std::unique_ptr<Ball>> balls;

    // Méthodes privées de chargement
    bool loadGlobal(std::ifstream &file);
    bool loadPaddle(std::ifstream &file);
    bool loadBricks(std::ifstream &file);
    bool loadBalls(std::ifstream &file);

    // Méthodes de validation interne
    bool validateGlobal() const;
    bool validatePaddle() const;
    bool validateBricks() const;
    bool validateBalls() const;
    bool validateBricksCollisions() const;
    bool validatePaddleBrickCollisions() const;
    bool validateBallsCollisions() const;
    bool validateBallBrickCollisions() const;
    bool validateBallPaddleCollisions() const;
    bool validateCollisions() const;

    // Méthodes privées de dynamique du jeu
    void checkBallCollision(std::vector<std::unique_ptr<Brick>>& bricks_to_add,
                            std::vector<std::unique_ptr<Ball>>& balls_to_add);
    void movePaddle();
    void checkBallRebound(std::vector<std::unique_ptr<Brick>>& bricks_to_add,
                          std::vector<std::unique_ptr<Ball>>& balls_to_add);
    void testWhileCollision(Ball& ball, Circle& testBall, double step_fraction,
                            std::vector<std::unique_ptr<Brick>>& bricks_to_add,
                            std::vector<std::unique_ptr<Ball>>& balls_to_add);
    void checkBrickCollision(Ball& ball, Circle& testBall, 
                             bool& collision, int& nb_rebonds,
                             std::vector<std::unique_ptr<Brick>>& bricks_to_add,
                             std::vector<std::unique_ptr<Ball>>& balls_to_add);
    void checkOtherBallCollision(Ball& ball, Circle& testBall,
                                 bool& collision, int& nb_rebonds);
    void onBrickHit(const Point& lastDelta, const Brick& brick,
                    std::vector<std::unique_ptr<Brick>>& bricks_to_add,
                    std::vector<std::unique_ptr<Ball>>& balls_to_add);


public:
    // Constructeur et Destructeur
    Game();
    ~Game();

    // Chargement et écriture des données
    bool loadFile(const std::string &filename);
    bool saveFile(const std::string &filename);

    // Getters
    Status getStatus() const { return status; } ///
    int getScore() const { return score; }
    int getLives() const { return lives; }
    int getNbBricks() const { return bricks.size(); }
    int getNbBalls() const { return balls.size(); }
    const Square& getArena() const { return arena; }
    Paddle* getPaddle() const { return paddle.get(); }
    const std::vector<std::unique_ptr<Brick>>& getBricks() const { return bricks; }
    const std::vector<std::unique_ptr<Ball>>& getBalls() const { return balls; }
    
    // Setters
    void setLives(int new_lives) { lives = new_lives; }
    void setMouseX(double x) { mousePosition.setX(x); }

    // Dessin du jeu
    void draw() const;

    // Reset du game
    void reset();

    // Méthodes de dynamique du jeu
    void update();
    void updateStatus();
    void addBall(double x, double y, double r, double dx, double dy);
    void generateBall();
};

#endif
