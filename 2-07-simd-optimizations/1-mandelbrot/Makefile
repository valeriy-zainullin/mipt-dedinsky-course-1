all: present

present: view.exe # benchmark.exe
	bash -c "export PATH=\"$$PATH:/usr/x86_64-w64-mingw32/sys-root/mingw/bin\"; ./view"

debug_bins: view.dbg.exe # benchmark.dbg.exe

%.obj: %.c compute.h
	x86_64-w64-mingw32-gcc -c -Wall -pedantic -Wextra -Wshadow -Werror -O2 -std=c99 -o $@ $<

%.dbg.obj: %.c compute.h
	x86_64-w64-mingw32-gcc -c -Wall -pedantic -Wextra -Wshadow -Werror -Og -g -std=c99 -o $@ $<

view.exe: view_main.obj compute_nosse.obj compute_sse.obj resource.res
	x86_64-w64-mingw32-gcc -o $@ $^ -lSDL2 -mwindows

benchmark.exe: benchmark_main.obj
	x86_64-w64-mingw32-gcc -o $@ $^

view.dbg.exe: view_main.dbg.obj compute_nosse.dbg.obj compute_sse.dbg.obj resource.res
	x86_64-w64-mingw32-gcc -o $@ $^ -lSDL2

benchmark.dbg.exe: benchmark_main.obj
	x86_64-w64-mingw32-gcc -o $@ $^
	
resource.res: resource.rc
	x86_64-w64-mingw32-windres -O coff -o $@ $<

	
clean:
	rm -f *.obj *.exe

.PHONY: present deebug_bins clean