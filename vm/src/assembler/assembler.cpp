#include "assembler.h"

#include "arch/arg_types.h"

#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>
#include <inttypes.h>
#include <ctype.h>

#define MAX_COMMAND_LENGTH_MACRO 10
#define LABEL_NAME_MAX_LENGTH_MACRO 256

static const size_t VM_ASSEMBLY_MAX_NUMBER_OF_LABELS = 1024;
static const size_t VM_ASSEMBLY_MAX_LABEL_LENGTH = LABEL_NAME_MAX_LENGTH_MACRO;

struct AssemblyLabel {
	bool defined;
	char name[VM_ASSEMBLY_MAX_LABEL_LENGTH + 1];
	size_t addr = 0;
};

struct Assembler {

	AssemblyLabel labels[VM_ASSEMBLY_MAX_NUMBER_OF_LABELS];
	size_t num_labels;

	size_t line;

	size_t ip;

	AssemblyError error;

};

const size_t VM_ASSEMBLY_MAX_INSTRUCTION_LENGTH = 8;

#define EXPAND2(MACRO) #MACRO
#define EXPAND(MACRO) EXPAND2(MACRO)

#define COMMAND_NAME_SCANF_FORMAT "%" EXPAND(MAX_COMMAND_LENGTH_MACRO) "s"
#define LABEL_NAME_SCANF_FORMAT "%" EXPAND(LABEL_NAME_MAX_LENGTH_MACRO) "[a-zA-Z0-9_]"

const size_t VM_ASSEMBLY_MAX_COMMAND_LENGTH = MAX_COMMAND_LENGTH_MACRO;

#define WRITE(VARIABLE) \
	if (fwrite(&VARIABLE, sizeof(VARIABLE), 1, output_stream) < 1) { \
		status->error = VM_ASSEMBLY_ERROR_WHILE_WRITING; \
		return; \
	} \
	*ip += sizeof(VARIABLE);

static void assemble_operation(
	AssemblyStatus* status,
	uint8_t command_number,
	uint8_t* allowed_arg_types,
	size_t num_allowed_arg_types,
	uint8_t arg_type,
	bool immediate_const_is_label,
	char register_name[2],
	int32_t immediate_const,
	int32_t* ip,
	FILE* output_stream
) {

	bool found_arg_type = false;
	for (size_t i = 0; i < num_allowed_arg_types; ++i) {
		if (allowed_arg_types[i] == arg_type) {
			found_arg_type = true;
			break;
		}
	}
	if (!found_arg_type) {
		status->error = VM_ASSEMBLY_ERROR_INVALID_ARG;
		return;
	}
	
	assert(command_number < (1 << 5));

	uint8_t operation = (arg_type << 5) | command_number;

	WRITE(operation);

	if ((arg_type & COMMAND_ARG_USES_REGISTER) != 0) {
		int8_t register_index = register_name[0] - 'a';
		WRITE(register_index);
	}

	if ((arg_type & COMMAND_ARG_USES_IMMEDIATE_CONST) != 0) {
		if (immediate_const_is_label) {
			uint32_t addr = 0;
			WRITE(addr);
		} else {
			WRITE(immediate_const);
		}
	}

}

#undef WRITE

static bool process_operation(
	AssemblyStatus* status,
	unsigned char* line,
	size_t length,
	AssemblyLabel* labels,
	size_t* num_labels,
	int32_t* ip,
	FILE* output_stream
) {
	((void) length);
	((void) labels);
	((void) num_labels);
	((void) ip);

	char command[VM_ASSEMBLY_MAX_COMMAND_LENGTH + 1];

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
	#define TRY_READ_ARG(FORMAT, ARG_TYPE, IMMEDIATE_CONST_IS_LABEL, ...) \
		if (!read) { \
			int num_characters_read = 0; \
			num_read = sscanf((char*) line + arg_start, FORMAT "%n", __VA_ARGS__, &num_characters_read); \
			if (num_characters_read != 0) { \
				arg_type = ARG_TYPE; \
\
				if (IMMEDIATE_CONST_IS_LABEL) { \
					immediate_const_is_label = true; \
				} \
\
				read = true; \
			} \
		}

	TRY_READ_ARG("[%1[a-c]x+%10" SCNd32 "]", COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST, false, register_name, &immediate_const);
	TRY_READ_ARG("[%10" SCNd32 "+%1[a-c]x]", COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST, false, &immediate_const, register_name);
	TRY_READ_ARG("[%10" SCNd32 "]", COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_IMMEDIATE_CONST, false, &immediate_const);
	TRY_READ_ARG("[%1[a-c]x]", COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_REGISTER, false, register_name);
	TRY_READ_ARG("%1[a-c]x+%10" SCNd32, COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST, false, register_name, &immediate_const);
	TRY_READ_ARG("%10" SCNd32 "+%1[a-c]x", COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST, false, &immediate_const, register_name);
	TRY_READ_ARG("%10" SCNd32, COMMAND_ARG_USES_IMMEDIATE_CONST, false, &immediate_const);
	TRY_READ_ARG(" %1[a-c]x", COMMAND_ARG_USES_REGISTER, false, register_name);

	TRY_READ_ARG("[%1[a-c]x+" LABEL_NAME_SCANF_FORMAT "]", COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST, true, register_name, label);
	TRY_READ_ARG("[" LABEL_NAME_SCANF_FORMAT "+%1[a-c]x]", COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST, true, label, register_name);
	TRY_READ_ARG("[" LABEL_NAME_SCANF_FORMAT "]", COMMAND_ARG_USES_MEMORY | COMMAND_ARG_USES_IMMEDIATE_CONST, true, label);
	TRY_READ_ARG(" %1[a-c]x+" LABEL_NAME_SCANF_FORMAT, COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST, true, register_name, label);
	TRY_READ_ARG(LABEL_NAME_SCANF_FORMAT "+%1[a-c]x", COMMAND_ARG_USES_REGISTER | COMMAND_ARG_USES_IMMEDIATE_CONST, true, label, register_name);
	TRY_READ_ARG(LABEL_NAME_SCANF_FORMAT, COMMAND_ARG_USES_IMMEDIATE_CONST, true, label);

	if (!read) {
		return false;		
	}

	#define COMMAND(NAME, NUMBER, ALLOWED_ARG_TYPES)                    \
		if (strcmp(command, #NAME) == 0) {                              \
                                                                        \
			uint8_t command_number = NUMBER;                            \
			uint8_t allowed_arg_types[] = ALLOWED_ARG_TYPES;            \
                                                                        \
			assemble_operation(                                         \
				status,                                                 \
				command_number,                                         \
				allowed_arg_types,                                      \
				sizeof(allowed_arg_types) / sizeof(*allowed_arg_types), \
				arg_type,                                               \
				immediate_const_is_label,                               \
				register_name,                                          \
				immediate_const,                                        \
				ip,                                                     \
				output_stream                                           \
			);                                                          \
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
}

static bool is_label_decl(unsigned char* string, size_t length) {
	return length >= 1 && string[length - 1] == ':';
}

// Max number of operations restriction.

static bool process_label(
	AssemblyStatus* status,
	unsigned char* line,
	size_t length,
	AssemblyLabel* labels,
	size_t* num_labels,
	int32_t* ip,
	FILE* output_stream
) {
	((void) output_stream);

	if (!is_label_decl(line, length)) {
		return false;
	}

	if (*line == ':') {
		status->error = VM_ASSEMBLY_ERROR_EMPTY_LABEL;
		return true;
	}

	size_t label_length = length - 1;
	if (label_length > VM_ASSEMBLY_MAX_LABEL_LENGTH) {
		status->error = VM_ASSEMBLY_ERROR_LABEL_IS_TOO_LONG;
		return true;
	}

	if (*num_labels >= VM_ASSEMBLY_MAX_NUMBER_OF_LABELS) {
		status->error = VM_ASSEMBLY_ERROR_TOO_MANY_LABELS;
		return true;
	}

	AssemblyLabel* label = &labels[*num_labels];
	label->defined = true;
	memcpy(label->name, line, label_length);
	label->name[label_length] = 0;
	label->addr = *ip;

	*num_labels += 1;

	return true;
}

static void process_line(
	AssemblyStatus* status,
	unsigned char* line,
	size_t length,
	AssemblyLabel* labels,
	size_t* num_labels,
	int32_t* ip,
	FILE* output_stream
) {
	assert(line[length] == 0);

	if (process_label(status, line, length, labels, num_labels, ip, output_stream)) {
		return;
	}

	if (process_operation(status, line, length, labels, num_labels, ip, output_stream)) {
		return;
	}
	
	status->error = VM_ASSEMBLY_ERROR_INVALID_EXPRESSION;
}

AssemblyStatus vm_assemble(TextLines* lines, FILE* output_stream) {
	AssemblyStatus status;
	status.line = 0;
	status.error = VM_ASSEMBLY_SUCCESS;

	AssemblyLabel labels[VM_ASSEMBLY_MAX_NUMBER_OF_LABELS] = {};
	uint32_t num_labels = 0;
	int32_t ip = 0;

	for (size_t i = 0; i < lines->number_of_lines; ++i) {

		TextLine* text_line = &lines->lines[i];

		status.line = i;

		unsigned char* line = text_line->first_character;
		size_t length = (size_t) (text_line->after_the_last_character - text_line->first_character);

		while (length >= 1 && isspace(*line)) {
			++line;
			--length;
		}

		if (length == 0) {
			continue;
		}

		process_line(&status, line, length, labels, &num_labels, &ip, output_stream);
	}

	if (ip == 0) {
		status.line = lines->number_of_lines;
		status.error = VM_ASSEMBLY_ERROR_EMPTY_PROGRAM;
		return status;
	}
	
	return status;
}
