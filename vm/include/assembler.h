#pragma once

#include "text.h"

#include <stddef.h>
#include <stdio.h>

enum VmAssemblyError {
	VM_ASSEMBLY_SUCCESS = 0,
	VM_ASSEMBLY_ERROR_EMPTY_LABEL,
	VM_ASSEMBLY_ERROR_EMPTY_PROGRAM,
	VM_ASSEMBLY_ERROR_LABEL_IS_TOO_LONG,
	VM_ASSEMBLY_ERROR_TOO_MANY_LABELS,
	VM_ASSEMBLY_ERROR_INVALID_COMMAND,
	VM_ASSEMBLY_ERROR_INVALID_EXPRESSION,
	VM_ASSEMBLY_ERROR_INVALID_ARG,
	VM_ASSEMBLY_ERROR_WHILE_WRITING
};

struct VmAssemblyStatus {
	size_t line;
	AssemblyError error;
};

struct VmAssembler {

	VmAssemblyLabel labels[VM_ASSEMBLY_MAX_NUMBER_OF_LABELS];
	size_t num_labels;

	size_t line;

	size_t ip;

	AssemblyError error;

};

AssemblyStatus vm_assemble(TextLines* lines, FILE* output_stream);
