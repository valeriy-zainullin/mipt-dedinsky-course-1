#pragma once

#include "status.h"
#include "support/forward_stream.h"

#include <stddef.h>
#include <stdint.h>

static const size_t VM_BYTECODE_MAX_OPERATION_LENGTH = sizeof(uint8_t) + sizeof(uint8_t) + sizeof(int32_t);

// VmBytecodeOperation?
struct VmOperation {
	uint8_t command_index;

	uint8_t arg_type;

	uint8_t register_index;
	int32_t immediate_const;
};

bool vm_bytecode_write_operation(VmStatus* status, VmForwardStream* output_stream, const VmOperation* operation);
bool vm_bytecode_read_operation(VmStatus* status, VmForwardStream* input_stream, VmOperation* operation);
