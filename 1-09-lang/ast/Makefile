include ../flags.mk

CFLAGS += -Iinclude/mycc-ast

C_SOURCES = $(patsubst src/%.c,%.c,$(wildcard src/*.c))	

# TODO: make target specific directories inside build. Build for target upon a request, by default use host target.
# TODO: make gcc output makefiles.

all: $(BUILD_DIR)/libmycc-ast.so $(BUILD_DIR)/libmycc-ast.dbg.so $(BUILD_DIR)/libmycc-ast.dbg.san.so

# -f for not showing errors if cannot delete a folder.
clean:
	rm -rf $(BUILD_DIR) $(OBJECT_DIR)

.PHONY: all clean

$(OBJECT_DIR)/%.obj: src/%.c | $(OBJECT_DIR)
	gcc -c $(CFLAGS) $(CFLAGS_REL) -o $@ $<

$(OBJECT_DIR)/%.dbg.obj: src/%.c | $(OBJECT_DIR)
	gcc -c $(CFLAGS) $(CFLAGS_DBG) -o $@ $<
	
$(OBJECT_DIR)/%.dbg.san.obj: src/%.c | $(OBJECT_DIR)
	gcc -c $(CFLAGS) $(CFLAGS_DBG) $(CFLAGS_DBG_SAN) -o $@ $<
	
$(BUILD_DIR)/libmycc-ast.so: $(patsubst %.c,$(OBJECT_DIR)/%.obj,$(C_SOURCES)) $(BUILD_DIR)/libmycc-libs.so | $(BUILD_DIR)
	gcc -shared $(CFLAGS_LINK) -o $@ $(filter %.obj,$^) -L$(BUILD_DIR) -lmycc-libs
$(BUILD_DIR)/libmycc-ast.dbg.so: $(patsubst %.c,$(OBJECT_DIR)/%.dbg.obj,$(C_SOURCES)) $(BUILD_DIR)/libmycc-libs.dbg.so | $(BUILD_DIR)
	gcc -shared $(CFLAGS_LINK) -o $@ $(filter %.obj,$^) -L$(BUILD_DIR) -lmycc-libs.dbg
$(BUILD_DIR)/libmycc-ast.dbg.san.so: $(patsubst %.c,$(OBJECT_DIR)/%.dbg.san.obj,$(C_SOURCES)) $(BUILD_DIR)/libmycc-libs.dbg.san.so | $(BUILD_DIR)
	gcc -shared $(CFLAGS_LINK) -o $@ $(filter %.obj,$^) -L$(BUILD_DIR) -lmycc-libs.dbg.san

# We don't know the dependencies of these libraries here.
# They may need to be updated as targets, so these libraries are PHONY targets. If there's nothing to do,
# nothing is done anyways.
# We will always see linking of mycc-parser.so, because one of it's dependencies needs update as it's PHONY,
# PHONY targets always need to be updated. But we can't do anything with it. We could be including makefiles
# instead, but adding to CFLAGS and etc. is harder then. 
$(BUILD_DIR)/libmycc-libs.so $(BUILD_DIR)/libmycc-libs.dbg.so $(BUILD_DIR)/libmycc-libs.dbg.san.so: | $(BUILD_DIR)
	$(MAKE) -C ../libs $@
.PHONY: $(BUILD_DIR)/libmycc-libs.so $(BUILD_DIR)/libmycc-libs.dbg.so $(BUILD_DIR)/libmycc-libs.dbg.san.so

$(BUILD_DIR) $(OBJECT_DIR):
	mkdir -p $@

# Don't use embedded make rules.
# Makes it not run yacc instead of bison without my permission.
.SUFFIXES:

# # Don't delete intermidiate files if no error occurs. I sometimes want to see bison output.
# # I could request it though... That's what I do. This message is just as an option here for others and future.
# # https://stackoverflow.com/a/67780778
# .SECONDARY: