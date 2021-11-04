CC=gcc
CXX=g++
AR=ar
TARGETS=build/main build/main_debug build/main_asan build/main_ubsan build/main_lsan build/libstack_int.a

ifeq ($(TARGET),MINGW64_X32)
CC=i686-w64-mingw32-gcc
CXX=i686-w64-mingw32-g++
TARGETS=build/main build/main_debug
endif
ifeq ($(TARGET),MINGW64_X64)
CC=x86_64-w64-mingw32-gcc
CXX=x86_64-w64-mingw32-g++
TARGETS=build/main build/main_debug
endif

ifneq ($(NO_DOCS),1)
TARGETS += docs
endif

CFLAGS=-Wall -pedantic -Wextra -Wshadow -Werror -std=c99 -Iinclude
CFLAGS_RELEASE=-O2
CFLAGS_DEBUG=-g -O0

CXXFLAGS=-Wall -pedantic -Wextra -Wshadow -Werror -std=c++11 -Iinclude
CXXFLAGS_RELEASE=-O2
CXXFLAGS_DEBUG=-g -O0

all: $(TARGETS)

clean:
	rm -r build

docs: | build
	# doxygen doxygen.config

.PHONY: all clean docs

build/main: build/main.cpp.o build/stack.cpp.o | build
	$(CXX) -o $@ $^
build/main_debug: build/main.cpp.debug.o build/stack.cpp.debug.o | build
	$(CXX) -o $@ $^
build/main_asan: build/main.cpp.asan.o build/stack.cpp.asan.o | build
	$(CXX) -o $@ $^ -lasan
build/main_ubsan: build/main.cpp.ubsan.o build/stack.cpp.ubsan.o | build
	$(CXX) -o $@ $^ -lubsan
build/main_lsan: build/main.cpp.lsan.o build/stack.cpp.lsan.o | build
	# $(CXX) -o $@ $^ -llsan
build/libstack_int.a: build/stack.cpp.o | build
	$(AR) -r $@ $@ $^

build/%.cpp.o: src/%.cpp | build build/deps
	$(CXX) -c $(CXXFLAGS) $(CXXLAGS_RELEASE) -o $@ -MD -MF build/deps/$(notdir $@).mk $<
build/%.cpp.debug.o: src/%.cpp | build build/deps
	$(CXX) -c $(CXXFLAGS) $(CXXFLAGS_DEBUG) -o $@ -MD -MF build/deps/$(notdir $@).mk $<
build/%.cpp.asan.o: src/%.cpp | build build/deps
	$(CXX) -c $(CXXFLAGS) $(CXXFLAGS_DEBUG) -fsanitize=address -o $@ -MD -MF build/deps/$(notdir $@).mk $<
build/%.cpp.ubsan.o: src/%.cpp | build build/deps
	$(CXX) -c $(CXXFLAGS) $(CXXFLAGS_DEBUG) -fsanitize=undefined -o $@ -MD -MF build/deps/$(notdir $@).mk $<
build/%.cpp.lsan.o: src/%.cpp | build build/deps
	# $(CXX) -c $(CXXFLAGS) $(CXXFLAGS_DEBUG) -fsanitize=leak -o $@ -MD -MF build/deps/$(notdir $@).mk $<

build:
	mkdir build

build/deps: | build
	mkdir build/deps

include $(wildcard build/deps/*)
