// main_window.h : fenetre GTKmm principale
//
// Auteurs  : Liam Van Camp, Victor Henri Willy Eder
// Version  : 1.0 du 08.04.2026
//

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtkmm/box.h>
#include <gtkmm/window.h>

#include "../graphic/graphic_area.h"

class MainWindow : public Gtk::Window
{
public:
    MainWindow();
    ~MainWindow() override = default;

private:
    Gtk::Box main_box;
    GraphicArea graphic_area;
};

#endif
