#include <stdio.h>

#include "arch/assembly/register.h"
#include "arch/assembly/label.h"
#include "arch/assembly/command.h"
#include "arch/assembly/immediate_const.h"

bool vm_read_text_arg(char* string, char* register_name, int32_t* immediate_const, FILE* input_stream) {
	bool read = false;

	#define IMMEDIATE_CONST_IS_NUMBER false
	#define IMMEDIATE_CONST_IS_LABEL  true

	#define TRY_READ_ARG(FORMAT, ARG_TYPE, IMMEDIATE_CONST_IS_LABEL, ...)                                \
		if (!read) {                                                                                     \
			int num_characters_read = 0;                                                                 \
			num_read = sscanf(string, FORMAT "%n", __VA_ARGS__, &num_characters_read);                   \
			if (num_characters_read != 0) {                                                              \
				arg_type = ARG_TYPE;                                                                     \
                                                                                                         \
				if (IMMEDIATE_CONST_IS_LABEL) {                                                          \
					immediate_const_is_label = true;                                                     \
				}                                                                                        \
                                                                                                         \
				read = true;                                                                             \
			}                                                                                            \
		}

	// ---- Memory arguments. ----

	TRY_READ_ARG(
		"[" REGISTER_NAME_SCANF_FORMAT "+" IMMEDIATE_CONST_SCANF_FORMAT "]",
		COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST,
		IMMEDIATE_CONST_IS_NUMBER,
		register_name,
		&immediate_const
	);
	
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