CC=gcc
CXX=g++
AR=ar
TARGETS=as vm

EXECUTABLES=as vm
SOURCES=$(wildcard src/assembly/*.cpp) $(wildcard src/assembler/*.cpp) $(wildcard src/bytecode/*.cpp) $(wildcard src/vm/*.cpp) $(wildcard src/support/*.cpp)

as_OBJECTS=$(addsuffix .obj, $(wildcard src/assembler/*.cpp)) $(addsuffix .obj, $(wildcard src/assembly/*.cpp)) src/bytecode/operation.cpp.obj src/support/forward_stream.cpp.obj
as_LIBS=../text-sorting/build/libtext.a

vm_OBJECTS=$(addsuffix .obj, $(wildcard src/vm/*.cpp)) src/bytecode/operation.cpp.obj src/support/forward_stream.cpp.obj
vm_LIBS=../stack/build/libstack_int.a


ifneq ($(NO_DOCS),1)
#TARGETS += docs
endif

CFLAGS=-pedantic -Werror -std=c99 -Iinclude -I../text-sorting/include -I../stack/include -DCLI_SUPPORTS_STDIN=0 \
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
  -Wmultistatement-macros -Wrestrict -Wshadow=global -Wsuggest-attribute=malloc -fcheck-new \
  -fsized-deallocation -fstack-check -fstrict-overflow -flto-odr-type-merging
CFLAGS_RELEASE=-O2 -DVM_DEBUG=0
CFLAGS_DEBUG=-g -O0 -DVM_DEBUG=1

CXXFLAGS= \
  -pedantic -Werror -std=c++11 -Iinclude -I../text-sorting/include -I../stack/include -DCLI_SUPPORTS_STDIN=0 \
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
  -Walloc-size-larger-than=1073741824 -Walloc-zero -Walloca -Walloca-larger-than=8192 -Wdangling-else \
  -Wduplicated-branches -Wformat-overflow=2 -Wformat-truncation=2 -Wrestrict -Wshadow=global -fcheck-new \
  -fsized-deallocation -fstack-check -fstrict-overflow -flto-odr-type-merging
CXXFLAGS_RELEASE=-O2 -DVM_DEBUG=0
CXXFLAGS_DEBUG=-g -O0 -DVM_DEBUG=1

define EXECUTABLE_RULE =
build/release/$(1): $$(addprefix build/obj/release/, $$($(1)_OBJECTS)) $$($(1)_LIBS) | build/release
	$(CXX) -o $$@ $$(addprefix build/obj/release/, $$($(1)_OBJECTS)) $$(addprefix -L, $$(dir $$($(1)_LIBS))) $$(addprefix -l, $$(patsubst lib%.a, %, $$(notdir $$($(1)_LIBS))))
build/debug/$(1): $$(addprefix build/obj/debug/, $$($(1)_OBJECTS)) | build/debug
	$(CXX) -o $$@ $$(addprefix build/obj/debug/, $$($(1)_OBJECTS)) $$(addprefix -L, $$(dir $$($(1)_LIBS))) $$(addprefix -l, $$(patsubst lib%.a, %, $$(notdir $$($(1)_LIBS))))
build/asan/$(1): $$(addprefix build/obj/asan/, $$($(1)_OBJECTS)) | build/asan
	$(CXX) -o $$@ $$(addprefix build/obj/asan/, $$($(1)_OBJECTS)) -lasan $$(addprefix -L, $$(dir $$($(1)_LIBS))) $$(addprefix -l, $$(patsubst lib%.a, %, $$(notdir $$($(1)_LIBS))))
build/ubsan/$(1): $$(addprefix build/obj/ubsan/, $$($(1)_OBJECTS)) | build/ubsan
	$(CXX) -o $$@ $$(addprefix build/obj/ubsan/, $$($(1)_OBJECTS)) -lubsan $$(addprefix -L, $$(dir $$($(1)_LIBS))) $$(addprefix -l, $$(patsubst lib%.a, %, $$(notdir $$($(1)_LIBS))))
build/lsan/$(1): $$(addprefix build/obj/lsan/, $$($(1)_OBJECTS)) | build/lsan
	#$(CXX) -o $$@ $$(addprefix build/obj/lsan/, $$($(1)_OBJECTS)) -llsan $$(addprefix -L, $$(dir $$($(1)_LIBS))) $$(addprefix -l, $$(patsubst lib%.a, %, $$(notdir $$($(1)_LIBS))))
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
build/obj/lsan/$(1).obj : $(1) | build/obj/lsan
	mkdir -p $$(dir $$@)
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

include $(wildcard $(patsubst %, build/obj/*/%.d, $(SOURCES)))
