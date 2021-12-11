#pragma once

#include <stdio.h>

static const char READER_EOF = -1;

struct Reader {
	FILE* stream;
	char current_char;
	char next_char;
};

void reader_init(Reader* reader, FILE* stream);
void reader_deinit(Reader* reader);

char reader_get_current_char(Reader* reader);
char reader_get_next_char(Reader* reader);

char reader_read(Reader* reader);

