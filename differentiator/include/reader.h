#pragma once

#include <stdio.h>

static const char READER_EOF = -1;

struct Reader {
	FILE* stream;
	char next_char;
	bool stop_at_lf;
};

void reader_init(Reader* reader, FILE* stream);
void reader_deinit(Reader* reader);

void reader_stop_at_lf(Reader* reader);

char reader_get_next_char(Reader* reader);

char reader_read(Reader* reader);

