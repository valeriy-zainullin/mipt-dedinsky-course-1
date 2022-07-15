CFLAGS = -Wall -pedantic -Wextra -Wshadow -Wconversion -Warray-bounds -Wimplicit-fallthrough=5 -Werror -std=c11 -Iinclude
CFLAGS_REL = -O2 -DNDEBUG -DRELEASE_BUILD=1 -DDEBUG_BUILD=0
CFLAGS_DBG = -g -O0 -DRELEASE_BUILD=0 -DDEBUG_BUILD=1

# TODO: make target specific directories inside build. Build for target upon a request, by default use host target.

all: present

present: build/tokenizer
	build/tokenizer < simple_file.c

build/genrtd_src/tokenizer.c: src/tokenizer.l | build/genrtd_src
	flex -o $@ $<

build/objects/tokenizer.obj: build/genrtd_src/tokenizer.c | build/objects
	gcc -c $(CFLAGS) $(CFLAGS_REL) -o $@ $<

build/objects/%.obj: src/%.c | build/objects
	gcc -c $(CFLAGS) $(CFLAGS_REL) -o $@ $<

build/objects/%.dbg.obj: src/%.c | build/objects
	gcc -c $(CFLAGS) $(CFLAGS_DBG) -o $@ $<
	
build/tokenizer: build/objects/tokenizer.obj build/objects/tokenizer_main.obj # build/tokenizer$(EXECUTABLE_OS_EXTENSION}
	gcc -o $@ $^

build/tokenizer.dbg: build/objects/tokenizer.obj build/objects/tokenizer_main.dbg.obj # build/tokenizer$(EXECUTABLE_OS_EXTENSION}
	gcc -o $@ $^

build:
	mkdir $@

build/objects build/genrtd_src: | build
	mkdir $@