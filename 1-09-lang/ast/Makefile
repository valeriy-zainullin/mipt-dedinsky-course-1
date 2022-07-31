include ../flags.mk

CFLAGS += -Iinclude/mycc-ast

C_SOURCES = $(patsubst src/%.c,%.c,$(wildcard src/*.c))	

# TODO: make target specific directories inside build. Build for target upon a request, by default use host target.
# TODO: make gcc output makefiles.

all: build/libmycc-ast.so build/libmycc-ast.dbg.so libmycc-ast.dbg.san.so

clean:
	rm -r build 

.PHONY: all clean

build/objects/%.obj: src/%.c | build/objects
	gcc -c $(CFLAGS) $(CFLAGS_REL) -o $@ $<

build/objects/%.dbg.obj: src/%.c | build/objects
	gcc -c $(CFLAGS) $(CFLAGS_DBG) -o $@ $<
	
build/objects/%.dbg.san.obj: src/%.c | build/objects
	gcc -c $(CFLAGS) $(CFLAGS_DBG) $(CFLAGS_DBG_SAN) -o $@ $<
	
build/libmycc-ast.so: $(patsubst src/%.c,build/objects/%.obj)
	gcc -shared -o $@ $^
build/libmycc-ast.dbg.so: $(patsubst %.c,build/objects/%.dbg.obj,$(C_SOURCES))
	gcc -shared -o $@ $^
build/libmycc-ast.dbg.san.so: $(patsubst %.c,build/objects/%.dbg.san.obj,$(C_SOURCES))
	gcc -shared -o $@ $^

build build/objects:
	mkdir -p $@

# Don't use embedded make rules.
# Makes it not run yacc instead of bison without my permission.
.SUFFIXES:

# # Don't delete intermidiate files if no error occurs. I sometimes want to see bison output.
# # I could request it though... That's what I do. This message is just as an option here for others and future.
# # https://stackoverflow.com/a/67780778
# .SECONDARY: