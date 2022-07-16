CFLAGS = -Wall -pedantic -Wextra -Wshadow -Wconversion -Warray-bounds -Wimplicit-fallthrough=5 -Werror -std=c11 -Iinclude
CFLAGS_REL = -O2 -DNDEBUG -DRELEASE_BUILD=1 -DDEBUG_BUILD=0
CFLAGS_DBG = -g -O0 -DRELEASE_BUILD=0 -DDEBUG_BUILD=1 -DYYDEBUG=1

# TODO: make target specific directories inside build. Build for target upon a request, by default use host target.

all: present

present: build/parser.dbg
	build/parser.dbg < samples/1-simple_main.c
	
test_tokenizer: build/tokenizer
	build/tokenizer < simple_file.c

parser_counterexamples: src/parser.y
	bison -Wall -fsyntax-only -Wcounterexamples $<

parser_graph: parser_graph.png

parser_graph.png: src/parser.y
	# Doesn't generate picture with -fsyntax-only.
	bison -Wall --graph=$@.dot -o $@.c $< || rm $@.c
	dot -o $@ $@.dot || rm $@.dot
	rm $@.dot

clean:
	rm -r build parser_graph*

.PHONY: all clean present parser_counterexamples parser_graph tokenizer_tests

build/genrtd_src/%.c: src/%.l | build/genrtd_src
	flex -o $@ $<

build/genrtd_src/%.c: src/%.y | build/genrtd_src
	# bison -Wall -Werror -o $@ $<
	bison -Wall -o $@ $<

build/objects/%.obj: build/genrtd_src/%.c | build/objects
	gcc -c $(CFLAGS) $(CFLAGS_REL) -o $@ $<

build/objects/%.dbg.obj: build/genrtd_src/%.c | build/objects
	gcc -c $(CFLAGS) $(CFLAGS_DBG) -o $@ $<
	
build/objects/%.obj: src/%.c | build/objects
	gcc -c $(CFLAGS) $(CFLAGS_REL) -o $@ $<

build/objects/%.dbg.obj: src/%.c | build/objects
	gcc -c $(CFLAGS) $(CFLAGS_DBG) -o $@ $<
	
build/tokenizer: build/objects/tokenizer.obj build/objects/tokenizer_main.obj # build/tokenizer$(EXECUTABLE_OS_EXTENSION}
	gcc -o $@ $^

build/tokenizer.dbg: build/objects/tokenizer.obj build/objects/tokenizer_main.dbg.obj # build/tokenizer$(EXECUTABLE_OS_EXTENSION}
	gcc -o $@ $^
	
build/parser: build/objects/tokenizer.obj build/objects/parser.obj build/objects/parser_main.obj
	gcc -o $@ $^

build/parser.dbg: build/objects/tokenizer.dbg.obj build/objects/parser.dbg.obj build/objects/parser_main.dbg.obj
	gcc -o $@ $^

build:
	mkdir $@

build/objects build/genrtd_src: | build
	mkdir $@