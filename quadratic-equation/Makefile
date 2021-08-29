all: bin/main

clean:
	rm -r bin

.PHONY: all clean

bin/main: bin/main.o bin/quadratic-equation.o | bin
	gcc -o $@ $^ -lm

bin/%.o: src/%.c | bin
	gcc -c -Wall -pedantic -Wextra -Wshadow -Werror -O2 -std=c99 -o $@ -Iinclude $<

bin:
	mkdir bin
