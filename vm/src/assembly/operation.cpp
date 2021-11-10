#include "assembly/operation.h"

#include "assembly/argument.h"
#include "status.h"
#include "support/forward_stream.h"

#include <assert.h>

bool vm_text_read_operation(VMStatus* status, VMForwardStream* input_stream, VMAssemblyOperation* operation) {
	assert(status != NULL);
	assert(input_stream != NULL);
	assert(operation != NULL);

	*status = VM_SUCCESS;

	char command[VM_ASSEMBLY_MAX_COMMAND_LENGTH + 1] = {};

	int arg_start = 0;
	int num_read = sscanf((char*) input_stream->bytes, VM_COMMAND_NAME_SCANF_FORMAT "%n", command, &arg_start);

	if (num_read < 0) {
		*status = VM_ERROR_WHILE_READING;
		return false;
	}

	if (num_read != 1) {
		*status = VM_ASSEMBLY_ERROR_INVALID_COMMAND;
		return false;
	}

	assert(arg_start >= 0);

	input_stream->bytes += arg_start;
	input_stream->offset += arg_start;
	input_stream->length -= arg_start;

	VMAssemblyArgument argument = {};

	if (!vm_text_read_arg(status, input_stream, &argument)) {
		return false;		
	}

	#define COMMAND(NAME, NUMBER, ALLOWED_ARG_TYPES,...)                                          \
		if (strcmp(command, #NAME) == 0) {                                                        \
			uint8_t allowed_arg_types[] = ALLOWED_ARG_TYPES;                                      \
																								  \
			bool found_arg_type = false;                                                          \
																								  \
			for (size_t i = 0; i < sizeof(allowed_arg_types) / sizeof(*allowed_arg_types); ++i) { \
				if (allowed_arg_types[i] == argument.arg_type) {                                  \
					found_arg_type = true;                                                        \
					break;                                                                        \
				}                                                                                 \
			}                                                                                     \
			                                                                                      \
			if (!found_arg_type) {                                                                \
				*status = VM_ASSEMBLY_ERROR_INVALID_ARGUMENT;                                     \
				return false;                                                                     \
			}                                                                                     \
			                                                                                      \
			strcpy(operation->command, command);                                                  \
			operation->argument = argument;                                                       \
			                                                                                      \
			return true;                                                                          \
			                                                                                      \
		} else
	#include "commands.h"
	#undef COMMAND
	{
		*status = VM_ASSEMBLY_ERROR_INVALID_COMMAND;
		return false;
	}
}

bool vm_text_write_operation(VMStatus* status, VMForwardStream* output_stream, const VMAssemblyOperation* operation) {
	assert(status != NULL);
	assert(output_stream != NULL);
	assert(operation != NULL);

	*status = VM_SUCCESS;

	if (!vm_write_bytes(output_stream, (const uint8_t*) operation->command, strlen(operation->command))) {
		*status = VM_ERROR_INSUFFICIENT_BUFFER;
		return false;
	}

	char tmp = ' ';
	if (!vm_write_bytes(output_stream, (const uint8_t*) &tmp, sizeof(tmp))) {
		return false;
	}

	if (!vm_text_write_arg(status, output_stream, &operation->argument)) {
		return false;
	}

	return true;
}
