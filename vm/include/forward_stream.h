#pragma once

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

struct VmForwardStream {
	uint8_t* bytes;
	size_t offset;
	size_t length;
};

bool vm_write_bytes(VmForwardStream* stream, const uint8_t* bytes, size_t length);
bool vm_read_bytes(VmForwardStream* stream, const uint8_t* bytes, size_t length);

bool vm_write_formatted_text(VmForwardStream* stream, const char* format, ...);
bool vm_read_formatted_text(VmForwardStream* stream, const char* format, ...);