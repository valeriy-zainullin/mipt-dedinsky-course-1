all: libkernel32.a

libkernel32.a: kernel32.c
	x86_64-w64-mingw32-gcc -s -shared -o kernel32.dll $< -Wl,--out-implib,libkernel32.a
	rm kernel32.dll

clean:
	rm -f *.a

.PHONY: all clean