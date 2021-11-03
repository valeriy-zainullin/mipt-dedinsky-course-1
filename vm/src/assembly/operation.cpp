#include "assembly/assembly_impl.h"
#include "assembly/status.h"

bool vm_text_read_operation(VmStatus* status, VmForwardStream* input_stream, Operation* operation) {

	char command[VM_ASSEMBLY_MAX_COMMAND_LENGTH + 1];

	vm_read_formatted_text(input_stream, COMMAND_NAME_SCANF_FORMAT, command);

	int arg_start = 0;

	int num_read = sscanf((char*) line, COMMAND_NAME_SCANF_FORMAT "%n", command, &arg_start);
	if (num_read < 1) {
		return false;
	}

	uint8_t arg_type = 0;
	bool immediate_const_is_label = false;

	char register_name[2] = {};
	int32_t immediate_const = 0;
	char label[VM_ASSEMBLY_MAX_LABEL_LENGTH + 1] = {};

	bool read = false;

	if (!read) {
		return false;		
	}

	#define COMMAND(NAME, NUMBER, ALLOWED_ARG_TYPES)                    \
		if (strcmp(command, #NAME) == 0) {                              \
                                                                        \
			uint8_t command_number = NUMBER;                            \
			uint8_t allowed_arg_types[] = ALLOWED_ARG_TYPES;            \
                                                                        \
			return true;                                                \
                                                                        \
		} else
	#include "arch/commands.h"
	#undef COMMAND
	{
		status->error = VM_ASSEMBLY_ERROR_INVALID_COMMAND;
		return true;
	}
