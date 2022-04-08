all: main_ready.exe main.exe

clean:
	rm *.exe *.obj *.lst || true

main.obj: %.obj : %.asm
	nasm $< -f win64 -g -W+all -l $<.lst

main.exe: %.exe : %.obj ../../2-05-printf/05printf.obj
	x86_64-w64-mingw32-ld --whole-archive -nostdlib -g -o $@ $^ $$(which kernel32.dll) $$(which bcrypt.dll) -L/usr/x86_64-w64-mingw32/sys-root/mingw/lib -e __start
	
main_ready.exe: main.exe
	x86_64-w64-mingw32-strip --strip-debug -o $@ $<
	
# Don't check file modification time for these and think that they always need rebuilding.
.PHONY: all clean