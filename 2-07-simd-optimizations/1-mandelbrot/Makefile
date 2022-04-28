all: present

present: view.exe benchmark.exe
	./benchmark
	bash -c "export PATH=\"$$PATH:/usr/x86_64-w64-mingw32/sys-root/mingw/bin\"; ./view"

debug_bins: view.dbg.exe benchmark.dbg.exe

%.obj: %.c compute.h
	x86_64-w64-mingw32-gcc -c -Wall -pedantic -Wextra -Wshadow -Werror -O2 -DNDEBUG -std=c99 -o $@ $<

%.dbg.obj: %.c compute.h
	x86_64-w64-mingw32-gcc -c -Wall -pedantic -Wextra -Wshadow -Werror -Og -g -std=c99 -o $@ $<

compute_avx.obj: compute_avx.c compute.h
	x86_64-w64-mingw32-gcc -c -mavx -Wall -pedantic -Wextra -Wshadow -Werror -O2 -DNDEBUG -std=c99 -o $@ $<

compute_avx.dbg.obj: compute_avx.c compute.h
	x86_64-w64-mingw32-gcc -c -mavx -Wall -pedantic -Wextra -Wshadow -Werror -Og -g -std=c99 -o $@ $<
	
resource.res: resource.rc
	x86_64-w64-mingw32-windres -O coff -o $@ $<

view.exe: view_main.obj compute_nosse.obj compute_sse.obj compute_avx.obj resource.res
	x86_64-w64-mingw32-gcc -static -o $@ $^ -lSDL2 -lSDL2_ttf -mwindows

view.dbg.exe: view_main.dbg.obj compute_nosse.dbg.obj compute_sse.dbg.obj compute_avx.dbg.obj resource.res
	x86_64-w64-mingw32-gcc -static -o $@ $^ -lSDL2 -lSDL2_ttf

benchmark.exe: benchmark_main.obj compute_nosse.obj compute_sse.obj compute_avx.obj
	x86_64-w64-mingw32-gcc -static -o $@ $^

benchmark.dbg.exe: benchmark_main.dbg.obj compute_nosse.dbg.obj compute_sse.dbg.obj compute_avx.dbg.obj
	x86_64-w64-mingw32-gcc -static -o $@ $^

clean:
	rm -f *.obj *.exe

.PHONY: present debug_bins clean