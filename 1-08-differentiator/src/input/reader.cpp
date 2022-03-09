#include "input/reader.h"

#include <assert.h>
#include <stdio.h>

void reader_init(Reader* reader, FILE* stream, bool stop_at_lf) {
	assert(reader != NULL);

	reader->stream = stream;
	reader->next_char = '\0';
	reader->stop_at_lf = stop_at_lf;
	
	reader_read(reader);
}

void reader_deinit(Reader* reader) {
	(void) reader;
}

char reader_get_next_char(Reader* reader) {
	return reader->next_char;
}

char reader_read(Reader* reader) {
	assert(reader != NULL);
	assert(reader->stream != NULL);
	
	if (reader->next_char == READER_EOF) {
		return READER_EOF;
	}
	
	char current_char = reader->next_char;
	reader->next_char = (char) fgetc(reader->stream);
	
	if (ferror(reader->stream) || feof(reader->stream)) {
		reader->next_char = READER_EOF;
	}
	
	if (reader->stop_at_lf && reader->next_char == '\n') {
		reader->next_char = READER_EOF;
	}
	
	return current_char;
}

