#include "assembly/operation.h"

#include "assembly/argument.h"
#include "status.h"
#include "support/forward_stream.h"

#include <assert.h>

bool vm_text_read_operation(VmStatus* status, VmForwardStream* input_stream, VmAssemblyOperation* operation) {
	assert(status != nullptr);
	assert(input_stream != nullptr);
	assert(operation != nullptr);

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

	VmAssemblyArgument argument = {};

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
					return true;                                                                  \
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
