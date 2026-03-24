#ifndef GAME_READER_H
#define GAME_READER_H

#include <string>
#include <vector>

#include "ball.h"
#include "brick.h"
#include "paddle.h"

struct GameData
{
    int score = 0;
    int lives = 0;
    Paddle paddle;
    std::vector<Ball> balls;
    std::vector<Brick*> bricks; // pointeurs a liberer manuellement
    
    // Construction simple d'une structure temporaire de lecture.
    GameData() = default;
    // On interdit la copie pour eviter deux structures qui partagent les memes Brick*.
    GameData(GameData const&) = delete;
    GameData& operator=(GameData const&) = delete;
    // On autorise le deplacement pour transferer la propriete des briques.
    GameData(GameData&& other) noexcept;
    GameData& operator=(GameData&& other) noexcept;
    // Libere les briques encore stockees a la destruction.
    ~GameData();

    // Reinitialise la structure et supprime les briques allouees.
    void clear();
};

// Lit un fichier de partie dans une structure temporaire, sans toucher a Game.
bool load_file(std::string const& filename, GameData& data);

#endif
