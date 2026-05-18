#include <filesystem>
#include <iostream>
#include "constants.h"
#include "graphic_gui.h"
#include "gui.h"

using namespace std;


enum Response {
    CANCEL,
    OPEN_FILE,
    SAVE_FILE
};

enum Buttons {
    EXIT,
    OPEN,
    SAVE,
    RESTART,
    START,
    STEP
};

constexpr unsigned drawing_size(500);

// Constructeur
My_window::My_window(string file_name)
    : main_box(Gtk::Orientation::HORIZONTAL), panel_box(Gtk::Orientation::VERTICAL),
      command_box(Gtk::Orientation::VERTICAL), loop_activated(false), loaded(false),
      buttons({Gtk::Button("exit"), Gtk::Button("open"), Gtk::Button("save"),
               Gtk::Button("restart"), Gtk::Button("start"), Gtk::Button("step")}),
      info_frame("Infos :"), info_text({Gtk::Label("score:"), Gtk::Label("lives:"),
                                        Gtk::Label("bricks:"), Gtk::Label("balls:")})
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

    // Charge et vérifie les informations du fichier .txt, s'il est déjà donné
    load_game(file_name);
    
    // Désactive le bouton RESTART car aucun fichier n'a jamais été chargé
    if (file_name.empty() || !std::filesystem::exists(file_name)) { 
        buttons[RESTART].set_sensitive(false); 
    }

    // Vérifie le status et affiche si besoin le mode WON / LOST
    checkStatus();
}

void My_window::load_game(string file_name) {
    if (!file_name.empty() && game.loadFile(file_name)) {
        loaded = true;
        // Mémorise le file_name pour recharger plus tard le fichier (bouton RESTART)
        current_file = file_name;

        // Change l'accessibilité des boutons
        buttons[EXIT].set_sensitive(true);
        buttons[OPEN].set_sensitive(true);
        buttons[SAVE].set_sensitive(true);
        buttons[RESTART].set_sensitive(true);
        buttons[START].set_label("start");
        buttons[START].set_sensitive(true);
        buttons[STEP].set_sensitive(true);
    }
    else {
        loaded = false;
        game.reset();
        current_file = file_name;

        // Change l'accessibilité des boutons
        buttons[EXIT].set_sensitive(true);
        buttons[OPEN].set_sensitive(true);        
        buttons[SAVE].set_sensitive(false);
        buttons[RESTART].set_sensitive(true);
        buttons[START].set_label("start");
        buttons[START].set_sensitive(false);
        buttons[STEP].set_sensitive(false);
    }
    
    update_infos();
    drawing.queue_draw();
}

void My_window::set_commands() {
    for (auto &button : buttons) {
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

void My_window::exit_clicked() {
    hide();
}

void My_window::open_clicked() {
    auto dialog = new Gtk::FileChooserDialog("Choose a text file",
                                             Gtk::FileChooserDialog::Action::OPEN);
    set_dialog(dialog);
}

void My_window::save_clicked() {
    auto dialog = new Gtk::FileChooserDialog("Choose a text file",
                                             Gtk::FileChooserDialog::Action::SAVE);
    set_dialog(dialog);
}

void My_window::restart_clicked() {
    // Réinitialise le jeu à partir du dernier fichier lu
    load_game(current_file); 

    // Vérifie le status et affiche si besoin le mode WON / LOST
    checkStatus();
}

void My_window::start_clicked() {
    if (loop_activated) {
        loop_conn.disconnect();
        loop_activated = false;

        // Change l'accessibilité des boutons (TRUE)
        buttons[EXIT].set_sensitive(true);
        buttons[OPEN].set_sensitive(true);
        buttons[SAVE].set_sensitive(true);
        buttons[RESTART].set_sensitive(true);
        buttons[START].set_label("start");
        buttons[STEP].set_sensitive(true);
    }
    // Lance le jeu seulement si la partie n'est pas terminée
    else if (game.getNbBricks() > 0 && 
            (game.getNbBalls() > 0 || game.getLives() > 0)) {
        loop_conn =
            Glib::signal_timeout().connect(sigc::mem_fun(*this, &My_window::loop), dt);
        loop_activated = true;
        
        // Change l'accessibilité des boutons (FALSE)
        buttons[EXIT].set_sensitive(false);
        buttons[OPEN].set_sensitive(false);
        buttons[SAVE].set_sensitive(false);
        buttons[RESTART].set_sensitive(false);
        buttons[START].set_label("stop");
        buttons[STEP].set_sensitive(false);
    }
}

void My_window::step_clicked() {
    // Actualise une seule fois le jeu (single update)
    loop();
}

void My_window::set_key_controller() {
    auto contr = Gtk::EventControllerKey::create();
    contr->signal_key_pressed().connect(sigc::mem_fun(*this, &My_window::key_pressed),
                                        false);
    add_controller(contr);
}

bool My_window::key_pressed(guint keyval, guint keycode, Gdk::ModifierType state) {
    switch (keyval) {
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

void My_window::set_dialog(Gtk::FileChooserDialog *dialog) {
    dialog->set_modal(true);
    dialog->set_transient_for(*this);
    dialog->set_select_multiple(false);
    dialog->signal_response().connect(
        sigc::bind(sigc::mem_fun(*this, &My_window::dialog_response), dialog));

    dialog->add_button("_Cancel", CANCEL);
    switch (dialog->get_action()) {
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

void My_window::dialog_response(int response, Gtk::FileChooserDialog *dialog) {
    filesystem::path file_name = "";
    if (dialog->get_file()) {
        file_name = dialog->get_file()->get_path();
        if (file_name.extension() != ".txt") {
            file_name = "";
        }
    }
    switch (response) {
    case CANCEL:
        dialog->hide();
        break;
    case OPEN_FILE:
        if (file_name != "") {
            cout << "open file " << file_name << endl;

            // Charge le jeu avec le file_name
            load_game(file_name.string()); 
            // Vérifie le status et affiche si besoin le mode WON / LOST
            checkStatus();

            dialog->hide();
        }
        break;
    case SAVE_FILE:
        if (file_name != "") {
            cout << "save file " << file_name << endl;

            // Sauvegarde le jeu dans un fichier .txt
            game.saveFile(file_name.string());

            dialog->hide();
        }
        break;
    default:
        break;
    }
}

// Met à jour le jeu et l'interface à chaque itération
bool My_window::loop() {
    if (checkStatus()){
        // Met à jour le jeu et gère toutes les collisions si status == ONGOING
        game.update();
        checkStatus();
    }

    update_infos();
    drawing.queue_draw();

    // Retourne true si loop_activated pour maintenir la boucle activée
    if (loop_activated) {
        return true;
    }
    return false;
}

// Vérifie le status de la partie
bool My_window::checkStatus() {
    Status status = game.getStatus();

    if (status == ONGOING) { return true; }
    
    // Affiche le message de fin
    if (status == LOST) { cout << message::lost(); } 
    else if (status == WON) { cout << message::won(); }

    // Arrête le timer du jeu
    loop_activated = false;
    
    // Met à jour les boutons
    setButtonsEnd();

    update_infos();
    drawing.queue_draw();

    return false;
}

void My_window::setButtonsEnd() {
    buttons[EXIT].set_sensitive(true);
    buttons[OPEN].set_sensitive(true);
    buttons[SAVE].set_sensitive(true);
    buttons[RESTART].set_sensitive(true);
    buttons[START].set_sensitive(false);
    buttons[START].set_label("start");
    buttons[STEP].set_sensitive(false);
}

void My_window::set_infos() {
    info_frame.set_child(info_grid);
    info_grid.set_column_homogeneous(true);
    for (size_t i(0); i < info_text.size(); ++i) {
        info_grid.attach(info_text[i], 0, i, 1, 1);
        info_grid.attach(info_value[i], 1, i, 1, 1);
        info_text[i].set_halign(Gtk::Align::START);
        info_value[i].set_halign(Gtk::Align::END);
        info_text[i].set_margin(3);
        info_value[i].set_margin(3);
    }
}

// Actualise les différents compteurs
void My_window::update_infos() {
    // Récupère les informations
    string s_score  = to_string(game.getScore());
    string s_lives  = to_string(game.getLives());
    string s_bricks = to_string(game.getNbBricks());
    string s_balls  = to_string(game.getNbBalls());

    // Met à jour les informations sur l'interface
    info_value[0].set_text(s_score);
    info_value[1].set_text(s_lives);
    info_value[2].set_text(s_bricks);
    info_value[3].set_text(s_balls);
}

void My_window::set_drawing() {
    drawing.set_content_width(drawing_size);
    drawing.set_content_height(drawing_size);
    drawing.set_expand();
    drawing.set_draw_func(sigc::mem_fun(*this, &My_window::on_draw));
}

// Appelé par drawing.queue_draw(), mais jamais directement !
void My_window::on_draw(
        const Cairo::RefPtr<Cairo::Context> &cr, int width, int height) {
    // Dessine le canevas blanc
    cr->set_source_rgb(1.0, 1.0, 1.0); 
    cr->paint();

    // Inverse l'axe Y du crayon
    graphic_set_context(cr);
    double side(min(width, height));
    cr->translate((width - side) / 2, (height + side) / 2);
    cr->scale(side / (arena_size), -side / (arena_size));

    // Dessine le jeu si le jeu a été chargé avec un fichier
    if (loaded) { game.draw(); }
}

void My_window::set_mouse_controller() {
    auto left_click = Gtk::GestureClick::create();
    auto move = Gtk::EventControllerMotion::create();

    left_click->set_button(GDK_BUTTON_PRIMARY);

    left_click->signal_pressed().connect(
        sigc::mem_fun(*this, &My_window::on_drawing_left_click));
    move->signal_motion().connect(sigc::mem_fun(*this, &My_window::on_drawing_move));

    drawing.add_controller(left_click);
    drawing.add_controller(move);
}

void My_window::on_drawing_left_click(int n_press, double x, double y) {
    // Crée une nouvelle balle sur la raquette avec un clic gauche
    if (game.getNbBalls() == 0 && game.getLives() > 0) { 
        game.generateBall(); 
        update_infos(); 
        drawing.queue_draw();
    }  
}

// Donne la position de la souris à game
void My_window::on_drawing_move(double x, double y) {
    // Convertit les coordonnées (reproduit l'inverse de ce qu'il y a dans on_draw)
    double width = drawing.get_width();
    double height = drawing.get_height();
    double side = min(width, height);
    
    // Centre par rapport à la zone de dessin (gérer le translate)
    double x_relatif = x - (width - side) / 2;
    
    // Passe les pixels aux unités de l'arène (gérer le scale)
    double x_jeu = x_relatif * (arena_size / side);

    game.setMouseX(x_jeu);
}
