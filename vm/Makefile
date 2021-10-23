CC=gcc
CXX=g++
TARGETS=as

EXECUTABLES=as
SOURCES=src/assembler.cpp

as_OBJECTS=assembler.cpp.obj

ifneq ($(NO_DOCS),1)
#TARGETS += docs
endif

CFLAGS=-Wall -pedantic -Wextra -Wshadow -Werror -std=c99 -Iinclude
CFLAGS_RELEASE=-O2
CFLAGS_DEBUG=-g -O0

CXXFLAGS=-Wall -pedantic -Wextra -Wshadow -Werror -std=c++11 -Iinclude
CXXFLAGS_RELEASE=-O2
CXXFLAGS_DEBUG=-g -O0

define EXECUTABLE_RULE =
build/release/$(1): $$(addprefix build/obj/release/, $$($(1)_OBJECTS)) | build/release
	$(CXX) -o $$@ $$^
build/debug/$(1): $$(addprefix build/obj/debug/, $$($(1)_OBJECTS)) | build/debug
	$(CXX) -o $$@ $$^
build/asan/$(1): $$(addprefix build/obj/asan/, $$($(1)_OBJECTS)) | build/asan
	$(CXX) -o $$@ $$^ -lasan
build/ubsan/$(1): $$(addprefix build/obj/ubsan/, $$($(1)_OBJECTS)) | build/ubsan
	$(CXX) -o $$@ $$^ -lubsan
build/lsan/$(1): $$(addprefix build/obj/lsan/, $$($(1)_OBJECTS)) | build/lsan
	#$(CXX) -o $$@ $$^ -llsan
endef

define SOURCE_RULE =
build/obj/release/$$(notdir $(1)).obj : $(1) | build/obj/release
	$$(CXX) -c $$(CXXFLAGS) $$(CXXLAGS_RELEASE) -o $$@ -MD $$<
build/obj/debug/$$(notdir $(1)).obj : $(1) | build/obj/debug
	$$(CXX) -c $$(CXXFLAGS) $$(CXXFLAGS_DEBUG) -o $$@ -MD $$<
build/obj/asan/$$(notdir $(1)).obj : $(1) | build/obj/asan
	$$(CXX) -c $$(CXXFLAGS) $$(CXXFLAGS_DEBUG) -fsanitize=address -o $$@ -MD $$<
build/obj/ubsan/$$(notdir $(1)).obj : $(1) | build/obj/ubsan
	$$(CXX) -c $$(CXXFLAGS) $$(CXXFLAGS_DEBUG) -fsanitize=undefined -o $$@ -MD $$<
build/obj/lsan/$$(notdir $(1)).obj : $(1) | build/obj/lsan
	$$(CXX) -c $$(CXXFLAGS) $$(CXXFLAGS_DEBUG) -fsanitize=leak -o $$@ -MD $$<
endef

all: $(TARGETS)

clean:
	rm -r build

docs: | build
	# doxygen doxygen.config

.PHONY: all clean docs $(TARGETS)

$(TARGETS) : % : build/release/% build/debug/% build/asan/% build/lsan/% build/ubsan/%

$(foreach EXECUTABLE,$(EXECUTABLES),$(eval $(call EXECUTABLE_RULE,$(EXECUTABLE))))
$(foreach SOURCE,$(SOURCES),$(eval $(call SOURCE_RULE,$(SOURCE))))

build:
	mkdir $@
build/release build/debug build/asan build/ubsan build/lsan build/obj: | build
	mkdir $@
build/obj/release build/obj/debug build/obj/asan build/obj/ubsan build/obj/lsan: | build/obj
	mkdir $@

include $(wildcard build/obj/*/*.d)
