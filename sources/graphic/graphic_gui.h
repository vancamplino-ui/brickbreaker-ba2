// graphic_gui.h : interface pour initialiser le contexte Cairo
//
// Auteurs : Liam Van Camp, Victor Henri Willy Eder
// Version : 1.0 du 26.04.2026

#ifndef GRAPHIC_GUI_H
#define GRAPHIC_GUI_H

#include <cairomm/context.h>

void graphic_set_context(const Cairo::RefPtr<Cairo::Context> &cr);

#endif
