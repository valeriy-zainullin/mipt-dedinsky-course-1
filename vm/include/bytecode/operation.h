#pragma once

struct VmOperation {
	uint8_t command_index;

	uint8_t arg_type;

	uint8_t register_index;
	int32_t argument;
};

bool vm_bytecode_write_operation(VmStatus* status, VmForwardStream* output_stream, const VmOperation* operation);