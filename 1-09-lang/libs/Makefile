include ../flags.mk

CFLAGS += -Iinclude/mycc-libs

C_SOURCES = $(patsubst src/%.c,%.c,$(wildcard src/*.c))	

# TODO: make target specific directories inside build. Build for target upon a request, by default use host target.
# TODO: make gcc output makefiles.

all: $(BUILD_DIR)/libmycc-libs.so $(BUILD_DIR)/libmycc-libs.dbg.so $(BUILD_DIR)/libmycc-libs.dbg.san.so

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
	
$(BUILD_DIR)/libmycc-libs.so: $(patsubst %.c,$(OBJECT_DIR)/%.obj,$(C_SOURCES)) | $(BUILD_DIR)
	gcc -shared $(CFLAGS_LINK) -o $@ $(filter %.obj,$^)
$(BUILD_DIR)/libmycc-libs.dbg.so: $(patsubst %.c,$(OBJECT_DIR)/%.dbg.obj,$(C_SOURCES)) | $(BUILD_DIR)
	gcc -shared $(CFLAGS_LINK) -o $@ $(filter %.obj,$^)
$(BUILD_DIR)/libmycc-libs.dbg.san.so: $(patsubst %.c,$(OBJECT_DIR)/%.dbg.san.obj,$(C_SOURCES)) | $(BUILD_DIR)
	gcc -shared $(CFLAGS_LINK) -o $@ $(filter %.obj,$^)

$(BUILD_DIR) $(OBJECT_DIR):
	mkdir -p $@

# Don't use embedded make rules.
# Makes it not run yacc instead of bison without my permission.
.SUFFIXES:

# # Don't delete intermidiate files if no error occurs. I sometimes want to see bison output.
# # I could request it though... That's what I do. This message is just as an option here for others and future.
# # https://stackoverflow.com/a/67780778
# .SECONDARY: