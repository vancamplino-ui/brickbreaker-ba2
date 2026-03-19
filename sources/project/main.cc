#include <iostream>

#include "../model/game.h"

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Usage: ./project file.txt\n";
        return 1;
    }

    Game game;

    if (!game.load(argv[1])) {
        // les messages d'erreur sont déjà affichés dans les fonctions read_*
        return 1;
    }
    
    return 0;
}