# Proposition de refactorisation pour `dialog_response`

Le but est de sortir la logique du `switch` dans deux sous-fonctions de `My_window`,
pour garder `dialog_response(...)` courte et lisible.

## Idees de declarations a ajouter dans `gui.h`

```cpp
void handle_open_file(std::filesystem::path const& file_name,
                      Gtk::FileChooserDialog* dialog);
void handle_save_file(std::filesystem::path const& file_name,
                      Gtk::FileChooserDialog* dialog);
```

Si tu prends cette version, il faudra aussi ajouter dans `gui.h` :

```cpp
#include <filesystem>
```

## Proposition pour `dialog_response(...)`

```cpp
void My_window::dialog_response(int response, Gtk::FileChooserDialog *dialog)
{
    std::filesystem::path file_name = "";

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
```

## Proposition pour `handle_open_file(...)`

```cpp
void My_window::handle_open_file(std::filesystem::path const& file_name,
                                 Gtk::FileChooserDialog* dialog)
{
    if (file_name == "") return;

    std::cout << "open file " << file_name << std::endl;

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
```

## Proposition pour `handle_save_file(...)`

```cpp
void My_window::handle_save_file(std::filesystem::path const& file_name,
                                 Gtk::FileChooserDialog* dialog)
{
    if (file_name == "") return;

    std::cout << "save file " << file_name << std::endl;
    game.save(file_name.string());
    dialog->hide();
}
```

## Pourquoi cette version est plus propre

- `dialog_response(...)` ne fait plus que :
  - recuperer le chemin
  - dispatcher selon le `switch`
- la logique metier de `Open` est isolee
- la logique metier de `Save` est isolee
- `current_file_name` n'est mis a jour qu'en cas de succes de `load(...)`

## Variante encore plus compacte

Si tu veux aller encore plus loin, on pourrait aussi extraire la partie :

```cpp
if (dialog->get_file()) {
    ...
}
```

dans une fonction du style :

```cpp
std::filesystem::path selected_text_file(Gtk::FileChooserDialog* dialog);
```

Mais je pense que la version ci-dessus est deja un bon compromis.
