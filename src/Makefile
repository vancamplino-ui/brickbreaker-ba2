# --- Configuration du projet ---
OUT      := project
CXX      := g++
CXXFLAGS := -Wall -std=c++17 -g -MMD -MP  # -MMD et -MP génèrent les dépendances (.h)
PKGS     := gtkmm-4.0

# Récupération automatique des flags GTK
LINKING  := $(shell pkg-config --cflags $(PKGS))
LDLIBS   := $(shell pkg-config --libs $(PKGS))

# Dossier pour les fichiers compilés
BUILD_DIR := build

# --- Liste des fichiers sources ---
# Ajoute ici tous tes fichiers .cc
CXXFILES := project.cc game.cc graphic.cc gui.cc \
			tools.cc message.cc ball.cc brick.cc paddle.cc

# Génération automatique des noms de fichiers objets (.o) et dépendances (.d)
OFILES   := $(addprefix $(BUILD_DIR)/, $(CXXFILES:.cc=.o))
DEPS     := $(OFILES:.o=.d)

# --- Règles de construction ---

.PHONY: all clean tests

all: $(OUT)

# Règle pour l'exécutable final
$(OUT): $(OFILES)
	@echo "Linking $@..."
	@$(CXX) $(CXXFLAGS) $(LINKING) $^ -o $@ $(LDLIBS)

# Règle générique pour les fichiers objets
$(BUILD_DIR)/%.o: %.cc
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@

# Inclusion des fichiers de dépendances (.d) générés par le compilateur
-include $(DEPS)

# --- Utilitaires ---

clean:
	@echo "Cleaning project..."
	@rm -rf $(BUILD_DIR) $(OUT)

tests: $(OUT)
	@if [ -d tests ]; then \
		for test in $$(ls tests); do \
			echo "Running$$test..."; \
			./$(OUT) tests/$$test; \
		done; \
	else \
		echo "No tests directory found."; \
	fi
