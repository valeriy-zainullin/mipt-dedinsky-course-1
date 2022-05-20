# After view program is built, all dlls required are copied to this directory.
# To make it run without changing of PATH environment variable and changing of working directory.
# But these dlls may vary. Easies to set all mingw dlls to deps and if there's an update of mingw, rebuild the file.
ALL_MINGW_DLLS = $(wildcard /usr/x86_64-w64-mingw32/sys-root/mingw/bin/*.dll)

CFLAGS = -Wall -pedantic -Wextra -Wshadow -Werror -std=c11
CFLAGS_REL = -O2 -DNDEBUG -DRELEASE_BUILD=1 -DDEBUG_BUILD=0
CFLAGS_DBG = -g -O0 -DRELEASE_BUILD=0 -DDEBUG_BUILD=1

all: present

present: view.exe benchmark.exe
	./benchmark
	./view

debug_bins: view.dbg.exe benchmark.dbg.exe

# If one debugs simd optimizations in gdb, he would see "Optimized out" a
# couple of times. I would like to see those variables although I saw the
# problem is later in the code. -Og makes simd variables be optimized.
# For debugging, here -O0 is better. I'll just use it everywhere, as I
# expect it to happen with normal variables as well. And debug binaries
# are still not for speed tests and are not a release build.

%.obj: %.c blend.h message_box.h
	x86_64-w64-mingw32-gcc -c $(CFLAGS) $(CFLAGS_REL) -o $@ $<

%.dbg.obj: %.c blend.h message_box.h
	x86_64-w64-mingw32-gcc -c $(CFLAGS) $(CFLAGS_DBG) -o $@ $<

blend_sse.obj: blend_sse.c blend.h
	x86_64-w64-mingw32-gcc -c -msse4.1 $(CFLAGS) $(CFLAGS_REL) -o $@ $<

blend_sse.dbg.obj: blend_sse.c blend.h
	x86_64-w64-mingw32-gcc -c -msse4.1 $(CFLAGS) $(CFLAGS_DBG) -o $@ $<

blend_avx.obj: blend_avx.c blend.h
	x86_64-w64-mingw32-gcc -c -mavx2 $(CFLAGS) $(CFLAGS_REL) -o $@ $<

blend_avx.dbg.obj: blend_avx.c blend.h
	x86_64-w64-mingw32-gcc -c -mavx2 $(CFLAGS) $(CFLAGS_DBG) -o $@ $<

resource.res: resource.rc
	x86_64-w64-mingw32-windres -O coff -o $@ $<


view.exe: view_main.obj blend.obj blend_nosse.obj blend_sse.obj blend_avx.obj resource.res $(ALL_MINGW_DLLS)
	x86_64-w64-mingw32-gcc -static -o $@ $(filter %.obj %.res,$^) -lSDL2 -lSDL2_ttf -lfreetype.dll -lwinmm -liconv -limm32 -lole32 -lversion -loleaut32 -lgdi32 -mwindows
	./copy_dlls.sh $@

view.dbg.exe: view_main.dbg.obj blend.dbg.obj blend_nosse.dbg.obj blend_sse.dbg.obj blend_avx.dbg.obj resource.res $(ALL_MINGW_DLLS)
	x86_64-w64-mingw32-gcc -static -o $@ $(filter %.obj %.res,$^) -lSDL2 -lSDL2_ttf -lfreetype.dll -lwinmm -liconv -limm32 -lole32 -lversion -loleaut32 -lgdi32
	./copy_dlls.sh $@

benchmark.exe: benchmark_main.obj blend.obj blend_nosse.obj blend_sse.obj blend_avx.obj
	x86_64-w64-mingw32-gcc -static -o $@ $^ -lSDL2 -lwinmm -liconv -limm32 -lole32 -lversion -loleaut32 -lgdi32

benchmark.dbg.exe: benchmark_main.dbg.obj blend.dbg.obj blend_nosse.dbg.obj blend_sse.dbg.obj blend_avx.dbg.obj
	x86_64-w64-mingw32-gcc -static -o $@ $^ -lSDL2 -lwinmm -liconv -limm32 -lole32 -lversion -loleaut32 -lgdi32

clean:
	rm -f *.obj *.exe *.dll

.PHONY: present debug_bins clean