#pragma once

#include "text.h"

#include <stddef.h>
#include <stdio.h>

enum AssemblyError {
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

struct AssemblyStatus {
	size_t line;
	AssemblyError error;
};

AssemblyStatus vm_assemble(TextLines* lines, FILE* output_stream);
