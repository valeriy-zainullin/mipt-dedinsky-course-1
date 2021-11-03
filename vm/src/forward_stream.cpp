#include "forward_stream.h"

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

bool vm_read_bytes(VmForwardStream* stream, const uint8_t* bytes, size_t length) {
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

bool vm_write_formatted_text(VmForwardStream* stream, const char* format, ...) {
	assert(stream != nullptr);
	assert(format != nullptr);

	va_list args;
	va_start(args, format);

	// TODO: restriction on string length for .db.
	// You shouldn't write more than max positive of int.
	int num_chars_written = vsnprintf(stream->bytes, stream->length, format, args);

	if (num_chars_written < 0 || (size_t) num_chars_written > stream->length) {
		va_end(args);
		return false;
	}

	stream->length -= num_chars_written;
	stream->offset += num_chars_written;

	va_end(args);
	return true;
}

bool vm_read_formatted_text(VmForwardStream* stream, const char* format, ...) {
	assert(stream != nullptr);
	assert(format != nullptr);

	va_list args;
	va_start(args, format);

	int num_read = vsnscanf(stream->bytes, stream->length, format, args);

	if (num_read < 0) {
		va_end(args);
		return false;
	}

	stream->length -= num_chars_written;
	stream->offset += num_chars_written;

	va_end(args);
	return true;
}
