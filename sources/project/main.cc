// main.cc  : point d'entree du programme
//
// Auteurs  : Liam Van Camp, Victor Henri Willy Eder
// Version  : 1.0 du 27.03.2026
//

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
