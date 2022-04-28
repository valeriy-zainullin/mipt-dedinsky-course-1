# After view program is built, all dlls required are copied to this directory.
# To make it run without changing of PATH environment variable and changing of working directory.
# But these dlls may vary. Easies to set all mingw dlls to deps and if there's an update of mingw, rebuild the file.
ALL_MINGW_DLLS = $(wildcard /usr/x86_64-w64-mingw32/sys-root/mingw/bin/*.dll)

all: present

present: view.exe benchmark.exe
	./benchmark
	./view

debug_bins: view.dbg.exe benchmark.dbg.exe

%.obj: %.c compute.h
	x86_64-w64-mingw32-gcc -c -Wall -pedantic -Wextra -Wshadow -Werror -O2 -DNDEBUG -DBUILD=BUILD_RELEASE -std=c99 -o $@ $<

%.dbg.obj: %.c compute.h
	x86_64-w64-mingw32-gcc -c -Wall -pedantic -Wextra -Wshadow -Werror -Og -g -DBUILD=BUILD_DEBUG -std=c99 -o $@ $<

compute_avx.obj: compute_avx.c compute.h
	x86_64-w64-mingw32-gcc -c -mavx -Wall -pedantic -Wextra -Wshadow -Werror -O2 -DNDEBUG -DBUILD=BUILD_RELEASE -std=c99 -o $@ $<

compute_avx.dbg.obj: compute_avx.c compute.h
	x86_64-w64-mingw32-gcc -c -mavx -Wall -pedantic -Wextra -Wshadow -Werror -Og -g -DBUILD=BUILD_DEBUG -std=c99 -o $@ $<
	
resource.res: resource.rc
	x86_64-w64-mingw32-windres -O coff -o $@ $<


view.exe: view_main.obj compute_nosse.obj compute_sse.obj compute_avx.obj resource.res $(ALL_MINGW_DLLS)
	x86_64-w64-mingw32-gcc -static -o $@ $^ -lSDL2 -lSDL2_ttf -lfreetype.dll -lwinmm -liconv -limm32 -lole32 -lversion -loleaut32 -lgdi32 -mwindows
	./copy_dlls.sh $@

view.dbg.exe: view_main.dbg.obj compute_nosse.dbg.obj compute_sse.dbg.obj compute_avx.dbg.obj resource.res $(ALL_MINGW_DLLS)
	x86_64-w64-mingw32-gcc -static -o $@ $^ -lSDL2 -lSDL2_ttf -lfreetype.dll -lwinmm -liconv -limm32 -lole32 -lversion -loleaut32 -lgdi32
	./copy_dlls.sh $@

benchmark.exe: benchmark_main.obj compute_nosse.obj compute_sse.obj compute_avx.obj
	x86_64-w64-mingw32-gcc -static -o $@ $^

benchmark.dbg.exe: benchmark_main.dbg.obj compute_nosse.dbg.obj compute_sse.dbg.obj compute_avx.dbg.obj
	x86_64-w64-mingw32-gcc -static -o $@ $^

clean:
	rm -f *.obj *.exe *.dll

.PHONY: present debug_bins clean