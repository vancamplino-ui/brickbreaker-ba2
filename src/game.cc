#include "game.h"
#include "message.h"
#include "constants.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;


namespace {
    // Affiche le message d'erreur et retourne false
    bool print_error(string message) {
        cerr << message;
        return false;
    }

    // Trouve la prochaine ligne valide et utile
    bool nextLine(ifstream &file, string &line) {
        while (file >> ws && getline(file, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }
            return true;
        }
        return false;
    }

    // Vérifie s'il y a trop d'informations sur la ligne
    bool verif_extra(stringstream &ss) {
        string extra;
        if (ss >> extra) { return true; }
        return false;
    }
}


// =============================================================================
// Constructeur et Destructeur
// =============================================================================

Game::Game() :
    status(ONGOING),
    score(0),
    lives(0),
    arena(Point(arena_size / 2.0, arena_size / 2.0), arena_size),
    mousePosition(0.0, 0.0),
    paddle(nullptr) 
{}

Game::~Game() = default;


// =============================================================================
// CHARGEMENT DES DONNÉES
// =============================================================================

bool Game::loadFile(const string &filename) { 
    ifstream file(filename);
    reset();

    // Vérifie l'ouverture du fichier
    if (!file.is_open()) { return false; }

    // Charge et vérifie toutes les informations du fichier .txt
    if (!loadGlobal(file) || 
        !loadPaddle(file) ||
        !loadBricks(file) || 
        !loadBalls(file)  || 
        !validateCollisions()) {
        reset(); /// remise à zéro si le fichier est mauvais
        return false;
    }

    // Vérifie qu'il ne reste aucune donnée non traitée à la fin du fichier
    string line;
    if (nextLine(file, line)) { 
        reset(); 
        return false; 
    }

    file.close();

    cout << message::success();

    // Actualise le statut de la partie (si le jeu est initialisé WON ou LOST)
    updateStatus();

    return true;
}

bool Game::loadGlobal(ifstream &file) {
    string line;

    // Charge le score
    if (!nextLine(file, line)) { return false; }
    stringstream ss_score(line);
    if (!(ss_score >> score)) { return false; }
    if (verif_extra(ss_score)) { return false; }

    // Charge les vies
    if (!nextLine(file, line)) { return false; }
    stringstream ss_lives(line);
    if (!(ss_lives >> lives)) { return false; }
    if (verif_extra(ss_lives)) { return false; }
    
    return validateGlobal();
}

bool Game::loadPaddle(ifstream &file) {
    string line;

    // Charge la raquette
    if (!nextLine(file, line)) { return false; }
    stringstream ss(line);
    double x, y, r;
    if (!(ss >> x >> y >> r)) { return false; }
    if (verif_extra(ss)) { return false; }
    paddle = make_unique<Paddle>(x, y, r);

    return validatePaddle();
}

bool Game::loadBricks(ifstream &file) {
    string line;

    // Charge le nombre de briques
    if (!nextLine(file, line)) { return false; }
    int nb_bricks;
    stringstream ss_nb(line);
    if (!(ss_nb >> nb_bricks) || nb_bricks < 0) { return false; }
    if (verif_extra(ss_nb)) { return false; }

    // Charge toutes les briques
    for (int i = 0; i < nb_bricks; ++i) {
        if (!nextLine(file, line)) { return false; }
        stringstream ss(line);
        int type;
        double x, y, c;
        if (!(ss >> type >> x >> y >> c)) return false;

        // Charge les hitpoints en plus si de type 0 (RAINBOW)
        if (type == RAINBOW) {
            int h;
            // Vérifie si l'info des hitpoints est présente
            if (!(ss >> h)) { return false; }
            bricks.push_back(make_unique<Rainbow_brick>(x, y, c, h));
        }
        // Charge sans les hitpoints
        else if (type == BALL) {
            bricks.push_back(make_unique<Ball_brick>(x, y, c));
        }
        else if (type == SPLIT) {
            bricks.push_back(make_unique<Split_brick>(x, y, c));
        }
        else { return print_error(message::invalid_brick_type(type)); }

        if (verif_extra(ss)) { return false; }
    }

    return validateBricks();
}

bool Game::loadBalls(ifstream &file) {
    string line;

    // Charge le nombre de balles
    if (!nextLine(file, line)) { return false; }
    int nb_balls;
    stringstream ss_nb_balls(line);
    if (!(ss_nb_balls >> nb_balls) || nb_balls < 0) { return false; }
    if (verif_extra(ss_nb_balls)) { return false; }

    // Charge toutes les balles
    for (int i = 0; i < nb_balls; ++i) {
        if (!nextLine(file, line)) { return false; }
        stringstream ss(line);
        double x, y, r, dx, dy;
        if (!(ss >> x >> y >> r >> dx >> dy)) { return false;}
        if (verif_extra(ss)) { return false; }
        balls.push_back(make_unique<Ball>(x, y, r, dx, dy));
    }

    return validateBalls();
}


// =============================================================================
// VALIDATION DES DONNÉES
// =============================================================================

bool Game::validateGlobal() const {
    // Vérifie le score et les vies
    if (score < 0) { return print_error(message::invalid_score(score)); }
    if (lives < 0) { return print_error(message::invalid_lives(lives)); }

    return true;
}

bool Game::validatePaddle() const {
    double paddle_x = paddle->getCenter().getX();
    double paddle_y = paddle->getCenter().getY();
    double paddle_r = paddle->getRadius();

    // Vérifie (y ≤ 0)
    if (paddle_y > 0) {
        return print_error(message::paddle_outside(paddle_x, paddle_y));
    }

    // Vérifie (y + rayon > 0)
    if (paddle_y + paddle_r <= 0) {
        return print_error(message::paddle_outside(paddle_x, paddle_y));
    }

    // Vérifie que les extrémités de la raquette sont dans [0, arena_size]
    if (!contains_Arc(arena, paddle->getCircle(), 0)){ 
        return print_error(message::paddle_outside(paddle_x, paddle_y)); 
    }

    return true;
}

bool Game::validateBricks() const {
    // Parcoure toutes les briques
    for (const auto& b : bricks) {
        // Vérifie la taille minimale
        if (b->getSize() < brick_size_min) {
            return print_error(message::invalid_brick_size(b->getSize()));
        }

        // Vérifie l'inclusion dans l'arène
        if (!contains(arena, b->getSquare(), 0)) {
            return print_error(
                message::brick_outside(b->getCenter().getX(), b->getCenter().getY()));
        }

        // Vérifie le bon type
        int type = b->getType();
        if (type != RAINBOW && type != BALL && type != SPLIT) {
            return print_error(message::invalid_brick_type(type));
        }

        // Vérifie le bon hit_point si RAINBOW
        if (b->getType() == RAINBOW) {
            if (b->getHitPoints() < MIN_HIT_POINTS || 
                b->getHitPoints() > MAX_HIT_POINTS) {
                return print_error(message::invalid_hit_points(b->getHitPoints()));
            }
        }
    }

    return true;
}

bool Game::validateBalls() const {
    // Parcoure toutes les balles
    for (const auto& b : balls) {
        // Vérifie la norme du delta
        Point delta = b->getDelta();
        if (delta.norm() > delta_norm_max) {
            return print_error(message::invalid_delta(delta.getX(), delta.getY()));
        }

        double x = b->getCenter().getX();
        double y = b->getCenter().getY();
        
        // Vérifie l'inclusion dans l'arène (sauf bord inférieur)
        if (!contains(arena, b->getCircle(), 0)){
            return print_error(message::ball_outside(x, y));
        }
    }

    return true;
}

bool Game::validateBricksCollisions() const {
    // Vérifie les collisions Brique-Brique
    for (size_t i = 0; i < bricks.size(); ++i) {
        for (size_t j = i + 1; j < bricks.size(); ++j) {
            if (intersect(bricks[i]->getSquare(), bricks[j]->getSquare(), 0)) {
                return print_error(message::collision_bricks(i, j));
            }
        }
    }

    return true;
}

bool Game::validatePaddleBrickCollisions() const {
    // Vérifie les collisions Brique-Raquette
    if (paddle) {
        for (size_t i = 0; i < bricks.size(); ++i) {
            if (intersect(paddle->getCircle(), bricks[i]->getSquare(), 0)) {
                return print_error(message::collision_paddle_brick(i));
            }
        }
    }

    return true;
}

bool Game::validateBallsCollisions() const {
    // Vérifie les collisions Balle-Balle
    for (size_t i = 0; i < balls.size(); ++i) {
        for (size_t j = i + 1; j < balls.size(); ++j) {
            if (intersect(balls[i]->getCircle(), balls[j]->getCircle(), 0)) {
                return print_error(message::collision_balls(i, j));
            }
        }
    }

    return true;
}

bool Game::validateBallBrickCollisions() const {
    // Vérifie les collisions Balle-Brique
    for (size_t i = 0; i < balls.size(); ++i) {
        for (size_t j = 0; j < bricks.size(); ++j) {
            if (intersect(balls[i]->getCircle(), bricks[j]->getSquare(), 0)) {
                return print_error(message::collision_ball_brick(i, j));
            }
        }
    }

    return true;
}

bool Game::validateBallPaddleCollisions() const {
    // Vérifie les collisions Balle-Raquette
    if (paddle) {
        for (size_t i = 0; i < balls.size(); ++i) {
            if (intersect(balls[i]->getCircle(), paddle->getCircle(), 0)) {
                return print_error(message::collision_paddle_ball(i));
            }
        }
    }

    return true;
}

bool Game::validateCollisions() const {
    // Valide l'absence de toutes les collisions possibles
    return validateBricksCollisions() &&
           validatePaddleBrickCollisions() &&
           validateBallsCollisions() &&
           validateBallBrickCollisions() &&
           validateBallPaddleCollisions();
}


// =============================================================================
// FONCTIONS UTILITAIRES DE GAME
// =============================================================================

void Game::reset() {
    // Réinitialise le jeu
    score = 0;
    lives = 0;
    paddle.reset();
    bricks.clear();
    balls.clear();
    status = ONGOING;
}

// Sauvegarde les données du jeu dans un fichier .txt
bool Game::saveFile(const std::string &filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    file << "# Save\n\n# score\n" << score << "\n\n# lives\n" << lives << "\n\n";

    if (paddle) {
        file << "# paddle\n" << paddle->getCenter().getX() << " " 
             << paddle->getCenter().getY() << " " << paddle->getRadius() << "\n\n";
    }

    file << "# bricks\n" << bricks.size() << "\n";
    for (const auto& b : bricks) {
        file << b->getType() << " " << b->getCenter().getX() << " " 
             << b->getCenter().getY() << " " << b->getSize();
        if (b->getType() == 0) file << " " << b->getHitPoints();
        file << "\n";
    }

    file << "\n# balls\n" << balls.size() << "\n";
    for (const auto& b : balls) {
        file << b->getCenter().getX() << " " << b->getCenter().getY() 
             << " " << b->getRadius() << "\n";
    }

    return true; 
}

// Dessine le jeu
void Game::draw() const {
    arena.draw_Border(GREY);
    if (paddle) { paddle->draw(); } 
    for (const auto& b : bricks) { b->draw(); }
    for (const auto& b : balls) { b->draw(); }
}


// =============================================================================
// FONCTIONS DYNAMIQUE DE GAME
// =============================================================================

// Met à jour la dynamique du jeu
void Game::update() {
    // Vecteurs temporaires pour stocker les briques/balles crées
    std::vector<std::unique_ptr<Brick>> bricks_to_add;   
    std::vector<std::unique_ptr<Ball>> balls_to_add; 

    // Déplace la balle et vérifie les collisions suite à son déplacement
    checkBallCollision(bricks_to_add, balls_to_add);

    // Déplace la raquette
    movePaddle();

    // Si la raquette tape une balle, vérifie les collisions engendrées
    checkBallRebound(bricks_to_add, balls_to_add);

    // Ajoute au jeu les nouvelles briques/balles crées
    for (auto& br : bricks_to_add) { bricks.push_back(std::move(br)); }
    for (auto& b : balls_to_add) balls.push_back(std::move(b));

    // Actualise le statut de la partie
    updateStatus();
}

// Actualise le statut de la partie
void Game::updateStatus() {
    if (balls.empty() && lives == 0) {
        status = LOST;
    } 
    else if (bricks.empty()) {
        // Ajuste le score final en récompensant par vies en trop
        score += lives * score_per_life; 
        lives = 0;
        status = WON;
    }
    else {
        status = ONGOING;
    }
}

// Gère les collisions de toutes les balles
void Game::checkBallCollision(std::vector<std::unique_ptr<Brick>>& bricks_to_add,
                              std::vector<std::unique_ptr<Ball>>& balls_to_add) {
    
    for (auto it = balls.begin(); it != balls.end();) {
        Ball& ball = **it;
        bool fell = false;

        // Sous-échantillonnage du déplacement avec n_step pour plus de stabilité
        // Permet d'éviter l'effet tunnel en divisant le vecteur delta
        for (int step = 0; step < n_steps; ++step) {
            Point c = ball.getCircle().getCenter();
            Point d = ball.getDelta();
            double fraction = 1.0 / n_steps;
      
            Circle testBall(Point(c.getX() + d.getX() * fraction, 
                                  c.getY() + d.getY() * fraction), ball.getRadius());

            // Teste si la balle tombe en dessous de l'arène
            if (testBall.getCenter().getY() < arena.getCenter().getY() - arena_size/2)
            {
                fell = true;
                break;
            }

            // Teste les collisions avec les autes entités
            testWhileCollision(ball, testBall, fraction, bricks_to_add, balls_to_add);
        }   
        if (fell) {
            // Détruit la balle et renvoie l'itérateur vers l'élément suivant
            it = balls.erase(it);
        } else {
            ++it;
        }
    }
}

// Déplace la raquette à la position x de la souris
void Game::movePaddle() {
    if (!paddle) return;

    // Calcule le nouveau x suivant la logique de déplacement
    double x_jeu = mousePosition.getX();
    double x_paddle = paddle->getCenter().getX();
    double dx = x_jeu - x_paddle; 
    double move_to_x = 0;

    // Vérifie la limite du vecteur vitesse de la raquette
    if (abs(dx) <= delta_norm_max) { 
        move_to_x = x_jeu; 
        paddle->setDelta(Point(dx,0)); ///
    } 
    else { 
        double realDelta = (dx > 0) ? delta_norm_max : -delta_norm_max;
        move_to_x = x_paddle + realDelta; 
        paddle->setDelta(Point(realDelta,0)); ///
    }

    // Déplace la raquette avec le nouveau x
    paddle->move(move_to_x, arena, bricks);
}

// Gère les collisions de toutes les balles si rebond avec la raquette
void Game::checkBallRebound(std::vector<std::unique_ptr<Brick>>& bricks_to_add,
                            std::vector<std::unique_ptr<Ball>>& balls_to_add){
    for (auto& ball_ptr : balls) {
        Ball& ball = *ball_ptr; 
        // Vérification de la collision avec la raquette
        if (paddle && ball.checkCollisionPaddle(paddle->getCircle(), 
                                        paddle->getDelta(), ball.getCircle())) {
            // Déplace la balle avec rebond sur la raquette 
            double fraction = 1.0;
            Circle testBall = ball.test_circle(); 
            
            // Teste les collisions avec les autes entités
            testWhileCollision(ball, testBall, fraction, bricks_to_add, balls_to_add);
        }
    }
}

// Gère tous les rebonds et collisions (tant qu'il y a collision et nb_bounce_max)
void Game::testWhileCollision(Ball& ball, Circle& testBall, double step_fraction,
                              std::vector<std::unique_ptr<Brick>>& bricks_to_add,
                              std::vector<std::unique_ptr<Ball>>& balls_to_add) {
    int nb_rebonds = 0;
    bool collision = true;

    while (collision && nb_rebonds < nb_bounce_max) {
        collision = false;
        
        // Vérifie les collisions avec les bords de l'arène (haut, gauche, droite)
        if (ball.reboundWall(arena, testBall)) {
            collision = true;
            nb_rebonds++;
        }

        // Vérifie les collisions avec la raquette
        if (paddle && ball.checkCollisionPaddle(paddle->getCircle(), 
                                                paddle->getDelta(), testBall)) {
            collision = true;
            nb_rebonds++;
        }

        checkBrickCollision(ball, testBall, collision, nb_rebonds, 
                            bricks_to_add, balls_to_add);
        
        checkOtherBallCollision(ball, testBall, collision, nb_rebonds);
        
        if (collision) {
            // Recalcule testBall pour la MÊME sous-étape mais avec le NOUVEAU delta
            Point c = ball.getCircle().getCenter();
            Point d = ball.getDelta();
            testBall = Circle(Point(c.getX() + d.getX() * step_fraction,
                            c.getY() + d.getY() * step_fraction), ball.getRadius());
        }
    }
    // MISE À JOUR PHYSIQUE FINALE : une seule fois (en dehors de la boucle while)
    ball.setCircle(testBall);
}

// Gère les collisions d'une balle avec toutes les briques
void Game::checkBrickCollision(Ball& ball, Circle& testBall,
                               bool& collision, int& nb_rebonds,
                               std::vector<std::unique_ptr<Brick>>& bricks_to_add,
                               std::vector<std::unique_ptr<Ball>>& balls_to_add) {
    for (auto bit = bricks.begin(); bit != bricks.end();) {
        Brick& brick = **bit;
        // Garde en mémoire l'ancien delta pour la Ball_brick
        Point lastDelta = ball.getDelta();

        if (ball.checkCollisionBrick(brick.getSquare(), testBall)) {
            collision = true;
            nb_rebonds++;
            
            // Gère la brique touchée   
            if (brick.hit()) {
                // Gère la conséquence de la collision propre au type de la brique
                onBrickHit(lastDelta, brick, bricks_to_add, balls_to_add);

                // Ajoute les points de la destruction de la brique au score
                score += score_per_hit;
                bit = bricks.erase(bit);
            } 
            // Si brique touchée mais pas détruite (ex: Rainbow avec bcp de HP)
            else { 
                ++bit; 
            }
            // On sort de la boucle for car la balle a rebondi 
            break;
        } 
        else { 
            ++bit; 
        }
    }
}

// Gère la collision d'une balle avec toutes les autres balles
void Game::checkOtherBallCollision(Ball& ball, Circle& testBall, 
                                   bool& collision, int& nb_rebonds) {
        // Parcoure toutes les balles
        for (auto it2 = balls.begin(); it2 != balls.end(); ++it2) {
        Ball& ball2 = **it2;

        // Vérifie que ce n'est pas la même instance
        if (&ball == &ball2) { continue; }

        if (ball.checkCollisionBall(ball2, testBall)) {
            collision = true;
            nb_rebonds++;
        }
    }
}

// Gère la conséquence de la collision propre au type de la brique
void Game::onBrickHit(const Point& lastDelta, const Brick& brick,
                      std::vector<std::unique_ptr<Brick>>& bricks_to_add,
                      std::vector<std::unique_ptr<Ball>>& balls_to_add) {
    // Ajoute une balle si de type BALL
    if (brick.getType() == BALL) {
        balls_to_add.push_back(make_unique<Ball>(
            brick.getCenter().getX(), brick.getCenter().getY(), new_ball_radius, 
            lastDelta.getX(), lastDelta.getY()));
    }
    // Fragmente la balle si de type SPLIT
    else if (brick.getType() == SPLIT) {
        double old_size = brick.getSize();
        double next_size = (old_size - split_brick_gap) / 2.0;

        // Vérifie si la division est possible selon les constantes
        if (next_size >= brick_size_min) {
            double x = brick.getCenter().getX();
            double y = brick.getCenter().getY();
            double offset = (old_size - next_size) / 2.0;

            // Ajoute les 4 briques aux coins
            bricks_to_add.push_back(make_unique<Split_brick>(
                x - offset, y - offset, next_size));
            bricks_to_add.push_back(make_unique<Split_brick>(
                x + offset, y - offset, next_size));
            bricks_to_add.push_back(make_unique<Split_brick>(
                x - offset, y + offset, next_size));
            bricks_to_add.push_back(make_unique<Split_brick>(
                x + offset, y + offset, next_size));
        }
    }
}

// Ajoute une balle au jeu
void Game::addBall(double x, double y, double r, double dx, double dy) {
    balls.push_back(std::make_unique<Ball>(x, y, r, dx, dy));
}

// Génère une nouvelle balle sur la raquette
void Game::generateBall() {
    if (paddle) {
        double x_newBall = paddle->getCenter().getX(); 

        // Calcul du positionnement vertical parfait 
        double y_newBall = paddle->getCenter().getY() + paddle->getRadius() 
                                                      + new_ball_radius + epsil_zero; 
        
        addBall(x_newBall, y_newBall, new_ball_radius, 0.0, new_ball_delta_norm); 
        --lives;
    } 
}



