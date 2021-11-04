#include "support/forward_stream.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool vm_write_bytes(VmForwardStream* stream, const uint8_t* bytes, size_t length) {
	assert(stream != nullptr);
	assert(bytes != nullptr);

	if (stream->length < length) {
		return false;
	}

	memcpy(stream->bytes, bytes, length);
	stream->length -= length;
	stream->offset += length;

	return true;
}

bool vm_read_bytes(VmForwardStream* stream, uint8_t* bytes, size_t length) {
	assert(stream != nullptr);
	assert(bytes != nullptr);

	if (stream->length < length) {
		return false;
	}

	memcpy(bytes, stream->bytes, length);
	stream->length -= length;
	stream->offset += length;

	return true;
}
