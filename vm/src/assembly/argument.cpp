#include "assembly/argument.h"

#include "arg_type.h"
#include "assembly/label.h"
#include "assembly/command.h"

#include <ctype.h>
#include <stdio.h>

// TODO: hex numbers.
bool vm_text_read_arg(
	VMStatus* status,
	VMForwardStream* input_stream,

	VMAssemblyArgument* argument
) {
	assert(status != NULL);
	assert(input_stream != NULL);
	assert(argument != NULL);

	// *status = VM_SUCCESS;

	int num_characters_read = 0;
	char register_name[2] = {};

	bool read = false;
	#define TRY_READ_ARG(FORMAT, ARG_TYPE, ...)                                                  \
		num_characters_read = 0;                                                                 \
		if (!read) {                                                                             \
			sscanf((char*) input_stream->bytes, FORMAT "%n", __VA_ARGS__, &num_characters_read); \
		}                                                                                        \
		if (num_characters_read != 0) {                                                          \
			argument->arg_type = ARG_TYPE;                                                       \
			/* Only means that following formats won't be tried. */                              \
			read = true;                                                                         \
		}                                                                                        \
		if (num_characters_read != 0)

	// ---- Memory arguments. ----

	TRY_READ_ARG(
		" [" VM_REGISTER_NAME_SCANF_FORMAT "+" VM_IMMEDIATE_CONST_SCANF_FORMAT "]",
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_REGISTER | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		register_name,
		&argument->immediate_const.value
	) {
		argument->register_index = (uint8_t) (register_name[0] - 'a');
		argument->immediate_const.is_label = false;
	}
	
	TRY_READ_ARG(
		" [" VM_IMMEDIATE_CONST_SCANF_FORMAT "+" VM_REGISTER_NAME_SCANF_FORMAT "]",
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_REGISTER | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		&argument->immediate_const.value,
		register_name
	) {
		argument->register_index = (uint8_t) (register_name[0] - 'a');
		argument->immediate_const.is_label = false;
	}

	TRY_READ_ARG(
		" [" VM_REGISTER_NAME_SCANF_FORMAT "]",
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_REGISTER,
		register_name
	) {
		argument->register_index = (uint8_t) (register_name[0] - 'a');
	}

	TRY_READ_ARG(
		" [" VM_IMMEDIATE_CONST_SCANF_FORMAT "]",
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		&argument->immediate_const.value
	) {
		argument->immediate_const.is_label = false;
	}

	// ---- ----
	
	// ---- Non-memory arguments. ----
	
	TRY_READ_ARG(
		VM_REGISTER_NAME_SCANF_FORMAT "+" VM_IMMEDIATE_CONST_SCANF_FORMAT,
		VM_COMMAND_ARG_USES_REGISTER | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		register_name,
		&argument->immediate_const.value
	) {
		argument->register_index = (uint8_t) (register_name[0] - 'a');
		argument->immediate_const.is_label = false;
	}
	
	TRY_READ_ARG(
		VM_IMMEDIATE_CONST_SCANF_FORMAT "+" VM_REGISTER_NAME_SCANF_FORMAT,
		VM_COMMAND_ARG_USES_REGISTER | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		&argument->immediate_const.value,
		register_name
	) {
		argument->register_index = (uint8_t) (register_name[0] - 'a');
		argument->immediate_const.is_label = false;
	}

	TRY_READ_ARG(
		VM_REGISTER_NAME_SCANF_FORMAT,
		VM_COMMAND_ARG_USES_REGISTER,
		register_name
	) {
		argument->register_index = (uint8_t) (register_name[0] - 'a');
	}

	TRY_READ_ARG(
		VM_IMMEDIATE_CONST_SCANF_FORMAT,
		VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		&argument->immediate_const.value
	) {
		argument->immediate_const.is_label = false;		
	}

	// ---- ----

	// ---- Label arguments. ----

	TRY_READ_ARG(
		" [" VM_REGISTER_NAME_SCANF_FORMAT "+" VM_ASSEMBLY_LABEL_NAME_SCANF_FORMAT "]",
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_REGISTER | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		register_name,
		argument->immediate_const.label
	) {
		argument->register_index = (uint8_t) (register_name[0] - 'a');
		argument->immediate_const.is_label = true;
	}

	// TODO: same format for format string names.
	TRY_READ_ARG(
		" [" VM_ASSEMBLY_LABEL_NAME_SCANF_FORMAT "+" VM_REGISTER_NAME_SCANF_FORMAT "]",
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_REGISTER | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		argument->immediate_const.label,
		register_name
	) {
		argument->register_index = (uint8_t) (register_name[0] - 'a');
		argument->immediate_const.is_label = true;
	}

	TRY_READ_ARG(
		" [" VM_ASSEMBLY_LABEL_NAME_SCANF_FORMAT "]",
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		argument->immediate_const.label
	) {
		argument->immediate_const.is_label = true;
	}
	
	TRY_READ_ARG(
		VM_REGISTER_NAME_SCANF_FORMAT "+" VM_ASSEMBLY_LABEL_NAME_SCANF_FORMAT,
		VM_COMMAND_ARG_USES_REGISTER | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		register_name,
		argument->immediate_const.label
	) {
		argument->register_index = (uint8_t) (register_name[0] - 'a');
		argument->immediate_const.is_label = true;
	}

	TRY_READ_ARG(
		VM_ASSEMBLY_LABEL_NAME_SCANF_FORMAT "+" VM_REGISTER_NAME_SCANF_FORMAT,
		VM_COMMAND_ARG_USES_REGISTER | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		argument->immediate_const.label,
		register_name
	) {
		argument->register_index = (uint8_t) (register_name[0] - 'a');
		argument->immediate_const.is_label = true;
	}
	
	TRY_READ_ARG(
		VM_ASSEMBLY_LABEL_NAME_SCANF_FORMAT,
		VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		argument->immediate_const.label
	) {
		argument->immediate_const.is_label = true;
	}

	if (!read) {
		while (input_stream->length > 0 && isspace(*(char*) input_stream->bytes)) {
			input_stream->bytes += 1;
			input_stream->offset += 1;
			input_stream->length -= 1;
		}
		if (input_stream->length == 0) {
			argument->arg_type = VM_COMMAND_ARG_NOT_PRESENT;
			read = true;
		}
	}

	// ---- ----

	if (!read) {
		*status = VM_ASSEMBLY_ERROR_INVALID_ARGUMENT;
		return false;
	}

	return true;
}

#define WRITE_CHAR(CHARACTER) {                                           \
	char copy = CHARACTER;                                                \
	if (!vm_write_bytes(output_stream, (uint8_t*) &copy, sizeof(copy))) { \
		*status = VM_ERROR_INSUFFICIENT_BUFFER;                           \
		return false;                                                     \
	}                                                                     \
}

bool vm_text_write_arg(
	VMStatus* status,
	VMForwardStream* output_stream,

	const VMAssemblyArgument* argument
) {
	assert(status != NULL);
	assert(output_stream != NULL);
	assert(argument != NULL);

	bool uses_memory = (argument->arg_type & VM_COMMAND_ARG_USES_MEMORY) != 0;
	bool uses_register = (argument->arg_type & VM_COMMAND_ARG_USES_REGISTER) != 0;
	bool uses_immediate_const = (argument->arg_type & VM_COMMAND_ARG_USES_IMMEDIATE_CONST) != 0;

	if (uses_memory) {
		WRITE_CHAR('[');
	}

	if (uses_register) {
		WRITE_CHAR((char) ('a' + argument->register_index));
		WRITE_CHAR('x');

		if (uses_immediate_const) {
			WRITE_CHAR('+');
		}
	}

	if (uses_immediate_const) {
		if (argument->immediate_const.is_label) {
			if (!vm_write_bytes(output_stream, (const uint8_t*) argument->immediate_const.label, strlen(argument->immediate_const.label))) {
				*status = VM_ERROR_INSUFFICIENT_BUFFER;
				return false;
			}
		} else {
			int num_bytes_written = snprintf((char*) output_stream->bytes, output_stream->length, "%" PRId32, argument->immediate_const.value);
			/*
			// TODO: handle errors.
			if (num_bytes_written <= 0) {
				*status = 
				return false;
			}
			*/
			output_stream->bytes += num_bytes_written;
			output_stream->offset += num_bytes_written;
			output_stream->length -= num_bytes_written;
		}
	}

	if (uses_memory) {
		WRITE_CHAR(']');
	}

	return true;
}
