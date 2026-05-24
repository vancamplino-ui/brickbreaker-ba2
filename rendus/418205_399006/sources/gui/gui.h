// gui.h : déclaration de la classe My_window
//
// Auteurs : Liam Van Camp, Victor Henri Willy Eder
// Version : 1.0 du 26.04.2026

#ifndef GUI_H
#define GUI_H

#include <array>
#include <filesystem>
#include <gtkmm.h>
#include <string>

#include "../model/game.h"

class My_window : public Gtk::Window
{
public:
    My_window(std::string file_name);

private:
    Gtk::Box main_box, panel_box, command_box;
    Gtk::Grid info_grid;
    bool loop_activated;
    bool arena_visible;
    sigc::connection loop_conn;
    std::array<Gtk::Button, 6> buttons;
    Gtk::Frame info_frame;
    std::array<Gtk::Label, 4> info_text, info_value;
    Gtk::DrawingArea drawing;
    Game game;
    std::string current_file_name;

    void set_commands();
    void exit_clicked();
    void open_clicked();
    void save_clicked();
    void restart_clicked();
    void start_clicked();
    void step_clicked();
    void set_key_controller();

    bool key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);
    void set_dialog(Gtk::FileChooserDialog *dialog);

    void dialog_response(int response, Gtk::FileChooserDialog *dialog);
    void handle_open_file(std::filesystem::path const& file_name,
                          Gtk::FileChooserDialog *dialog);
    void handle_save_file(std::filesystem::path const& file_name,
                          Gtk::FileChooserDialog *dialog);

    void stop_loop();
    bool loop();

    void set_infos();
    void update_infos();
    void set_drawing();
    void on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height);

    void set_mouse_controller();
    void on_drawing_left_click(int n_press, double x, double y);
    void on_drawing_move(double x, double y);
};

#endif
