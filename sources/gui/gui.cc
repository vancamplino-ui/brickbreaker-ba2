#include <algorithm>
#include <filesystem>
#include <iostream>

#include "constants.h"
#include "graphic_gui.h"
#include "gui.h"

using namespace std;

enum Response
{
    CANCEL,
    OPEN_FILE,
    SAVE_FILE
};
enum Buttons
{
    EXIT,
    OPEN,
    SAVE,
    RESTART,
    START,
    STEP
};

constexpr unsigned drawing_size(500);

My_window::My_window(string file_name)
    : main_box(Gtk::Orientation::HORIZONTAL), panel_box(Gtk::Orientation::VERTICAL),
      command_box(Gtk::Orientation::VERTICAL), loop_activated(false),
      buttons({Gtk::Button("exit"), Gtk::Button("open"), Gtk::Button("save"),
               Gtk::Button("restart"), Gtk::Button("start"), Gtk::Button("step")}),
      info_frame("Infos :"), info_text({Gtk::Label("score:"), Gtk::Label("lives:"),
                                        Gtk::Label("bricks:"), Gtk::Label("balls:")}),
      current_file_name(file_name)
{
    set_title("Brick Breaker");
    set_child(main_box);
    main_box.append(panel_box);
    main_box.append(drawing);
    panel_box.append(command_box);
    panel_box.append(info_frame);

    set_commands();
    set_key_controller();
    set_mouse_controller();
    set_infos();
    set_drawing();

    if (!current_file_name.empty()) {
        game.reset();
        if (!game.load(current_file_name)) {
            game.reset();
        }
    }

    update_infos();
    drawing.queue_draw();
}
void My_window::set_commands()
{
    for (auto &button : buttons)
    {
        command_box.append(button);
        command_box.set_size_request(120, -1);
        button.set_margin(1);
    }

    buttons[EXIT].signal_clicked().connect(
        sigc::mem_fun(*this, &My_window::exit_clicked));
    buttons[OPEN].signal_clicked().connect(
        sigc::mem_fun(*this, &My_window::open_clicked));
    buttons[SAVE].signal_clicked().connect(
        sigc::mem_fun(*this, &My_window::save_clicked));
    buttons[RESTART].signal_clicked().connect(
        sigc::mem_fun(*this, &My_window::restart_clicked));
    buttons[START].signal_clicked().connect(
        sigc::mem_fun(*this, &My_window::start_clicked));
    buttons[STEP].signal_clicked().connect(
        sigc::mem_fun(*this, &My_window::step_clicked));
}

void My_window::exit_clicked()
{
    hide();
}
void My_window::open_clicked()
{
    auto dialog = new Gtk::FileChooserDialog("Choose a text file",
                                             Gtk::FileChooserDialog::Action::OPEN);
    set_dialog(dialog);
}
void My_window::save_clicked()
{
    auto dialog = new Gtk::FileChooserDialog("Choose a text file",
                                             Gtk::FileChooserDialog::Action::SAVE);
    set_dialog(dialog);
}
void My_window::restart_clicked()
{
    cout << __func__ << endl; 
      if (current_file_name.empty()) return;

    game.reset();
    if (!game.load(current_file_name)) {
        game.reset();
    }

    update_infos();
    drawing.queue_draw();
}
void My_window::start_clicked()
{
    cout << __func__ << endl;
    if (loop_activated)
    {
        loop_conn.disconnect();
        loop_activated = false;
        buttons[EXIT].set_sensitive(true);
        buttons[OPEN].set_sensitive(true);
        buttons[SAVE].set_sensitive(true);
        buttons[RESTART].set_sensitive(true);
        buttons[START].set_label("start");
        buttons[STEP].set_sensitive(true);
    }
    else if (!game.is_finished())
    {
        loop_conn =
            Glib::signal_timeout().connect(sigc::mem_fun(*this, &My_window::loop), dt);
        loop_activated = true;
        buttons[EXIT].set_sensitive(false);
        buttons[OPEN].set_sensitive(false);
        buttons[SAVE].set_sensitive(false);
        buttons[RESTART].set_sensitive(false);
        buttons[START].set_label("stop");
        buttons[STEP].set_sensitive(false);
    }
    else
    {
        loop_activated = false;
        buttons[START].set_label("start");

    }

}
void My_window::step_clicked()
{
    cout << __func__ << endl; 
    game.update();
    update_infos();
    drawing.queue_draw();
}
void My_window::set_key_controller()
{
    auto contr = Gtk::EventControllerKey::create();
    contr->signal_key_pressed().connect(sigc::mem_fun(*this, &My_window::key_pressed),
                                        false);
    add_controller(contr);
}
bool My_window::key_pressed(guint keyval, guint keycode, Gdk::ModifierType state)
{
    switch (keyval)
    {
    case '1':
        step_clicked();
        return true;
    case 's':
        start_clicked();
        return true;
    case 'r':
        restart_clicked();
        return true;
    default:
        break;
    }
    return false;
}

void My_window::set_dialog(Gtk::FileChooserDialog *dialog)
{
    dialog->set_modal(true); // block the main window until the dialog is closed
    dialog->set_transient_for(*this);
    dialog->set_select_multiple(false);//interdit la séléction multiple de fichiers 
    dialog->signal_response().connect(
        sigc::bind(sigc::mem_fun(*this, &My_window::dialog_response), dialog));

    dialog->add_button("_Cancel", CANCEL);
    switch (dialog->get_action())
    {
    case Gtk::FileChooserDialog::Action::OPEN:
        dialog->add_button("_Open", OPEN_FILE);
        break;
    case Gtk::FileChooserDialog::Action::SAVE:
        dialog->add_button("_Save", SAVE_FILE);
        break;
    default:
        break;
    }

    auto filter_text = Gtk::FileFilter::create();
    filter_text->set_name("Text files");
    filter_text->add_pattern("*.txt");
    dialog->add_filter(filter_text);

    auto filter_any = Gtk::FileFilter::create();
    filter_any->set_name("Any files");
    filter_any->add_pattern("*");
    dialog->add_filter(filter_any);

    dialog->show();
}
void My_window::handle_open_file(std::filesystem::path const& file_name,
                                 Gtk::FileChooserDialog *dialog)
{
    if (file_name == "") return;

    cout << "open file " << file_name << endl;

    game.reset();
    if (game.load(file_name.string())) {
        current_file_name = file_name.string();
    } else {
        game.reset();
    }

    update_infos();
    drawing.queue_draw();
    dialog->hide();
}

void My_window::handle_save_file(std::filesystem::path const& file_name,
                                 Gtk::FileChooserDialog *dialog)
{
    if (file_name == "") return;

    cout << "save file " << file_name << endl;
    game.save(file_name.string());
    dialog->hide();
}
void My_window::dialog_response(int response, Gtk::FileChooserDialog *dialog)
{
    filesystem::path file_name = "";
    if (dialog->get_file()) {
        file_name = dialog->get_file()->get_path();
        if (file_name.extension() != ".txt") {
            file_name = "";
        }
    }

    switch (response)
    {
    case CANCEL:
        dialog->hide();
        break;

    case OPEN_FILE:
        handle_open_file(file_name, dialog);
        break;

    case SAVE_FILE:
        handle_save_file(file_name, dialog);
        break;

    default:
        break;
    }
}

void My_window::stop_loop()
{
    loop_activated = false;
    buttons[EXIT].set_sensitive(true);
    buttons[OPEN].set_sensitive(true);
    buttons[SAVE].set_sensitive(true);
    buttons[RESTART].set_sensitive(true);
    buttons[START].set_label("start");
    buttons[STEP].set_sensitive(true);
}

bool My_window::loop()
{
    if (!loop_activated) return false;

    if (game.is_finished()) {
        stop_loop();
        return false;
    }

    game.update();
    update_infos();
    drawing.queue_draw();

    if (game.is_finished()) {
        stop_loop();
        return false;
    }

    return true;
}

void My_window::set_infos()
{
    info_frame.set_child(info_grid);
    info_grid.set_column_homogeneous(true);
    for (size_t i(0); i < info_text.size(); ++i)
    {
        info_grid.attach(info_text[i], 0, i, 1, 1);
        info_grid.attach(info_value[i], 1, i, 1, 1);
        info_text[i].set_halign(Gtk::Align::START);
        info_value[i].set_halign(Gtk::Align::END);
        info_text[i].set_margin(3);
        info_value[i].set_margin(3);
    }
}

void My_window::update_infos()
{
    info_value[0].set_text(to_string(game.get_score()));
    info_value[1].set_text(to_string(game.get_lives()));
    info_value[2].set_text(to_string(game.get_bricks().size()));
    info_value[3].set_text(to_string(game.get_balls().size()));
}

void My_window::set_drawing()
{
    drawing.set_content_width(drawing_size);
    drawing.set_content_height(drawing_size);
    drawing.set_expand();
    drawing.set_draw_func(sigc::mem_fun(*this, &My_window::on_draw));
}
void My_window::on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height)
{
    graphic_set_context(cr);
    double side(std::min(width, height));
    cr->translate((width - side) / 2, (height + side) / 2);
    cr->scale(side / (arena_size), -side / (arena_size));
    // TODO: draw the game
}

void My_window::set_mouse_controller()
{
    auto left_click = Gtk::GestureClick::create();
    auto move = Gtk::EventControllerMotion::create();

    left_click->set_button(GDK_BUTTON_PRIMARY);

    left_click->signal_pressed().connect(
        sigc::mem_fun(*this, &My_window::on_drawing_left_click));
    move->signal_motion().connect(sigc::mem_fun(*this, &My_window::on_drawing_move));

    drawing.add_controller(left_click);
    drawing.add_controller(move);
}
void My_window::on_drawing_left_click(int n_press, double x, double y)
{
    cout << __func__ << endl; // TODO
}
void My_window::on_drawing_move(double x, double y)
{
    cout << __func__ << endl; // TODO
}
