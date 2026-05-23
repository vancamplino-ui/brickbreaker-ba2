// graphic.h : déclaration des fonctions de dessin Cairo
//
// Auteurs : Liam Van Camp, Victor Henri Willy Eder
// Version : 1.0 du 24.05.2026

#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "../tools/tools.h"

enum Color
{
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    CYAN,
    BLUE,
    PURPLE,
    BLACK,
    GREY,
    WHITE,
};

void set_color(Color color);
void draw_background();
void draw_arena_border();
void draw_rainbow_brick(Square const& square, int hit_points);
void draw_ball_brick(Square const& square);
void draw_split_brick(Square const& square, unsigned level = 0);
void draw_ball(Circle const& circle);
void draw_paddle(Circle const& arc);

#endif