#include "bytecode/operation.h"

#include "arg_type.h"
#include "bytecode/impl_utils.h"
#include "status.h"

#include <stdio.h>

bool vm_bytecode_write_operation(VmStatus* status, VmForwardStream* output_stream, const VmOperation* operation) {
	uint8_t operation_code = (uint8_t) ((operation->arg_type << 5) | operation->command_index);
	WRITE(operation_code);

	if (operation->arg_type == VM_COMMAND_ARG_NOT_PRESENT) {
		return true;
	}

	if ((operation->arg_type & VM_COMMAND_ARG_USES_REGISTER) != 0) {
		WRITE(operation->register_index);
	}

	if ((operation->arg_type & VM_COMMAND_ARG_USES_IMMEDIATE_CONST) != 0) {
		WRITE(operation->immediate_const);
	}

	return true;
}

bool vm_bytecode_read_operation(VmStatus* status, VmForwardStream* input_stream, VmOperation* operation) {
	uint8_t operation_code = 0;
	READ(operation_code);

	operation->arg_type = (uint8_t) (operation_code >> 5);
	operation->command_index = (uint8_t) (operation_code & ((1 << 5) - 1));
	
	if (operation->arg_type == VM_COMMAND_ARG_NOT_PRESENT) {
		return true;
	}

	if ((operation->arg_type & VM_COMMAND_ARG_USES_REGISTER) != 0) {
		READ(operation->register_index);
	}

	if ((operation->arg_type & VM_COMMAND_ARG_USES_IMMEDIATE_CONST) != 0) {
		READ(operation->immediate_const);
	}

	return true;
}