all: bin/main

clean:
	rm -r bin

.PHONY: all clean

bin/main: bin/main.o bin/quadratic-equation.o | bin
	g++ -o $@ $^ -lm

bin/%.o: src/%.cpp | bin bin/deps
	g++ -c -Wall -pedantic -Wextra -Wshadow -Werror -O2 -std=c++11 -o $@ -Iinclude -MD -MF bin/deps/$(notdir $@).mk $<

bin:
	mkdir bin

bin/deps: | bin
	mkdir bin/deps

include $(wildcard bin/deps/*)
