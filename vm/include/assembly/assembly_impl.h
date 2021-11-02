#pragma once

#include "assembly/status.h"

#include <stdio.h>

#define WRITE(VARIABLE) \
	if (fwrite(&VARIABLE, sizeof(VARIABLE), 1, output_stream) < 1) { \
		status->error = VM_ASSEMBLY_ERROR_WHILE_WRITING; \
		return; \
	} \
	*ip += sizeof(VARIABLE);

#define READ(VARIABLE) \
	if (fread(&VARIABLE, sizeof(VARIABLE), 1, input_stream) < 1) { \
		status->error = VM_ASSEMBLY_ERROR_WHILE_READING; \
		return; \
	}
