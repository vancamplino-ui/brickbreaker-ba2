// main_window.cc : fenetre GTKmm principale
//
// Auteurs  : Liam Van Camp, Victor Henri Willy Eder
// Version  : 1.0 du 08.04.2026
//

#include "main_window.h"

MainWindow::MainWindow()
    : main_box(Gtk::Orientation::VERTICAL)
{
    set_title("Brick Breaker");
    set_default_size(480, 320);

    set_child(main_box);
    main_box.append(graphic_area);
}
