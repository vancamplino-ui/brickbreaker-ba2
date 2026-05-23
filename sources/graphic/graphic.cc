// graphic.cc : dessin des entités du jeu via Cairo
//
// Auteurs : Liam Van Camp, Victor Henri Willy Eder
// Version : 1.0 du 24.05.2026

#include <cmath>

#include "graphic.h"
#include "graphic_gui.h"
#include "../tools/constants.h"
#include "../tools/tools.h"

static const Cairo::RefPtr<Cairo::Context> *ptcr(nullptr);

namespace
{
    constexpr double TWO_PI = 6.28318530717958647692;
    constexpr double PI = 3.14159265358979323846;
    Color rainbow_color_from_hit_points(int hit_points);
    void fill_square(Square const& square);
    void fill_circle(Circle const& circle);
    double split_child_half_size(double parent_half_size);
}

void graphic_set_context(const Cairo::RefPtr<Cairo::Context> &cr)
{
    ptcr = &cr;
}

void set_color(Color color)
{
    double r(0.), g(0.), b(0.);

    switch (color)
    {
    case RED:
        r = 1.0;
        break;
    case ORANGE:
        r = 1.0;
        g = 0.5;
        break;
    case YELLOW:
        r = 1.0;
        g = 1.0;
        break;
    case GREEN:
        g = 1.0;
        break;
    case CYAN:
        g = 1.0;
        b = 1.0;
        break;
    case BLUE:
        b = 1.0;
        break;
    case PURPLE:
        r = 0.5;
        b = 1.0;
        break;
    case BLACK:
        r = g = b = 0.0;
        break;
    case GREY:
        r = g = b = 0.5;
        break;
    case WHITE:
        r = g = b = 1.0;
        break;
    default:
        break;
    }
    (*ptcr)->set_source_rgb(r, g, b);
}

void draw_background()
{
    set_color(WHITE);
    (*ptcr)->rectangle(0.0, 0.0, arena_size, arena_size);
    (*ptcr)->fill();
}

void draw_arena_border()
{
    set_color(GREY);
    (*ptcr)->set_line_width(1.0);
    (*ptcr)->rectangle(-0.5, -0.5, arena_size + 1, arena_size + 1);
    (*ptcr)->stroke();
}

void draw_rainbow_brick(Square const& square, int hit_points)
{
    set_color(rainbow_color_from_hit_points(hit_points));
    fill_square(square);
}

void draw_ball_brick(Square const& square)
{
    set_color(RED);
    fill_square(square);

    set_color(BLACK);
    fill_circle({square.center, new_ball_radius});
}

void draw_split_brick(Square const& square, unsigned level)
{
    set_color(rainbow_color_from_hit_points(static_cast<int>(level) + 1));
    fill_square(square);

    const double child_half_size(split_child_half_size(square.half_size));
    const double child_side(2.0 * child_half_size);

    if (child_side < brick_size_min) return;

    const double child_offset(square.half_size - child_half_size);
    const unsigned child_level(level + 1);

    draw_split_brick({{square.center.x - child_offset, square.center.y + child_offset},
                      child_half_size},
                     child_level);
    draw_split_brick({{square.center.x + child_offset, square.center.y + child_offset},
                      child_half_size},
                     child_level);
    draw_split_brick({{square.center.x - child_offset, square.center.y - child_offset},
                      child_half_size},
                     child_level);
    draw_split_brick({{square.center.x + child_offset, square.center.y - child_offset},
                      child_half_size},
                     child_level);
}

void draw_ball(Circle const& circle)
{
    set_color(BLACK);
    fill_circle(circle);
}

void draw_paddle(Circle const& arc)
{
    if (arc.radius <= 0.0) return;

    //0 = y + r sin(t)
    //sin(t) = -y / r
    const double ratio(-arc.center.y / arc.radius);
    const double start_angle(std::asin(ratio));
    const double end_angle(PI - start_angle);

    set_color(BLACK);
    (*ptcr)->set_line_width(1.0);
    (*ptcr)->arc(arc.center.x,
                 arc.center.y,
                 arc.radius,
                 start_angle,
                 end_angle);
    (*ptcr)->stroke();
}

namespace
{
    Color rainbow_color_from_hit_points(int hit_points)
    {
        static const Color colors[] = {RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, PURPLE};

        if (hit_points <= 1) return colors[0];
        if (hit_points >= 7) return colors[6];
        return colors[hit_points - 1];
    }

    void fill_square(Square const& square)
    {
        const double side(2.0 * square.half_size);
        (*ptcr)->rectangle(square.center.x - square.half_size,
                           square.center.y - square.half_size,
                           side,
                           side);
        (*ptcr)->fill();
    }

    void fill_circle(Circle const& circle)
    {
        (*ptcr)->arc(circle.center.x,
                     circle.center.y,
                     circle.radius,
                     0.0,
                     TWO_PI);
        (*ptcr)->fill();
    }

    double split_child_half_size(double parent_half_size)
    {
        return (2.0 * parent_half_size - split_brick_gap) / 4.0;
    }
}