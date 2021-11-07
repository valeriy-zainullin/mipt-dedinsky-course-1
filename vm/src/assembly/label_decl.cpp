#include "assembly/label_decl.h"

#include <stdio.h>

bool vm_text_lookahead_line_is_label_decl(unsigned char* line, size_t length) {
	return length >= 1 && line[length - 1] == ':';
}

bool vm_text_read_label_decl(VmStatus* status, VmForwardStream* input_stream, char* label) {
	assert(status != NULL);
	assert(input_stream != NULL);
	assert(label != NULL);

	*status = VM_SUCCESS;

	int num_chars_read = 0;
	int num_read = sscanf((char*) input_stream->bytes, VM_ASSEMBLY_LABEL_DECL_SCANF_FORMAT "%n", label, &num_chars_read);

	if (num_read < 0) {
		*status = VM_ERROR_WHILE_READING;
		return false;
	}

	if (num_chars_read == 0) {
		*status = VM_ASSEMBLY_ERROR_INVALID_LABEL_DECL;
		return false;
	}

	// Label length is checked by scanf format.

	return true;
}