# todo: explain why c++03. Basically, we started learning C++ from 2003 standard, that's why.
# it is my solution for a task in the programming course.
CXX = g++
CXXFLAGS = -Wall -pedantic -Wextra -Wshadow -Werror -std=c++03

CXXFLAGS_DBG_SAN = -fsanitize=address,undefined

HEADERS = $(wildcard *.h)

all: present

present: view.dbg.san
	./view.dbg.san

debug_bins: view.dbg

%.obj: %.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) -O2 -DNDEBUG -o $@ $<

%.dbg.obj: %.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) -Og -g -o $@ $<

%.dbg.san.obj: %.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) -O0 -g $(CXXFLAGS_SAN) -o $@ $<

view: view.obj
	$(CXX) -o $@ $(filter %.obj %.res,$^) -lSDL2 -lSDL2_ttf

view.dbg: view.dbg.obj
	$(CXX) -o $@ $(filter %.obj %.res,$^) -lSDL2 -lSDL2_ttf

view.dbg.san: view.dbg.san.obj
	$(CXX) $(CXXFLAGS_DBG_SAN) -o $@ $(filter %.obj %.res,$^) -lSDL2 -lSDL2_ttf

clean:
	rm -f *.obj view view.dbg

.PHONY: present debug_bins clean
