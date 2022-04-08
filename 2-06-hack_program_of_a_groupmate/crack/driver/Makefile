CFLAGS = \
	-Wall -pedantic -Wextra -Wshadow -Werror -O2 -std=c99 \
	-fno-exceptions -I/usr/x86_64-w64-mingw32/sys-root/mingw/include/ddk
LDFLAGS = \
	--subsystem native --image-base 0x10000 --section-alignment 0x1000 \
	--entry DriverEntry --stack 0x40000 --dynamicbase --nxcompat \
	-nostdlib -L/usr/x86_64-w64-mingw32/sys-root/mingw/lib \
	-lntoskrnl -lhal

all: driver.sys

driver.o: driver.c
	x86_64-w64-mingw32-gcc -c $(CFLAGS) -o $@ $<

driver.sys: driver.o
	x86_64-w64-mingw32-ld --whole-archive $(LDFLAGS) -o $@ $<

clean:
	rm driver.sys driver.o
	
.PHONY: all clean