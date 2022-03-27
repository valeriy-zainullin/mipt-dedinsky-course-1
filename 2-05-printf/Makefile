all: msvcrt_printf_from_asm.exe myprintf_from_c.exe tests

clean:
	rm *.exe *.obj *.lst || true

%.obj : %.asm
	nasm $< -f win64 -g -W+all -l $<.lst

msvcrt_printf_from_asm.exe: %.exe : %.obj
	x86_64-w64-mingw32-ld -whole-archive -g -o $@ $^ $$(which msvcrt.dll) $$(which kernel32.dll) -L/usr/x86_64-w64-mingw32/sys-root/mingw/lib -e __start

myprintf_from_c.exe: %.exe : %.c 05printf.obj
	x86_64-w64-mingw32-gcc -g -o $@ $^
	
tests: myprintf_from_c.exe msvcrt_printf_from_asm.exe
	@echo "=== TESTS ==="
	@echo "-- msvcrt_printf --"
	@./msvcrt_printf_from_asm.exe
	@echo "-- myprintf_from_c --"
	@./myprintf_from_c.exe

# Don't check file modification time for these and think that they always need rebuilding.
.PHONY: all clean tests