#pragma once

#include "support/macro_utils.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

struct VMForwardStream {
	uint8_t* bytes;
	size_t offset;
	size_t length;
};

MAY_BE_UNUSED static const char VM_EOF = -1;

// void ptr?
bool vm_write_bytes(VMForwardStream* stream, const uint8_t* bytes, size_t length);
bool vm_read_bytes(VMForwardStream* stream, uint8_t* bytes, size_t length);

char vm_peek_char(VMForwardStream* stream);
char vm_read_char(VMForwardStream* stream);
