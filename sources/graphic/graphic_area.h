// graphic_area.h : zone graphique GTKmm
//
// Auteurs  : Liam Van Camp, Victor Henri Willy Eder
// Version  : 1.0 du 08.04.2026
//

#ifndef GRAPHIC_AREA_H
#define GRAPHIC_AREA_H

#include <gtkmm/drawingarea.h>

class GraphicArea : public Gtk::DrawingArea
{
public:
    GraphicArea();
    ~GraphicArea() override = default;
};

#endif
