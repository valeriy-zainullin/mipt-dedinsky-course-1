#pragma once

static const size_t VM_ASSEMBLY_MAX_LABEL_LENGTH = 256;

enum AssemblyError {
	VM_ASSEMBLY_SUCCESS = 0,
	VM_ASSEMBLY_ERROR_EMPTY_LABEL,
	VM_ASSEMBLY_ERROR_EMPTY_PROGRAM,
	VM_ASSEMBLY_TOO_MANY_LABELS,
	VM_ASSEMBLY_INVALID_EXPRESSION
};

struct AssemblyStatus {
	size_t line;
	AssemblyError error;
};