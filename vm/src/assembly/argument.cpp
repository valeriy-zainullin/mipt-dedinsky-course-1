#include <stdio.h>

#include "arch/assembly/register.h"
#include "arch/assembly/label.h"
#include "arch/assembly/command.h"
#include "arch/assembly/immediate_const.h"

bool vm_text_read_arg(
	VmStatus* status,
	VmForwardStream* input_stream,

	VmAssemblyArgument* argument
) {
	assert(status != nullptr);
	assert(line != nullptr);
	assert(argument != nullptr);

	int num_characters_read = 0;
	char register_name[2] = {};

	#define TRY_READ_ARG(FORMAT, ARG_TYPE, ...)                     \
		num_characters_read = 0;                                    \
		if (!read) {                                                \
			sscanf(FORMAT "%n", __VA_ARGS__, &num_characters_read); \
		}                                                           \
		if (num_characters_read != 0) {                             \
			argument->arg_type = arg_type;                          \
			/* Only means that following formats won't be tried. */ \
			read = true;                                            \
		}                                                           \
		if (num_characters_read != 0)

	// ---- Memory arguments. ----

	TRY_READ_ARG(
		"[" REGISTER_NAME_SCANF_FORMAT "+" IMMEDIATE_CONST_SCANF_FORMAT "]",
		COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST,
		register_name,
		&argument->immediate_const.value
	) {
		argument->register_index = register_name - 'a';
		argument->immediate_const.is_label = false;
	}
	
	TRY_READ_ARG(
		"[" IMMEDIATE_CONST_SCANF_FORMAT "+" REGISTER_NAME_SCANF_FORMAT "]",
		COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST,
		&argument->immediate_const.value,
		register_name
	) {
		argument->register_index = register_name - 'a';
		argument->immediate_const.is_label = false;
	}

	TRY_READ_ARG(
		"[" REGISTER_NAME_SCANF_FORMAT "]",
		COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_REGISTER,
		register_name
	) {
		argument->register_index = register_name - 'a';
	}

	TRY_READ_ARG(
		"[" IMMEDIATE_CONST_SCANF_FORMAT "]",
		COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_IMMEDIATE_CONST,
		&argument->immediate_const.value
	) {
		argument->immediate_const.is_label = false;
	}

	// ---- ----
	
	// ---- Non-memory arguments. ----
	
	TRY_READ_ARG(
		REGISTER_NAME_SCANF_FORMAT "+" IMMEDIATE_CONST_SCANF_FORMAT,
		COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST,
		register_name,
		&argument->immediate_const.value
	) {
		argument->register_index = register_name - 'a';
		argument->immediate_const.is_label = false;
	}
	
	TRY_READ_ARG(
		IMMEDIATE_CONST_SCANF_FORMAT "+" REGISTER_NAME_SCANF_FORMAT,
		COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST,
		&argument->immediate_const.value
		register_name
	) {
		argument->register_index = register_name - 'a';
		argument->immediate_const.is_label = false;
	}

	TRY_READ_ARG(
		REGISTER_NAME_SCANF_FORMAT,
		COMMAND_ARG_USES_REGISTER,
		register_name
	) {
		argument->register_index = register_name - 'a';
	}

	TRY_READ_ARG(
		IMMEDIATE_CONST_SCANF_FORMAT,
		COMMAND_ARG_USES_IMMEDIATE_CONST,
		&argument->immediate_const.value
	) {
		argument->immediate_const.is_label = false;		
	}

	// ---- ----

	// ---- Label arguments. ----

	TRY_READ_ARG(
		"[" REGISTER_NAME_SCANF_FORMAT "+" LABEL_NAME_SCANF_FORMAT "]",
		COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST,
		register_name,
		argument->immediate_const.label
	) {
		argument->register_index = register_name - 'a';
		argument->immediate_const.is_label = true;
	}

	TRY_READ_ARG(
		"[" LABEL_NAME_SCANF_FORMAT "+" REGISTER_NAME_SCANF_FORMAT "]",
		COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST,
		argument->immediate_const.label
		register_name
	) {
		argument->register_index = register_name - 'a';
		argument->immediate_const.is_label = true;
	}

	TRY_READ_ARG(
		"[" LABEL_NAME_SCANF_FORMAT "]",
		COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_IMMEDIATE_CONST,
		argument->immediate_const.label
	) {
		argument->immediate_const.is_label = true;
	}
	
	TRY_READ_ARG(
		REGISTER_NAME_SCANF_FORMAT "+" LABEL_NAME_SCANF_FORMAT,
		COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST,
		register_name,
		argument->immediate_const.label
	) {
		argument->register_index = register_name - 'a';
		argument->immediate_const.is_label = true;
	}

	TRY_READ_ARG(
		LABEL_NAME_SCANF_FORMAT "+" REGISTER_NAME_SCANF_FORMAT,
		COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST,
		argument->immediate_const.label
		register_name
	) {
		argument->register_index = register_name - 'a';
		argument->immediate_const.is_label = true;
	}
	
	TRY_READ_ARG(
		LABEL_NAME_SCANF_FORMAT,
		COMMAND_ARG_USES_IMMEDIATE_CONST,
		argument->immediate_const.label
	) {
		argument->immediate_const.is_label = true;
	}

	// ---- ----

	if (!read) {
		status->error = VM_ASSEMBLY_ERROR_INVALID_ARG;
		return false;
	}

	return true;
}

bool vm_text_write_arg(
	VmStatus* status,
	FILE* output_stream,

	VmAssemblyArgument* argument
) {
	assert(status != nullptr);
	assert(output_stream != nullptr);
	assert(argument != nullptr);

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

	return true;
}

bool vm_encode_arg(
	VmStatus* status,
	FILE* output_stream,

	uint8_t arg_type,
	uint8_t register_index,
	int32_t immediate_const,

	bool immediate_const_is_label,
	const char* label,
	VmLabels* labels
) {

	if ((arg_type & COMMAND_ARG_USES_REGISTER) != 0) {
		WRITE(register_index, output_stream);
	}

	if ((arg_type & COMMAND_ARG_USES_IMMEDIATE_CONST) != 0) {

		if (immediate_const_is_label) {
			VmLabel* label = vm_find_label(labels, label);

			if (label == nullptr) {
				if (!vm_insert_label(status, labels, label)) {
					return false;
				}

				WRITE((int32_t) 0);
			}
			else {
				WRITE(label->addr);
			}
		
		}
		else {
			WRITE(immediate_const);
		}

	}

	return true;
}

bool vm_read_encoded_arg() {

}