#include "graphic.h"
#include "graphic_gui.h"
#include <cmath>

using namespace std;

// Portabilité du code 
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const Cairo::RefPtr<Cairo::Context> *ptcr(nullptr);

// Graphic_gui.h
void graphic_set_context(const Cairo::RefPtr<Cairo::Context> &cr) {
    ptcr = &cr;
}

// Local functions
void set_color(Color color) {
    double r(0.), g(0.), b(0.);

    switch (color) {
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


// =============================================================================
// FONCTIONS DE DESSIN
// =============================================================================

void graphic_draw_rectangle(double x_center, double y_center, double size,
                            Color color) {
    set_color(color);
    (*ptcr)->rectangle(x_center - size / 2.0, y_center - size / 2.0, size, size);
    (*ptcr)->fill();
}

void graphic_draw_circle(double x_center, double y_center, double radius,
                         Color color) {
    set_color(color);
    (*ptcr)->arc(x_center, y_center, radius, 0, 2 * M_PI);
    (*ptcr)->fill();
}

void graphic_draw_arc(double x_center, double y_center, double radius,
                      Color color) {
    // Calcul de l'angle de dessin de la raquette
    double a1 = asin(abs(y_center)/radius);
    double a2 = M_PI - a1;

    set_color(color);
    (*ptcr)->set_line_width(0.5);
    (*ptcr)->arc(x_center, y_center, radius, a1, a2); 
    (*ptcr)->stroke();
}

void graphic_draw_border(double x_center, double y_center, double size,
                      Color borderColor){
    set_color(borderColor);
    (*ptcr)->set_line_width(0.5);
    (*ptcr)->rectangle(x_center - size / 2.0, y_center - size / 2.0, size, size);
    (*ptcr)->stroke();
}
