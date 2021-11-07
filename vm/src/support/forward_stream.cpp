#include "support/forward_stream.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool vm_write_bytes(VmForwardStream* stream, const uint8_t* bytes, size_t length) {
	assert(stream != NULL);
	assert(bytes != NULL);

	if (stream->length < length) {
		return false;
	}

	memcpy(stream->bytes, bytes, length);
	stream->bytes += length;
	stream->length -= length;
	stream->offset += length;

	return true;
}

bool vm_read_bytes(VmForwardStream* stream, uint8_t* bytes, size_t length) {
	assert(stream != NULL);
	assert(bytes != NULL);

	if (stream->length < length) {
		return false;
	}

	memcpy(bytes, stream->bytes, length);
	stream->bytes += length;
	stream->length -= length;
	stream->offset += length;

	return true;
}

char vm_peek_char(VmForwardStream* stream) {
	if (stream->length == 0) {
		return VM_EOF;
	}

	return (char) *stream->bytes;
}

char vm_read_char(VmForwardStream* stream) {
	char result = vm_peek_char(stream);

	stream->bytes += 1;
	stream->length -= 1;
	stream->offset += 1;

	return result;
}
