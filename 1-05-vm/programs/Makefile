SOURCES = $(wildcard src/*.as)
BINARIES = $(SOURCES:src/%.as=bin/%.bin)

all: $(BINARIES)

bin/%.bin: src/%.as | bin
	../build/asan/as $< $@

bin:
	mkdir bin

clean:
	rm -r bin

.PHONY: all clean
