#include "assembly/argument.h"

#include "arg_type.h"
#include "assembly/label.h"
#include "assembly/command.h"

#include <ctype.h>
#include <stdio.h>

bool vm_text_read_arg(
	VmStatus* status,
	VmForwardStream* input_stream,

	VmAssemblyArgument* argument
) {
	assert(status != nullptr);
	assert(input_stream != nullptr);
	assert(argument != nullptr);

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
		"[" VM_REGISTER_NAME_SCANF_FORMAT "+" VM_IMMEDIATE_CONST_SCANF_FORMAT "]",
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_REGISTER | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		register_name,
		&argument->immediate_const.value
	) {
		argument->register_index = (uint8_t) (register_name[0] - 'a');
		argument->immediate_const.is_label = false;
	}
	
	TRY_READ_ARG(
		"[" VM_IMMEDIATE_CONST_SCANF_FORMAT "+" VM_REGISTER_NAME_SCANF_FORMAT "]",
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_REGISTER | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		&argument->immediate_const.value,
		register_name
	) {
		argument->register_index = (uint8_t) (register_name[0] - 'a');
		argument->immediate_const.is_label = false;
	}

	TRY_READ_ARG(
		"[" VM_REGISTER_NAME_SCANF_FORMAT "]",
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_REGISTER,
		register_name
	) {
		argument->register_index = (uint8_t) (register_name[0] - 'a');
	}

	TRY_READ_ARG(
		"[" VM_IMMEDIATE_CONST_SCANF_FORMAT "]",
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
		"[" VM_REGISTER_NAME_SCANF_FORMAT "+" VM_ASSEMBLY_LABEL_NAME_SCANF_FORMAT "]",
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_REGISTER | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		register_name,
		argument->immediate_const.label
	) {
		argument->register_index = (uint8_t) (register_name[0] - 'a');
		argument->immediate_const.is_label = true;
	}

	// TODO: same format for format string names.
	TRY_READ_ARG(
		"[" VM_ASSEMBLY_LABEL_NAME_SCANF_FORMAT "+" VM_REGISTER_NAME_SCANF_FORMAT "]",
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_REGISTER | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		argument->immediate_const.label,
		register_name
	) {
		argument->register_index = (uint8_t) (register_name[0] - 'a');
		argument->immediate_const.is_label = true;
	}

	TRY_READ_ARG(
		"[" VM_ASSEMBLY_LABEL_NAME_SCANF_FORMAT "]",
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

/*bool vm_text_write_arg(
	VmStatus* status,
	FILE* output_stream,

	VmAssemblyArgument* argument
) {
	assert(status != nullptr);
	assert(output_stream != nullptr);
	assert(argument != nullptr);
*//*
	bool uses_memory = (argument->arg_type & COMMAND_ARG_USES_MEMORY) != 0;
	bool uses_register = (argument->arg_type & COMMAND_ARG_USES_REGISTER) != 0;
	bool uses_immediate_const = (argument->arg_type & COMMAND_ARG_USES_IMMEDIATE_CONST) != 0;

	if (uses_memory) {
		TEXT_WRITE_CHAR('[');
	}

	if (uses_register) {
		TEXT_WRITE_CHAR((char) ('a' + argument->register_index));
		TEXT_WRITE_CHAR('x');

		if (uses_immediate_const) {
			TEXT_WRITE_CHAR('+');
		}
	}

	if (uses_immediate_const) {
		if (argument->immediate_const.is_label) {
			TEXT_WRITE_STRING(argument->immediate_const.label);
		} else {
			TEXT_WRITE_INT32(argument->immediate_const);
		}
	}

	if (uses_memory) {
		TEXT_WRITE_CHAR(']');
	}
*/
/*	return true;
}*/
