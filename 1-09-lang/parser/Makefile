include ../flags.mk

CFLAGS += -Iinclude/mycc-parser

CFLAGS_DBG += -DYYDEBUG=1

# TODO: idea for porting: just make more rules for different file names. Conditionally set variable with parser executable name on the platform. Or create subdirectories for different platforms. Make a note "if on windows, run with cygwin make and bash."

all: present

present: parser_tests
	
tokenizer_tests: $(BUILD_DIR)/tokenizer.dbg.san
	$< < ../simplified-lang-tests/0-simpliest_main.c
	$< < ../simplified-lang-tests/1-multiplicative-return-main.c 
	$< < ../simplified-lang-tests/2-additive-return-main.c 
	$< < ../simplified-lang-tests/3-bit_shift-return-main.c 
	
parser_tests: $(BUILD_DIR)/parser.dbg.san # builld/parser.dbg.san # leaks of ast nodes in parsing are not fixed for now.
	#$< < samples/1-simple_main.c
	#$< < samples/2-simple-struct.c
	#$< < samples/3-printf.c
	#$< < ../simplified-lang-tests/1-simple_main.c
	$< < ../simplified-lang-tests/0-simpliest_main.c
	$< < ../simplified-lang-tests/1-multiplicative-return-main.c 
	$< < ../simplified-lang-tests/2-additive-return-main.c 
	$< < ../simplified-lang-tests/3-bit_shift-return-main.c 

parser_counterexamples: src/parser.y
	bison -Wall -fsyntax-only -Wcounterexamples $<

parser_graph: parser_graph.png

parser_graph.png: src/parser.y
	# Doesn't generate picture with -fsyntax-only.
	bison -Wall --graph=$@.dot -o $@.c $< || rm $@.c
	dot -o $@ $@.dot || rm $@.dot
	rm $@.dot

clean:
	rm -r $(BUILD_DIR) $(OBJECT_DIR) parser_graph*

.PHONY: all clean present parser_counterexamples parser_graph tokenizer_tests

$(OBJECT_DIR)/genrtd_src/%.c: src/%.l | $(OBJECT_DIR)/genrtd_src
	flex -o $@ $<

$(OBJECT_DIR)/genrtd_src/%.c: src/%.y | $(OBJECT_DIR)/genrtd_src
	# bison -Wall -Werror -o $@ $<
	bison -Wall -l -o $@ $<

$(OBJECT_DIR)/%.obj: $(OBJECT_DIR)/genrtd_src/%.c | $(OBJECT_DIR)
	gcc -c $(CFLAGS) $(CFLAGS_REL) -o $@ $<

$(OBJECT_DIR)/%.dbg.obj: $(OBJECT_DIR)/genrtd_src/%.c | $(OBJECT_DIR)
	gcc -c $(CFLAGS) $(CFLAGS_DBG) -o $@ $<
	
$(OBJECT_DIR)/%.dbg.san.obj: $(OBJECT_DIR)/genrtd_src/%.c | $(OBJECT_DIR)
	gcc -c $(CFLAGS) $(CFLAGS_DBG) $(CFLAGS_DBG_SAN) -o $@ $<

$(OBJECT_DIR)/%.obj: src/%.c | $(OBJECT_DIR)
	gcc -c $(CFLAGS) $(CFLAGS_REL) -o $@ $<

$(OBJECT_DIR)/%.dbg.obj: src/%.c | $(OBJECT_DIR)
	gcc -c $(CFLAGS) $(CFLAGS_DBG) -o $@ $<
	
$(OBJECT_DIR)/%.dbg.san.obj: src/%.c | $(OBJECT_DIR)
	gcc -c $(CFLAGS) $(CFLAGS_DBG) $(CFLAGS_DBG_SAN) -o $@ $<
	
$(BUILD_DIR)/libmycc-parser.so: $(patsubst %.c,$(OBJECT_DIR)/%.obj,parser.c tokenizer.c) $(BUILD_DIR)/libmycc-ast.so | $(BUILD_DIR)
	gcc -shared $(CFLAGS_LINK) -o $@ $(filter %.obj,$^) -L$(BUILD_DIR) -lmycc-ast
$(BUILD_DIR)/libmycc-parser.dbg.so: $(patsubst %.c,$(OBJECT_DIR)/%.dbg.obj,parser.c tokenizer.c) $(BUILD_DIR)/libmycc-ast.dbg.so | $(BUILD_DIR)
	gcc -shared $(CFLAGS_LINK) -o $@ $(filter %.obj,$^) -L$(BUILD_DIR) -lmycc-ast.dbg
$(BUILD_DIR)/libmycc-parser.dbg.san.so: $(patsubst %.c,$(OBJECT_DIR)/%.dbg.san.obj,parser.c tokenizer.c) $(BUILD_DIR)/libmycc-ast.dbg.san.so | $(BUILD_DIR)
	gcc -shared $(CFLAGS_LINK) -o $@ $(filter %.obj,$^) -L$(BUILD_DIR) -lmycc-ast.dbg.san

$(BUILD_DIR)/tokenizer: $(patsubst %.c,$(OBJECT_DIR)/%.obj,tokenizer.c tokenizer_main.c) # build/tokenizer$(EXECUTABLE_OS_EXTENSION} | $(BUILD_DIR)
	gcc -o $@ $^
$(BUILD_DIR)/tokenizer.dbg: $(patsubst %.c,$(OBJECT_DIR)/%.dbg.obj,tokenizer.c tokenizer_main.c) # build/tokenizer$(EXECUTABLE_OS_EXTENSION} | $(BUILD_DIR)
	gcc -o $@ $^
$(BUILD_DIR)/tokenizer.dbg.san: $(patsubst %.c,$(OBJECT_DIR)/%.dbg.san.obj,tokenizer.c tokenizer_main.c) # build/tokenizer$(EXECUTABLE_OS_EXTENSION} | $(BUILD_DIR)
	gcc $(CFLAGS_DBG_SAN) -o $@ $^

$(BUILD_DIR)/parser: $(patsubst %.c,$(OBJECT_DIR)/%.obj,parser_main.c) $(BUILD_DIR)/libmycc-parser.so | $(BUILD_DIR)
	gcc $(CFLAGS_LINK) -o $@ $(filter %.obj,$^) -L$(BUILD_DIR) -lmycc-parser -lmycc-ast
$(BUILD_DIR)/parser.dbg: $(patsubst %.c,$(OBJECT_DIR)/%.dbg.obj,parser_main.c) $(BUILD_DIR)/libmycc-parser.dbg.so | $(BUILD_DIR)
	gcc $(CFLAGS_LINK) -o $@ $(filter %.obj,$^) -L$(BUILD_DIR) -lmycc-parser.dbg -lmycc-ast.dbg
$(BUILD_DIR)/parser.dbg.san: $(patsubst %.c,$(OBJECT_DIR)/%.dbg.san.obj,parser_main.c) $(BUILD_DIR)/libmycc-parser.dbg.san.so | $(BUILD_DIR)
	gcc $(CFLAGS_DBG_SAN) $(CFLAGS_LINK) -o $@ $(filter %.obj,$^) -L$(BUILD_DIR) -lmycc-parser.dbg.san -lmycc-ast.dbg.san

# We don't know the dependencies of these libraries here.
# They may need to be updated as targets, so these libraries are PHONY targets. If there's nothing to do,
# nothing is done anyways.
# We will always see linking of mycc-parser.so, because one of it's dependencies needs update as it's PHONY,
# PHONY targets always need to be updated. But we can't do anything with it. We could be including makefiles
# instead, but adding to CFLAGS and etc. is harder then. 
$(BUILD_DIR)/libmycc-ast.so $(BUILD_DIR)/libmycc-ast.dbg.so $(BUILD_DIR)/libmycc-ast.dbg.san.so: | $(BUILD_DIR)
	$(MAKE) -C ../ast $@
.PHONY: $(BUILD_DIR)/libmycc-ast.so $(BUILD_DIR)/libmycc-ast.dbg.so $(BUILD_DIR)/libmycc-ast.dbg.san.so

$(BUILD_DIR) $(OBJECT_DIR) $(OBJECT_DIR)/genrtd_src:
	mkdir -p $@

# Don't use embedded make rules.
# Makes it not run yacc instead of bison without my permission.
.SUFFIXES:

# # Don't delete intermidiate files if no error occurs. I sometimes want to see bison output.
# # I could request it though... That's what I do. This message is just as an option here for others and future.
# # https://stackoverflow.com/a/67780778
# .SECONDARY: