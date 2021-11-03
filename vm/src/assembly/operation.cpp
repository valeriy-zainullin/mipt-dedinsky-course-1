#include "assembly/argument.h"
#include "assembly/assembly_impl.h"
#include "assembly/status.h"

bool vm_text_read_operation(VmStatus* status, VmForwardStream* input_stream, Operation* operation) {
	assert(status != nullptr):
	assert(input_stream != nullptr);
	assert(operation != nullptr);

	char command[VM_ASSEMBLY_MAX_COMMAND_LENGTH + 1];

	int arg_start = 0;
	int num_read = sscanf((char*) input_stream->bytes, COMMAND_NAME_SCANF_FORMAT "%n", command);

	if (num_read < 0) {
		status->error = VM_ASSEMBLY_ERROR_WHILE_READING;
		return false;
	}

	if (num_read != 1) {
		status->error = VM_ASSEMBLY_ERROR_INVALID_COMMAND;
		return false;
	}

	VmAssemblyArgument argument = {};

	if (!vm_text_read_arg(status, input_stream, &argument)) {
		return false;		
	}

	#define COMMAND(NAME, NUMBER, ALLOWED_ARG_TYPES)                                              \
		if (strcmp(command, #NAME) == 0) {                                                        \
			uint8_t command_number = NUMBER;                                                      \
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
				return false;                                                                     \
			}                                                                                     \
			                                                                                      \
			operation->command = command;                                                         \
			operation->argument = argument;                                                       \
			                                                                                      \
			return true;                                                                          \
			                                                                                      \
		} else
	#include "arch/commands.h"
	#undef COMMAND
	{
		status->error = VM_ASSEMBLY_ERROR_INVALID_COMMAND;
		return false;
	}
}
