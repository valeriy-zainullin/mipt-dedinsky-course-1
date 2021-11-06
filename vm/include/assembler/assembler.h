#pragma once

#include "assembly/label.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct VmAssemblyLabel {
	bool defined;
	char name[VM_ASSEMBLY_MAX_LABEL_LENGTH + 1];
	int32_t addr = 0;
};

struct VmAssemblyLabels {
	VmAssemblyLabel labels[VM_ASSEMBLY_MAX_NUMBER_OF_LABELS];
	size_t nlabels;
};

struct VmAssembler {
	FILE* output_file;
	int pass;

	int32_t ip;
	VmAssemblyLabels labels;
};
