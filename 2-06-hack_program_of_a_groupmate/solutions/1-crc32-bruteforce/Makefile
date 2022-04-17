all: run

run: main.exe
	./main
	./nHack < input.txt

main.exe: main.c
	x86_64-w64-mingw32-gcc -Wall -pedantic -Wextra -Wshadow -Werror -march=native -O2 -std=c99 -o $@ $<

clean:
	rm main.exe
	
.PHONY: all run clean