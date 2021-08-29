all: bin/main

clean:
	rm -r bin

.PHONY: all clean

bin/main: bin/main.o bin/quadratic-equation.o | bin
	gcc -o $@ $^ -lm

bin/%.o: src/%.c | bin bin/deps
	gcc -c -Wall -pedantic -Wextra -Wshadow -Werror -O2 -std=c99 -o $@ -Iinclude -MD -MF bin/deps/$(notdir $@).mk $<

bin:
	mkdir bin

bin/deps: | bin
	mkdir bin/deps

include $(wildcard bin/deps/*)
