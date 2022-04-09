all: SHA512.dll

SHA512.dll: SHA512.c
	x86_64-w64-mingw32-gcc -shared -nostdlib -nostartfiles -o $@ $< -lkernel32
	# $$(which kernel32.dll)

clean:
	rm SHA512.dll
	
.PHONY: all clean