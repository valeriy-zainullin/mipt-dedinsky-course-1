CFLAGS = -Wall -pedantic -Wextra -Wshadow -Wconversion -Warray-bounds -Wimplicit-fallthrough=5 -Werror -std=c11 -fPIC -I../ast/include -I../parser/include -I../libs/include
CFLAGS_REL = -O2 -DNDEBUG -DRELEASE_BUILD=1 -DDEBUG_BUILD=0
CFLAGS_DBG = -g -O0 -DRELEASE_BUILD=0 -DDEBUG_BUILD=1
CFLAGS_DBG_SAN = -fsanitize=address,undefined
