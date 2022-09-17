BUILD_DIR=build
OBJECT_DIR=build/objects

all:
	$(MAKE) -C parser ../build/parser.dbg ../build/parser all BUILD_DIR=../$(BUILD_DIR) OBJECT_DIR=../$(OBJECT_DIR)
	
# -f for not showing errors if cannot delete a folder.
clean:
	rm -rf $(BUILD_DIR) $(OBJECT_DIR)

.PHONY: all clean