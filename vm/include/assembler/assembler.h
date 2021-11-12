#pragma once

#include "assembly/label.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct VMAssemblyLabel {
	bool defined;
	char name[VM_ASSEMBLY_MAX_LABEL_LENGTH + 1];
	int32_t addr;
};

struct VMAssemblyLabels {
	VMAssemblyLabel labels[VM_ASSEMBLY_MAX_NUMBER_OF_LABELS];
	size_t nlabels;
};

struct VMAssembler {
	FILE* output_file;
	int pass;

	int32_t ip;
	VMAssemblyLabels* labels;
};

bool vm_init_assembler(VMAssembler* assembler);
bool vm_deinit_assembler(VMAssembler* assembler);
