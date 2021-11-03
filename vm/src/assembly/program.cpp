#include "assembly/label_decl.h"

#define INVOKE_HOOK(HOOK, ...)        \
	if (!HOOK(status, __VA_ARGS__)) { \
		return status;                \
	}

bool vm_text_process_operation(VmAssemblyStatus* status, VmForwardStream* stream, void* argument) {

	Operation operation;

	if (!vm_text_read_operation(status, stream, &operation)) {
		return false;
	}

	INVOKE_HOOK(vm_hook_on_operation, argument, operation);

	return true;

}

bool vm_text_process_label_decl(VmAssemblyStatus* status, VmForwardStream* stream, void* argument) {

	unsigned char label_name[VM_ASSEMBLY_MAX_LABEL_LENGTH + 1] = {};

	if (!vm_text_read_label_decl(status, stream, label_name)) {
		return false;
	}

	INVOKE_HOOK(vm_hook_on_label_decl, argument, label_name);

	return true;

}

bool vm_text_process_line(VmAssemblyStatus* status, unsigned char* line, size_t length, void* argument) {
	VmForwardStream stream = {};
	stream.bytes = (uint8_t*) line;
	stream.offset = 0;
	stream.length = length;

	if (vm_text_lookeahead_line_is_label_decl(line, length)) {
		return vm_text_process_label_decl(status, &stream, argument)
	}

	return vm_text_process_operation(status, &stream, argument);
}

VmAssemblyStatus vm_text_process_program(TextLines* lines, void* argument) {
	VmAssemblyStatus status;
	status.line = 0;
	status.error = VM_ASSEMBLY_SUCCESS;

	INVOKE_HOOK(vm_hook_on_program_start, arguments);

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

		if (!process_line(&status, line, length, labels, &num_labels, &ip, output_stream)) {
			return status;
		}
	}

	INVOKE_HOOK(vm_hook_on_program_end, arguments);

	return status;

}