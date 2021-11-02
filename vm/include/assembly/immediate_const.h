#pragma once

#include <inttypes.h>

#include "arch/assembly/label.h"

#define VM_IMMEDIATE_CONST_SCANF_FORMAT "%10" SCNd32

struct VmAssemblyImmediateConst {
	int32_t value;

	bool is_label;
	char label[VM_ASSEMBLY_MAX_LABEL_LENGTH];
};
