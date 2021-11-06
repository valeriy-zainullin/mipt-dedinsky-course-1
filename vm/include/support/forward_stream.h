#pragma once

#include "support/macro_utils.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

struct VmForwardStream {
	uint8_t* bytes;
	size_t offset;
	size_t length;
};

MAY_BE_UNUSED static const char VM_EOF = -1;

bool vm_write_bytes(VmForwardStream* stream, const uint8_t* bytes, size_t length);
bool vm_read_bytes(VmForwardStream* stream, uint8_t* bytes, size_t length);

char vm_peek_char(VmForwardStream* stream);
char vm_read_char(VmForwardStream* stream);
