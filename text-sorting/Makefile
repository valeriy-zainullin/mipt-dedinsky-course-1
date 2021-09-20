CC=gcc
CXX=g++
CFLAGS=-Wall -pedantic -Wextra -Wshadow -Werror -std=c99 -Iinclude
CFLAGS_RELEASE=-O2
CFLAGS_DEBUG=-g -O0

all: bin/main bin/main_asan bin/main_ubsan bin/main_lsan

clean:
	rm -r bin

.PHONY: all clean

bin/main: bin/main.c.o bin/text.c.o | bin
	gcc -o $@ $^
bin/main_debug: bin/main.debug.c.o bin/text.debug.c.o | bin
	gcc -o $@ $^
bin/main_asan: bin/main.asan.c.o bin/text.asan.c.o | bin
	gcc -o $@ $^
bin/main_ubsan: bin/main.ubsan.c.o bin/text.ubsan.c.o | bin
	gcc -o $@ $^ -lm
bin/main_lsan: bin/main.lsan.c.o bin/text.lsan.c.o | bin
	gcc -o $@ $^

bin/%.c.o: src/%.c | bin bin/deps
	gcc -c $(CFLAGS) $(CLAGS_RELEASE) -o $@ -MD -MF bin/deps/$(notdir $@).mk $<
bin/%.debug.c.o: src/%.c | bin bin/deps
	gcc -c $(CFLAGS) $(CFLAGS_DEBUG) -o $@ -MD -MF bin/deps/$(notdir $@).mk $<
bin/%.asan.c.o: src/%.c | bin bin/deps
	gcc -c $(CFLAGS) $(CFLAGS_DEBUG) -fsanitize=address -o $@ -MD -MF bin/deps/$(notdir $@).mk $<
bin/%.ubsan.c.o: src/%.c | bin bin/deps
	gcc -c $(CFLAGS) $(CFLAGS_DEBUG) -fsanitize=undefined -o $@ -MD -MF bin/deps/$(notdir $@).mk $<
bin/%.lsan.c.o: src/%.c | bin bin/deps
	gcc -c $(CFLAGS) $(CFLAGS_DEBUG) -fsanitize=leak -o $@ -MD -MF bin/deps/$(notdir $@).mk $<

bin:
	mkdir bin

bin/deps: | bin
	mkdir bin/deps

include $(wildcard bin/deps/*)
