#include "support/forward_stream.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool vm_write_bytes(VMForwardStream* stream, const uint8_t* bytes, size_t length) {
	assert(stream != NULL);
	assert(bytes != NULL);

	if (stream->length < length) {
		return false;
	}

	memcpy(stream->bytes, bytes, length);

	vm_advance_stream(stream, length);

	return true;
}

bool vm_read_bytes(VMForwardStream* stream, uint8_t* bytes, size_t length) {
	assert(stream != NULL);
	assert(bytes != NULL);

	if (stream->length < length) {
		return false;
	}

	memcpy(bytes, stream->bytes, length);

	vm_advance_stream(stream, length);

	return true;
}

char vm_peek_char(VMForwardStream* stream) {
	assert(stream != NULL);

	if (stream->length == 0) {
		return VM_EOF;
	}

	return (char) *stream->bytes;
}

char vm_read_char(VMForwardStream* stream) {
	assert(stream != NULL);

	char result = vm_peek_char(stream);

	vm_advance_stream(stream, 1);

	return result;
}

void vm_skip_repeats(VMForwardStream* stream, char character) {
	assert(stream != NULL);
	assert(character != VM_EOF);

	while (vm_peek_char(stream) == character) {
		vm_read_char(stream);
	}
}
