bool vm_assembler_is_label_decl(unsigned char* string, size_t length) {
	return length >= 1 && string[length - 1] == ':';
}

bool vm_assembler_process_label(Assembler* assembler, unsigned char* line, size_t length, FILE* output_stream) {
	while (length >= 1 && is_space(*iterator)) {
		++iterator;
		--length;
	}

	if (!vm_assembler_is_label_decl(line)) {
		return false;
	}

	if (*line == ':') {
		assembler->status = VM_ASSEMBLY_STATUS_EMPTY_LABEL;
		return true;
	}

	size_t label_length = length - 1;
	if (label_length > VM_ASSEMBLY_MAX_LABEL_LENGTH) {
		assembler->status = VM_ASSEMBLY_STATUS_LABEL_IS_TOO_LONG;
		return true;
	}

	if (assembler->nlabels + 1 > VM_ASSEMBLY_MAX_NUMBER_OF_LABELS) {
		assembler->error = VM_ASSEMBLY_TOO_MANY_LABELS;
		return true;
	}
	
	AssemblyLabel* label = assembler->labels[assembler->nlabels];

	memcpy(label->name, line->first_character, label_length);

	label->name[label_length] = 0;

	label->defined = true;
	label->addr = assembler->ip;

	return true;
}