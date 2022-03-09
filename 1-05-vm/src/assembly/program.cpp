#include "assembly/program.h"

#include "assembly/directive.h"
#include "assembly/label.h"
#include "assembly/label_decl.h"
#include "assembly/operation.h"
#include "support/forward_stream.h"

#include <ctype.h>

#define INVOKE_HOOK(HOOK, ...)        \
	if (!HOOK(status, __VA_ARGS__)) { \
		return false;                 \
	}

bool vm_text_process_directive(VMStatus* status, VMForwardStream* stream, void* argument) {

	VMAssemblyDirective directive = {};

	if (!vm_text_read_directive(status, stream, &directive)) {
		return false;
	}

	INVOKE_HOOK(vm_text_hook_on_directive, argument, &directive);

	return true;

}

bool vm_text_process_operation(VMStatus* status, VMForwardStream* stream, void* argument) {

	VMAssemblyOperation operation = {};

	if (!vm_text_read_operation(status, stream, &operation)) {
		return false;
	}

	INVOKE_HOOK(vm_text_hook_on_operation, argument, &operation);

	return true;

}

bool vm_text_process_label_decl(VMStatus* status, VMForwardStream* stream, void* argument) {

	char label_name[VM_ASSEMBLY_MAX_LABEL_LENGTH + 1] = {};

	if (!vm_text_read_label_decl(status, stream, label_name)) {
		return false;
	}

	INVOKE_HOOK(vm_text_hook_on_label_decl, argument, label_name);

	return true;

}

bool vm_text_process_line(VMStatus* status, unsigned char* line, size_t length, void* argument) {
	VMForwardStream stream = {};
	vm_init_stream(&stream, (uint8_t*) line, length);

	if (vm_text_lookahead_line_is_directive(line, length)) {
		return vm_text_process_directive(status, &stream, argument);
	}

	if (vm_text_lookahead_line_is_label_decl(line, length)) {
		return vm_text_process_label_decl(status, &stream, argument);
	}

	return vm_text_process_operation(status, &stream, argument);
}

static void skip_leading_spaces(unsigned char** line, size_t* length) {
	while (*length >= 1 && isspace(**line)) {
		++*line;
		--*length;
	}
}

static bool is_comment(unsigned char* line, size_t length) {
	(void) length;

	return line[0] == ';';
}

VMAssemblyStatus vm_text_process_program(TextLines* lines, void* argument) {
	VMAssemblyStatus assembly_status;
	assembly_status.line = 0;
	assembly_status.error = VM_SUCCESS;

	VMStatus* status = &assembly_status.error;

	if (!vm_text_hook_on_program_start(status, argument)) {
		return assembly_status;
	}

	for (size_t i = 0; i < lines->number_of_lines; ++i) {

		TextLine* text_line = &lines->lines[i];

		assembly_status.line = i;

		unsigned char* line = text_line->first_character;
		size_t length = (size_t) (text_line->after_the_last_character - text_line->first_character);

		skip_leading_spaces(&line, &length);

		if (length == 0) {
			continue;
		}

		if (is_comment(line, length)) {
			continue;
		}

		if (!vm_text_process_line(status, line, length, argument)) {
			return assembly_status;
		}
	}

	if (!vm_text_hook_on_program_end(status, argument)) {
		return assembly_status;
	}

	return assembly_status;

}