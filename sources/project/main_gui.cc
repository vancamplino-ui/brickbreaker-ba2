// main_gui.cc : point d'entree de l'interface graphique GTKmm
//
// Auteurs  : Liam Van Camp, Victor Henri Willy Eder
// Version  : 1.0 du 08.04.2026
//

#include <gtkmm/application.h>

#include "../gui/main_window.h"

int main(int argc, char* argv[])
{
    auto app(Gtk::Application::create("ch.epfl.icc2.brickbreaker"));
    return app->make_window_and_run<MainWindow>(argc, argv);
}
