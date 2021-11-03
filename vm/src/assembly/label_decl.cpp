#include "assembly/label_decl.h"

bool vm_text_lookahead_line_is_label_decl(unsigned char* line, size_t length) {
	return length >= 1 && string[length - 1] == ':';
}

bool vm_text_read_label_decl(VmAssemblyStatus* status, VmForwardStream* stream, char* label) {
	int num_chars_read = 0;
	int num_read = sscanf(stream->bytes, VM_ASSEMBLY_LABEL_DECL_FORMAT "%n", label, &num_chars_read);

	if (num_read < 0) {
		status->error = VM_ASSEMBLY_ERROR_WHILE_READING;
		return false;
	}

	if (num_chars_read == 0) {
		status->error = VM_ASSEMBLY_INVALID_LABEL_DECL;
		return false;
	}

	// Label length is checked by scanf format.

	return true;
}