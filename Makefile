# ============================================================
# Definitions de macros
# ============================================================

CXX      = g++
CXXFLAGS = -g -Wall -Wextra -pedantic -std=c++17

CXXFILES = \
	sources/project/main.cc \
	sources/model/game.cc \
	sources/model/ball.cc \
	sources/model/brick.cc \
	sources/model/paddle.cc \
	sources/model/message.cc \
	sources/tools/tools.cc

OFILES = \
	sources/project/main.o \
	sources/model/game.o \
	sources/model/ball.o \
	sources/model/brick.o \
	sources/model/paddle.o \
	sources/model/message.o \
	sources/tools/tools.o

# ============================================================
# Definition de la premiere regle
# ============================================================

project: $(OFILES)
	$(CXX) $(CXXFLAGS) $(OFILES) -o project

# ============================================================
# Definitions de cibles particulieres
# ============================================================

depend:
	@echo " *** MISE A JOUR DES DEPENDANCES ***"
	@(sed '/^# DO NOT DELETE THIS LINE/q' Makefile && \
	  $(CXX) -MM $(CXXFLAGS) $(CXXFILES) | \
	  egrep -v "/usr/include" \
	 ) >Makefile.new
	@mv Makefile.new Makefile

clean:
	@echo " *** EFFACE MODULES OBJET ET EXECUTABLE ***"
	@/bin/rm -f sources/*/*.o project
	@/bin/rm -f *~ sources/*/*~ sources/*/*~

#
# -- Regles de dependances generees automatiquement
#
# DO NOT DELETE THIS LINE