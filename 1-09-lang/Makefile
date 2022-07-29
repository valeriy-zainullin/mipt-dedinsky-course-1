CFLAGS = -Wall -pedantic -Wextra -Wshadow -Wconversion -Warray-bounds -Wimplicit-fallthrough=5 -Werror -std=c11 -Iinclude
CFLAGS_REL = -O2 -DNDEBUG -DRELEASE_BUILD=1 -DDEBUG_BUILD=0
CFLAGS_DBG = -g -O0 -DRELEASE_BUILD=0 -DDEBUG_BUILD=1 -DYYDEBUG=1
CFLAGS_DBG_SAN = -fsanitize=address,undefined

# TODO: make target specific directories inside build. Build for target upon a request, by default use host target.
# TODO: make gcc output makefiles.

all: present

present: parser_tests
	
tokenizer_tests: build/tokenizer.dbg.san
	$< < simple_file.c
	
parser_tests: build/parser.dbg # builld/parser.dbg.san # leaks of ast nodes in parsing are not fixed for now.
	#$< < samples/1-simple_main.c
	#$< < samples/2-simple-struct.c
	#$< < samples/3-printf.c
	$< < simplified-lang-tests/1-simple_main.c

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
	bison -Wall -l -o $@ $<

build/objects/%.obj: build/genrtd_src/%.c | build/objects
	gcc -c $(CFLAGS) $(CFLAGS_REL) -o $@ $<

build/objects/%.dbg.obj: build/genrtd_src/%.c | build/objects
	gcc -c $(CFLAGS) $(CFLAGS_DBG) -o $@ $<
	
build/objects/%.dbg.san.obj: build/genrtd_src/%.c | build/objects
	gcc -c $(CFLAGS) $(CFLAGS_DBG) $(CFLAGS_DBG_SAN) -o $@ $<

build/objects/%.obj: src/%.c | build/objects
	gcc -c $(CFLAGS) $(CFLAGS_REL) -o $@ $<

build/objects/%.dbg.obj: src/%.c | build/objects
	gcc -c $(CFLAGS) $(CFLAGS_DBG) -o $@ $<
	
build/objects/%.dbg.san.obj: src/%.c | build/objects
	gcc -c $(CFLAGS) $(CFLAGS_DBG) $(CFLAGS_DBG_SAN) -o $@ $<

build/tokenizer: build/objects/tokenizer.obj build/objects/tokenizer_main.obj # build/tokenizer$(EXECUTABLE_OS_EXTENSION}
	gcc -o $@ $^

build/tokenizer.dbg: build/objects/tokenizer.dbg.obj build/objects/tokenizer_main.dbg.obj # build/tokenizer$(EXECUTABLE_OS_EXTENSION}
	gcc -o $@ $^
	
build/tokenizer.dbg.san: build/objects/tokenizer.dbg.san.obj build/objects/tokenizer_main.dbg.san.obj # build/tokenizer$(EXECUTABLE_OS_EXTENSION}
	gcc $(CFLAGS_DBG_SAN) -o $@ $^

build/parser: build/objects/tokenizer.obj build/objects/parser.obj build/objects/ast.obj build/objects/vector.obj build/objects/parser_main.obj
	gcc -o $@ $^

build/parser.dbg: build/objects/tokenizer.dbg.obj build/objects/parser.dbg.obj build/objects/ast.dbg.obj build/objects/vector.dbg.obj build/objects/parser_main.dbg.obj
	gcc -o $@ $^

build/parser.dbg.san: build/objects/tokenizer.dbg.san.obj build/objects/parser.dbg.san.obj build/objects/ast.dbg.san.obj build/objects/vector.dbg.san.obj build/objects/parser_main.dbg.san.obj
	gcc $(CFLAGS_DBG_SAN) -o $@ $^

build build/objects build/genrtd_src:
	mkdir -p $@

# Don't use embedded make rules.
# Makes it not run yacc instead of bison without my permission.
.SUFFIXES:

# # Don't delete intermidiate files if no error occurs. I sometimes want to see bison output.
# # I could request it though... That's what I do. This message is just as an option here for others and future.
# # https://stackoverflow.com/a/67780778
# .SECONDARY: