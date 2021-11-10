#pragma once

#include "support/macro_utils.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

struct VMForwardStream {
	uint8_t* bytes;
	size_t length;
	size_t offset;
};

MAY_BE_UNUSED static const char VM_EOF = -1;

inline void vm_advance_stream(VMForwardStream* stream, size_t nbytes) {
	assert(stream->bytes != NULL);
	assert(nbytes <= stream->length);

	stream->bytes += nbytes;
	stream->offset += nbytes;
	stream->length -= nbytes;
}

inline void vm_init_stream(VMForwardStream* stream, uint8_t* bytes, size_t length, size_t offset = 0) {
	assert(stream != NULL);
	assert(bytes != NULL);

	stream->bytes = bytes;
	stream->length = length;
	stream->offset = offset;
}

inline void vm_deinit_stream(VMForwardStream* stream) {
	assert(stream != NULL);

	(void) stream;
}

// void ptr?
bool vm_write_bytes(VMForwardStream* stream, const uint8_t* bytes, size_t length);
bool vm_read_bytes(VMForwardStream* stream, uint8_t* bytes, size_t length);

char vm_peek_char(VMForwardStream* stream);
char vm_read_char(VMForwardStream* stream);

void vm_skip_repeats(VMForwardStream* stream, char character);
