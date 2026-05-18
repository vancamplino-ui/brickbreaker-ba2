#ifndef GRAPHIC_GUI_H
#define GRAPHIC_GUI_H

#include <gtkmm/drawingarea.h>
#include "graphic.h"


// Interface entre l'interface GTKmm et le moteur de dessin (contexte Cairo)
void graphic_set_context(const Cairo::RefPtr<Cairo::Context> &cr);

#endif
