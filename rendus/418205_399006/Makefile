OUT      := project
CXX      := g++
CXXFLAGS := -Wall -std=c++17
PKGS     := gtkmm-4.0
LINKING  := $(shell pkg-config --cflags $(PKGS))
LDLIBS   := $(shell pkg-config --libs $(PKGS))
INCLUDES := -Isources/gui -Isources/graphic -Isources/tools -Isources/model

BUILD_DIR := build

CXXFILES := \
	sources/graphic/graphic.cc \
	sources/project/project.cc \
	sources/gui/gui.cc \
	sources/model/game.cc \
	sources/model/ball.cc \
	sources/model/brick.cc \
	sources/model/paddle.cc \
	sources/model/message.cc \
	sources/tools/tools.cc
	
OFILES   := $(addprefix $(BUILD_DIR)/, $(CXXFILES:.cc=.o))

.PHONY: all clean tests

all: $(OUT)

$(BUILD_DIR)/%.o: %.cc
	@mkdir -p "$$(dirname $@)"
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $(LINKING) -c $< -o $@

$(OUT): $(OFILES)
	@$(CXX) $(CXXFLAGS) $(LINKING) $^ -o $@ $(LDLIBS)

clean:
	@echo "Cleaning project..."
	@rm -rf $(BUILD_DIR) $(OUT)

tests: $(OUT)
	@for test in $$(ls tests); do \
		echo "Running $$test..."; \
		./$(OUT) tests/$$test; \
	done
