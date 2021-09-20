CC=gcc
CXX=g++
CFLAGS=-Wall -pedantic -Wextra -Wshadow -Werror -std=c99 -Iinclude
CFLAGS_RELEASE=-O2
CFLAGS_DEBUG=-g -O0

all: bin/main bin/main_asan bin/main_ubsan bin/main_lsan

clean:
	rm -r bin

.PHONY: all clean

bin/main: bin/main.c.o bin/text.c.o bin/string_utils.c.o | bin
	gcc -o $@ $^
bin/main_debug: bin/main.c.debug.o bin/text.c.debug.o bin/string_utils.c.debug.o | bin
	gcc -o $@ $^
bin/main_asan: bin/main.c.asan.o bin/text.c.asan.o bin/string_utils.c.asan.o | bin
	gcc -o $@ $^ -lasan
bin/main_ubsan: bin/main.c.ubsan.o bin/text.c.ubsan.o bin/string_utils.c.ubsan.o | bin
	gcc -o $@ $^ -lubsan
bin/main_lsan: bin/main.c.lsan.o bin/text.c.lsan.o bin/string_utils.c.lsan.o | bin
	gcc -o $@ $^ -llsan

bin/%.c.o: src/%.c | bin bin/deps
	gcc -c $(CFLAGS) $(CLAGS_RELEASE) -o $@ -MD -MF bin/deps/$(notdir $@).mk $<
bin/%.c.debug.o: src/%.c | bin bin/deps
	gcc -c $(CFLAGS) $(CFLAGS_DEBUG) -o $@ -MD -MF bin/deps/$(notdir $@).mk $<
bin/%.c.asan.o: src/%.c | bin bin/deps
	gcc -c $(CFLAGS) $(CFLAGS_DEBUG) -fsanitize=address -o $@ -MD -MF bin/deps/$(notdir $@).mk $<
bin/%.c.ubsan.o: src/%.c | bin bin/deps
	gcc -c $(CFLAGS) $(CFLAGS_DEBUG) -fsanitize=undefined -o $@ -MD -MF bin/deps/$(notdir $@).mk $<
bin/%.c.lsan.o: src/%.c | bin bin/deps
	gcc -c $(CFLAGS) $(CFLAGS_DEBUG) -fsanitize=leak -o $@ -MD -MF bin/deps/$(notdir $@).mk $<

bin:
	mkdir bin

bin/deps: | bin
	mkdir bin/deps

include $(wildcard bin/deps/*)
