// project.cc : point d'entree du programme
//
// Auteurs  : Liam Van Camp, Victor Henri Willy Eder
// Version  : 1.0 du 26.04.2026

#include <gtkmm/application.h>
#include <string>

#include "gui.h"

int main(int argc, char *argv[])
{
    std::string file_name("");
    if (argc > 1)
    {
        file_name = argv[1];
    }
    auto app = Gtk::Application::create();
    return app->make_window_and_run<My_window>(1, argv, file_name);
}
