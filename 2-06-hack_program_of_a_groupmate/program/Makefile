all: main_ready.exe main.exe

clean:
	rm *.exe *.obj *.lst || true

main.obj: %.obj : %.asm
	nasm $< -f win64 -g -W+all -l $<.lst

SHA512.dll: LeFroid_sha256-512-master/SHA512.dll
	cp $< $@

test2.obj: test2.c
	x86_64-w64-mingw32-gcc -c -Wall -pedantic -Wextra -Wshadow -Werror -g -std=c99 -o $@ $<

main.exe: %.exe : %.obj ../../2-05-printf/05printf.obj SHA512.dll # test-bcrypt-csample.obj
	# First idea.
	# x86_64-w64-mingw32-ld --whole-archive -g -o $@ $^ $$(which kernel32.dll) $$(which bcrypt.dll) -Lmy_kernel32.dll -lkernel32 -e __start
	# Bcrypt is used from C.
	# ./build_test2.bat
	# x86_64-w64-mingw32-ld --whole-archive -g -o $@ $^ test2.obj $$(which kernel32.dll) $$(which bcrypt.dll) -Lmy_kernel32.dll -lkernel32 -e __start
	# SHA512 from github.
	x86_64-w64-mingw32-ld --whole-archive -g -o $@ $^ $$(which kernel32.dll) $$(which bcrypt.dll) -Lmy_kernel32.dll -lkernel32 -e __start
	
main_ready.exe: main.exe
	x86_64-w64-mingw32-strip --strip-debug -o $@ $<
	
# Don't check file modification time for these and think that they always need rebuilding.
.PHONY: all clean
