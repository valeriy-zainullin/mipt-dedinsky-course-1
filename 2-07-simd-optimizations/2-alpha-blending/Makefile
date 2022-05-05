# After view program is built, all dlls required are copied to this directory.
# To make it run without changing of PATH environment variable and changing of working directory.
# But these dlls may vary. Easies to set all mingw dlls to deps and if there's an update of mingw, rebuild the file.
ALL_MINGW_DLLS = $(wildcard /usr/x86_64-w64-mingw32/sys-root/mingw/bin/*.dll)

CFLAGS = -Wall -pedantic -Wextra -Wshadow -Werror -std=c99

all: present

present: view.exe benchmark.exe
	./benchmark
	./view

debug_bins: view.dbg.exe benchmark.dbg.exe

%.obj: %.c blend.h
	x86_64-w64-mingw32-gcc -c $(CFLAGS) -O2 -DNDEBUG -DBUILD=BUILD_RELEASE -o $@ $<

%.dbg.obj: %.c blend.h
	x86_64-w64-mingw32-gcc -c $(CFLAGS) -Og -g -DBUILD=BUILD_DEBUG -o $@ $<

blend_sse.obj: blend_sse.c blend.h
	x86_64-w64-mingw32-gcc -c -msse4.1 $(CFLAGS) -O2 -DNDEBUG -DBUILD=BUILD_RELEASE -o $@ $<

blend_sse.dbg.obj: blend_sse.c blend.h
	x86_64-w64-mingw32-gcc -c -msse4.1 $(CFLAGS) -Og -g -DBUILD=BUILD_DEBUG -o $@ $<

blend_avx.obj: blend_avx.c blend.h
	x86_64-w64-mingw32-gcc -c -mavx2 $(CFLAGS) -O2 -DNDEBUG -DBUILD=BUILD_RELEASE -o $@ $<

blend_avx.dbg.obj: blend_avx.c blend.h
	x86_64-w64-mingw32-gcc -c -mavx2 $(CFLAGS) -Og -g -DBUILD=BUILD_DEBUG -o $@ $<

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