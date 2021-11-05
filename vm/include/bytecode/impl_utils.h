#pragma once

#include "status.h"

#define WRITE(VARIABLE)                                                                 \
	if (!vm_write_bytes(output_stream, (const uint8_t*) &VARIABLE, sizeof(VARIABLE))) { \
		*status = VM_ERROR_INSUFFICIENT_BUFFER;                                         \
		return false;                                                                   \
	}

#define READ(VARIABLE)                                                          \
	if (!vm_read_bytes(input_stream, (uint8_t*) &VARIABLE, sizeof(VARIABLE))) { \
		*status = VM_ERROR_INSUFFICIENT_BUFFER;                                 \
		return false;                                                           \
	}

