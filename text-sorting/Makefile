CC=gcc
CXX=g++
AR=ar
TARGETS=build/main build/main_debug build/main_asan build/main_ubsan build/main_lsan build/text.a

ifeq ($(TARGET),MINGW64_X32)
CC=i686-w64-mingw32-gcc
CXX=i686-w64-mingw32-gcc
TARGETS=build/main build/main_debug
endif
ifeq ($(TARGET),MINGW64_X64)
CC=x86_64-w64-mingw32-gcc
CXX=x86_64-w64-mingw32-gcc
TARGETS=build/main build/main_debug
endif

ifneq ($(NO_DOCS),1)
TARGETS += docs
endif

CFLAGS=-Wall -pedantic -Wextra -Wshadow -Werror -std=c99 -Iinclude
CFLAGS_RELEASE=-O2
CFLAGS_DEBUG=-g -O0 -DTEXT_DEBUG

all: $(TARGETS)

clean:
	rm -r build

docs: | build
	doxygen doxygen.config

.PHONY: all clean docs

build/main: build/main.c.o build/text.c.o build/string_utils.c.o build/my_qsort.c.o | build
	$(CC) -o $@ $^
build/main_debug: build/main.c.debug.o build/text.c.debug.o build/string_utils.c.debug.o build/my_qsort.c.debug.o | build
	$(CC) -o $@ $^
build/main_asan: build/main.c.asan.o build/text.c.asan.o build/string_utils.c.asan.o build/my_qsort.c.asan.o | build
	$(CC) -o $@ $^ -lasan
build/main_ubsan: build/main.c.ubsan.o build/text.c.ubsan.o build/string_utils.c.ubsan.o build/my_qsort.c.ubsan.o | build
	$(CC) -o $@ $^ -lubsan
build/main_lsan: build/main.c.lsan.o build/text.c.lsan.o build/string_utils.c.lsan.o build/my_qsort.c.lsan.o | build
	#$(CC) -o $@ $^ -llsan

build/text.a: build/text.c.o | build
	$(AR) -r $@ $^

build/%.c.o: src/%.c | build build/deps
	$(CC) -c $(CFLAGS) $(CFLAGS_RELEASE) -o $@ -MD -MF build/deps/$(notdir $@).mk $<
build/%.c.debug.o: src/%.c | build build/deps
	$(CC) -c $(CFLAGS) $(CFLAGS_DEBUG) -o $@ -MD -MF build/deps/$(notdir $@).mk $<
build/%.c.asan.o: src/%.c | build build/deps
	$(CC) -c $(CFLAGS) $(CFLAGS_DEBUG) -fsanitize=address -o $@ -MD -MF build/deps/$(notdir $@).mk $<
build/%.c.ubsan.o: src/%.c | build build/deps
	$(CC) -c $(CFLAGS) $(CFLAGS_DEBUG) -fsanitize=undefined -o $@ -MD -MF build/deps/$(notdir $@).mk $<
build/%.c.lsan.o: src/%.c | build build/deps
	$(CC) -c $(CFLAGS) $(CFLAGS_DEBUG) -fsanitize=leak -o $@ -MD -MF build/deps/$(notdir $@).mk $<

build:
	mkdir build

build/deps: | build
	mkdir build/deps

include $(wildcard build/deps/*)
