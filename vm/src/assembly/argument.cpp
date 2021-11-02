#include <stdio.h>

#include "arch/assembly/register.h"
#include "arch/assembly/label.h"
#include "arch/assembly/command.h"
#include "arch/assembly/immediate_const.h"

bool vm_text_try_read_arg(
	VmStatus* status,

	const char* line,
	VmAssemblyArgument* argument
) {

	int num_characters_read = 0;

	#define TRY_READ_ARG(FORMAT, ARG_TYPE, ...)                                \
		num_characters_read = 0;
		sscanf(FORMAT "%n", __VA_ARGS__, &num_characters_read);
		if (num_characters_read != 0) {
			argument->type = arg_type;
		}
		if (num_characters_read != 0)

	// ---- Memory arguments. ----

	TRY_READ_ARG(
		"[" REGISTER_NAME_SCANF_FORMAT "+" IMMEDIATE_CONST_SCANF_FORMAT "]",
		COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST,
		
	) {
		argument->immediate_const_is_label = true;
		return true;
	}
	
	TRY_READ_ARG(
		"[" IMMEDIATE_CONST_SCANF_FORMAT "+" REGISTER_NAME_SCANF_FORMAT "]",
		COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST,
		IMMEDIATE_CONST_IS_NUMBER,
		&immediate_const,
		register_name
	);

	TRY_READ_ARG(
		"[" REGISTER_NAME_SCANF_FORMAT "]",
		COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_REGISTER,
		IMMEDIATE_CONST_IS_NUMBER,
		register_name
	);

	TRY_READ_ARG(
		"[" IMMEDIATE_CONST_SCANF_FORMAT "]",
		COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_IMMEDIATE_CONST,
		IMMEDIATE_CONST_IS_NUMBER,
		&immediate_const
	);

	// ---- ----
	
	// ---- Non-memory arguments. ----
	
	TRY_READ_ARG(
		REGISTER_NAME_SCANF_FORMAT "+" IMMEDIATE_CONST_SCANF_FORMAT,
		COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST,
		IMMEDIATE_CONST_IS_NUMBER,
		register_name,
		&immediate_const
	);
	
	TRY_READ_ARG(
		IMMEDIATE_CONST_SCANF_FORMAT "+" REGISTER_NAME_SCANF_FORMAT,
		COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST,
		IMMEDIATE_CONST_IS_NUMBER,
		&immediate_const,
		register_name
	);

	TRY_READ_ARG(
		REGISTER_NAME_SCANF_FORMAT,
		COMMAND_ARG_USES_REGISTER,
		IMMEDIATE_CONST_IS_NUMBER,
		register_name
	);

	TRY_READ_ARG(
		IMMEDIATE_CONST_SCANF_FORMAT,
		COMMAND_ARG_USES_IMMEDIATE_CONST,
		IMMEDIATE_CONST_IS_NUMBER,
		&immediate_const
	);

	// ---- ----

	// ---- Label arguments. ----

	TRY_READ_ARG(
		"[" REGISTER_NAME_SCANF_FORMAT "+" LABEL_NAME_SCANF_FORMAT "]",
		COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST,
		IMMEDIATE_CONST_IS_LABEL,
		register_name,
		label
	);

	TRY_READ_ARG(
		"[" LABEL_NAME_SCANF_FORMAT "+" REGISTER_NAME_SCANF_FORMAT "]",
		COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST,
		IMMEDIATE_CONST_IS_LABEL,
		label,
		register_name
	);
	
	TRY_READ_ARG(
		"[" LABEL_NAME_SCANF_FORMAT "]",
		COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_IMMEDIATE_CONST,
		IMMEDIATE_CONST_IS_LABEL,
		label
	);
	
	TRY_READ_ARG(
		REGISTER_NAME_SCANF_FORMAT "+" LABEL_NAME_SCANF_FORMAT,
		COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST,
		IMMEDIATE_CONST_IS_LABEL,
		register_name,
		label
	);
	
	TRY_READ_ARG(
		LABEL_NAME_SCANF_FORMAT "+" REGISTER_NAME_SCANF_FORMAT,
		COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST,
		IMMEDIATE_CONST_IS_LABEL,
		label,
		register_name
	);
	
	TRY_READ_ARG(
		LABEL_NAME_SCANF_FORMAT,
		COMMAND_ARG_USES_IMMEDIATE_CONST,
		IMMEDIATE_CONST_IS_LABEL,
		label
	);

	// ---- ----
}

bool vm_text_write_arg(
	VmStatus* status,
	FILE* output_stream,

	VmAssemblyArgument* argument
) {

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