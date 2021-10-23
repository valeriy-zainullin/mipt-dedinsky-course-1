#include "status.h"

#include <stdbool.h>
#include <string.h>
#include <cassert.h>

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

static bool is_label_decl(TextLine* line) {
	return *(line.after_the_last - 1) == ':';
}

static bool is_space(const unsigned char character) {
	return character == ' ' || character == '\t';
}

static bool process_label(Assembler* assembler, TextLine* line, FILE* output_stream) {
	if (!is_label_decl(line)) {
		return false;
	}

	TextIterator iterator = line->first_character;
	while (iterator != line->after_the_last && is_space(*iterator)) {
		++iterator;
	}

	if (*iterator == ':') {
		assembler->status = VM_ASSEMBLY_STATUS_EMPTY_LABEL;
		return true;
	}

	size_t label_length = (size_t) (line.after_the_last - iterator) - 1;
	if (label_length > VM_ASSEMBLY_MAX_LABEL_LENGTH) {
		assembler->status = VM_ASSEMBLY_STATUS_LABEL_IS_TOO_LONG;
		return true;
	}

	if (assembler->nlabels + 1 > VM_ASSEMBLY_MAX_NUMBER_OF_LABELS) {
		assembler->error = VM_ASSEMBLY_TOO_MANY_LABELS;
		return false;
	}
	
	AssemblyLabel* label = assembler->labels[assembler->nlabels];

	memcpy(label->name, line->first_character, label_length);

	label->name[label_length] = 0;

	label->defined = true;
	label->addr = assembler->ip;

	return true;
}

static bool process_command(Assembler* assembler, TextLine* line, FILE* output_stream) {
	int num_read = 
}

static AssemblyStatus do_assembling_pass(TextLines* lines, FILE* output_stream) {
	AssemblyStatus status;
	status.line = 0;
	status.error = VM_ASSEMBLY_STATUS_SUCCESS;

	Assembler assembler = {};

	if (lines.number_of_lines == 0) {
		assembler.error = VM_ASSEMBLY_ERROR_EMPTY_PROGRAM;
		return make_status(assembler);
	}

	for (size_t i = 0; i < lines.number_of_lines; ++i) {

		assembler.line = i;	
		TextLine* line = &lines.lines[assembler.line];

		assert(*(line.after_the_last - 1) == 0);
		assert(line.first_character != line.after_the_last);

		if (
			process_label(&assembler, line, output_stream) ||
			process_command(&assembler, line, output_stream)
		) {
			if (assembler.error == VM_ASSEMBLY_STATUS_SUCCESS) {
				continue;
			} else {
				return make_status(assembler);
			}
		}

		assembler.error = VM_ASSEMBLY_INVALID_EXPRESSION;

#define COMMAND(NAME, NUMBER, ENCODING_CODE, DECODING_CODE, EXECUTION_CODE) { \
	\
}
#include "arch/commands.h"
#undef COMMAND
	}
}

AssemblyStatus vm_assemble(TextLines* lines, FILE* output_stream) {
