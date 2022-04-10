// Workaround for x86_64-w64-mingw32-ld linking one of it's crt*.o object files when one is not needed at all.
/*
$ make
# First idea.
# x86_64-w64-mingw32-ld --whole-archive -g -o main.exe main.obj ../../2-05-printf/05printf.obj SHA512.dll $(which kernel32.dll) $(which bcrypt.dll) -Lmy_kernel32.dll -lkernel32 -e __start
# Bcrypt is used from C.
# ./build_test2.bat
# x86_64-w64-mingw32-ld --whole-archive -g -o main.exe main.obj ../../2-05-printf/05printf.obj SHA512.dll test2.obj $(which kernel32.dll) $(which bcrypt.dll) -Lmy_kernel32.dll -lkernel32 -e __start
# SHA512 from github.
x86_64-w64-mingw32-ld --whole-archive -g -o main.exe main.obj ../../2-05-printf/05printf.obj SHA512.dll $(which kernel32.dll) $(which bcrypt.dll) -Lmy_kernel32.dll -lkernel32 -e __start
x86_64-w64-mingw32-ld: ertr000027.o:(.rdata+0x0): неопределённая ссылка на «_pei386_runtime_relocator»
make: *** [Makefile:22: main.exe] Ошибка 1
*/
// Just implement such a function. Hope it is not called. And it shouldn't be as we control the entrypoint, C code compiled with MSVC seems (judging by disassembly) to not use it.
// And it works, which means something.
// It happens second time. Before it was happening for HeapAlloc and HeapFree. I just wrote my kernel32 import library in my_kernel32.dll.

void _pei386_runtime_relocator() {
}