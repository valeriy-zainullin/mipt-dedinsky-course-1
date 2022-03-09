IS_CYGWIN = $(findstring CYGWIN, $(shell uname -a))

ifndef CC
  CC = gcc
endif

ifndef CXX
  CXX = g++
endif

ifndef AR
  AR = ar
endif

ifneq ($(IS_CYGWIN),CYGWIN)
TARGETS=build/main build/main_debug build/main_asan build/main_ubsan build/main_lsan build/libstack_vm.a
else
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
build/libstack_vm.a: build/stack_vm.cpp.o | build
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
