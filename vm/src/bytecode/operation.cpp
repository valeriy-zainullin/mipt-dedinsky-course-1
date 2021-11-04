#include "bytecode/operation.h"

#include "arg_type.h"
#include "bytecode/impl_utils.h"
#include "status.h"

bool vm_bytecode_write_operation(VmStatus* status, VmForwardStream* output_stream, const VmOperation* operation) {
	uint8_t operation_code = (uint8_t) ((operation->arg_type << 5) | operation->command_index);
	WRITE(operation_code);

	if ((operation->arg_type & VM_COMMAND_ARG_USES_REGISTER) != 0) {
		WRITE(operation->register_index);
	}

	if ((operation->arg_type & VM_COMMAND_ARG_USES_IMMEDIATE_CONST) != 0) {
		WRITE(operation->immediate_const);
	}

	return true;
}