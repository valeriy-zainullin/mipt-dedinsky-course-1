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

TARGETS=main libvector_vm

EXECUTABLES=main
LIBRARIES=libvector_vm
SOURCES=$(wildcard src/*.cpp) $(wildcard exported/*.cpp)

main_OBJECTS=$(addsuffix .obj, $(wildcard src/*.cpp))
main_LIBS=

libvector_vm_OBJECTS=$(addsuffix .obj, exported/vector_vm.cpp)

ifneq ($(NO_DOCS),1)
#TARGETS += docs
endif

CFLAGS=-pedantic -Werror -std=c99 -Iinclude -I../text-sorting/include -I../stack/include \
  -Wall -Wextra -Weffc++ -Wc++0x-compat -Wc++11-compat -Wc++14-compat -Waggressive-loop-optimizations \
  -Walloc-zero -Walloca -Walloca-larger-than=8192 -Warray-bounds -Wcast-align -Wcast-qual \
  -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wdangling-else \
  -Wduplicated-branches -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security \
  -Wformat-signedness -Wformat=2 -Wformat-overflow=2 -Wformat-truncation=2 -Winline -Wlarger-than=8192 \
  -Wvla-larger-than=8192 -Wlogical-op -Wmissing-declarations -Wnon-virtual-dtor -Wopenmp-simd \
  -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wrestrict -Wshadow -Wsign-promo \
  -Wstack-usage=8192 -Wstrict-null-sentinel -Wstrict-overflow=2 -Wstringop-overflow=4 \
  -Wsuggest-attribute=noreturn -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum \
  -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-literal-suffix \
  -Wno-missing-field-initializers -Wnarrowing -Wno-old-style-cast -Wvarargs -Waligned-new \
  -Walloc-size-larger-than=1073741824 -Walloc-zero -Walloca -Walloca-larger-than=8192 \
  -Wdangling-else -Wduplicated-branches -Wformat-overflow=2 -Wformat-truncation=2 -Wmissing-attributes \
  -Wmultistatement-macros -Wrestrict -Wshadow=global -Wsuggest-attribute=malloc -Wno-unsued-result \
  -fcheck-new -fsized-deallocation -fstack-check -fstrict-overflow -flto-odr-type-merging
CFLAGS_RELEASE=-O2
CFLAGS_DEBUG=-g -O0

CXXFLAGS= \
  -pedantic -Werror -std=c++11 -Iinclude \
  -Wall -Wextra -Weffc++ -Wc++0x-compat -Wc++11-compat -Wc++14-compat -Waggressive-loop-optimizations \
  -Walloc-zero -Walloca -Walloca-larger-than=8192 -Warray-bounds -Wcast-align -Wchar-subscripts \
  -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wdangling-else -Wduplicated-branches \
  -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 \
  -Wformat-overflow=2 -Wformat-truncation=2 -Winline -Wlarger-than=8192 -Wvla-larger-than=8192 -Wlogical-op \
  -Wmissing-declarations -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith \
  -Wredundant-decls -Wrestrict -Wshadow -Wsign-promo -Wstack-usage=8192 -Wstrict-null-sentinel \
  -Wstrict-overflow=2 -Wstringop-overflow=4 -Wsuggest-attribute=noreturn -Wsuggest-final-types \
  -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused \
  -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wnarrowing -Wno-old-style-cast \
  -Wvarargs -Waligned-new -Walloc-size-larger-than=1073741824 -Walloc-zero -Walloca \
  -Walloca-larger-than=8192 -Wdangling-else -Wduplicated-branches -Wformat-overflow=2 -Wformat-truncation=2 \
  -Wrestrict -Wshadow=global -fcheck-new -Wno-unused-result -fsized-deallocation -fstrict-overflow \
  -flto-odr-type-merging
# Disabling -fstack-check, otherwise doesn't not compile on linux.
CXXFLAGS_RELEASE=-O2
CXXFLAGS_DEBUG=-g -O0

define EXECUTABLE_RULE =
build/release/$(1): $$(addprefix build/obj/release/, $$($(1)_OBJECTS)) $$($(1)_LIBS) | build/release
	$(CXX) -o $$@ $$(addprefix build/obj/release/, $$($(1)_OBJECTS)) $$(addprefix -L, $$(dir $$($(1)_LIBS))) $$(addprefix -l, $$(patsubst lib%.a, %, $$(notdir $$($(1)_LIBS))))
build/debug/$(1): $$(addprefix build/obj/debug/, $$($(1)_OBJECTS)) | build/debug
	$(CXX) -o $$@ $$(addprefix build/obj/debug/, $$($(1)_OBJECTS)) $$(addprefix -L, $$(dir $$($(1)_LIBS))) $$(addprefix -l, $$(patsubst lib%.a, %, $$(notdir $$($(1)_LIBS))))
build/asan/$(1): $$(addprefix build/obj/asan/, $$($(1)_OBJECTS)) | build/asan
	$(CXX) -o $$@ $$(addprefix build/obj/asan/, $$($(1)_OBJECTS)) -lasan $$(addprefix -L, $$(dir $$($(1)_LIBS))) $$(addprefix -l, $$(patsubst lib%.a, %, $$(notdir $$($(1)_LIBS))))
build/ubsan/$(1): $$(addprefix build/obj/ubsan/, $$($(1)_OBJECTS)) | build/ubsan
	$(CXX) -o $$@ $$(addprefix build/obj/ubsan/, $$($(1)_OBJECTS)) -lubsan $$(addprefix -L, $$(dir $$($(1)_LIBS))) $$(addprefix -l, $$(patsubst lib%.a, %, $$(notdir $$($(1)_LIBS))))
endef

define LIBRARY_RULE =
build/release/$(1).a: $$(addprefix build/obj/release/, $$($(1)_OBJECTS)) | build/release
	$(AR) -r $$@ $$@ $$^
build/debug/$(1).a: $$(addprefix build/obj/debug/, $$($(1)_OBJECTS)) | build/debug
	$(AR) -r $$@ $$@ $$^
build/asan/$(1).a: $$(addprefix build/obj/asan/, $$($(1)_OBJECTS)) | build/asan
	$(AR) -r $$@ $$@ $$^
build/ubsan/$(1).a: $$(addprefix build/obj/ubsan/, $$($(1)_OBJECTS)) | build/ubsan
	$(AR) -r $$@ $$@ $$^
endef

define SOURCE_RULE =
build/obj/release/$(1).obj : $(1) | build/obj/release
	mkdir -p $$(dir $$@)
	$$(CXX) -c $$(CXXFLAGS) $$(CXXFLAGS_RELEASE) -o $$@ -MD $$<
build/obj/debug/$(1).obj : $(1) | build/obj/debug
	mkdir -p $$(dir $$@)
	$$(CXX) -c $$(CXXFLAGS) $$(CXXFLAGS_DEBUG) -o $$@ -MD $$<
build/obj/asan/$(1).obj : $(1) | build/obj/asan
	mkdir -p $$(dir $$@)
	$$(CXX) -c $$(CXXFLAGS) $$(CXXFLAGS_DEBUG) -fsanitize=address -o $$@ -MD $$<
build/obj/ubsan/$(1).obj : $(1) | build/obj/ubsan
	mkdir -p $$(dir $$@)
	$$(CXX) -c $$(CXXFLAGS) $$(CXXFLAGS_DEBUG) -fsanitize=undefined -o $$@ -MD $$<
endef

all: $(TARGETS)

clean:
	rm -r build

docs: | build
	# doxygen doxygen.config

.PHONY: all clean docs $(TARGETS)

ifneq ($(IS_CYGWIN),CYGWIN)
$(EXECUTABLES) : % : build/release/% build/debug/% build/asan/% build/ubsan/%
$(LIBRARIES) : % : build/release/%.a build/debug/%.a build/asan/%.a build/ubsan/%.a
else
$(EXECUTABLES) : % : build/release/% build/debug/%
$(LIBRARIES) : % : build/release/%.a build/debug/%.a
endif

$(foreach EXECUTABLE,$(EXECUTABLES),$(eval $(call EXECUTABLE_RULE,$(EXECUTABLE))))
$(foreach LIBRARY,$(LIBRARIES),$(eval $(call LIBRARY_RULE,$(LIBRARY))))
$(foreach SOURCE,$(SOURCES),$(eval $(call SOURCE_RULE,$(SOURCE))))

build:
	mkdir $@
build/release build/debug build/asan build/ubsan build/obj: | build
	mkdir $@
build/obj/release build/obj/debug build/obj/asan build/obj/ubsan: | build/obj
	mkdir $@

include $(wildcard $(patsubst %, build/obj/*/%.d, $(SOURCES)))
