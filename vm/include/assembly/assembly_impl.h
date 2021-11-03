#pragma once

#include "assembly/status.h"
#include "forward_stream.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#define TEXT_WRITE_STRING(VALUE)                                \
	if (!vm_write_bytes(output_stream, VALUE, strlen(VALUE))) { \
		status->error = VM_ASSEMBLY_INSUFFICIENT_BUFFER;        \
		return false;                                           \
	}

#define ASSEMBLY_READ(VARIABLE) \
	if (!vm_read_bytes(output_stream, (const uint8_t*) &VARIABLE, sizeof(VARIABLE))) { \
		status->error = VM_ASSEMBLY_INSUFFICIENT_BUFFER;                               \
		return false;                                                                  \
	}

#define ASSEMBLY_WRITE_VARIABLE(VARIABLE)                                               \
	if (!vm_write_bytes(output_stream, (const uint8_t*) &VARIABLE, sizeof(VARIABLE))) { \
		status->error = VM_ASSEMBLY_INSUFFICIENT_BUFFER;                                \
		return false;                                                                   \
	}

/*
#if defined(__GNUC__)

// Always use bools here. But I do that always.
#define LIKELY(EXPR) __builtin_expect(EXPR, 1)
#define UNLIKELY(EXPR) __builtin_expect(EXPR, 0)

#else

#define LIKELY(EXPR) EXPR
#define UNLIKELY(EXPR) EXPR

#endif
*/
