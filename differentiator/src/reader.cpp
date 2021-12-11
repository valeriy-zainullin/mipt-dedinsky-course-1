#include "reader.h"

#include <assert.h>
#include <stdio.h>

void reader_init(Reader* reader, FILE* stream) {
	assert(reader != NULL);

	reader->stream = stream;
	reader->current_char = '\0';
	reader->next_char = '\0';
	
	reader_read(reader, stream);
	reader_read(reader, stream);
}

void reader_deinit(Reader* reader) {}

char reader_get_current_char(Reader* reader) {
	return reader->current_char;
}

char reader_get_next_char(Reader* reader) {
	return reader->next_char;
}

char reader_read(Reader* reader) {
	assert(reader != NULL);
	assert(reader->stream != NULL);
	
	if (reader->current_char == READER_EOF) {
		return READER_EOF;
	}
	
	reader->current_char = reader->next_char;
	reader->next_char = fgetc(reader->stream);
	
	if (ferror(stream) || feof(stream)) {
		reader->next_char = READER_EOF;
	}
	
	return reader->current_char;
}

