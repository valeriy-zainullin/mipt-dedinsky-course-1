#pragma once

#include "assembly/label.h"
#include "status.h"
#include "support/forward_stream.h"

#include <stddef.h>
#include <inttypes.h>

#define VM_IMMEDIATE_CONST_SCANF_FORMAT "%10" SCNd32
#define VM_FLOAT_IMMEDIATE_CONST_SCANF_FORMAT "%f"

// 3 registers: ax, bx, cx.
#define VM_REGISTER_NAME_SCANF_FORMAT " %1[a-c]x"

static const size_t VM_MAX_REGISTER_INDEX = 2;

struct VMAssemblyImmediateConst {
	int32_t value;

	bool is_label;
	char label[VM_ASSEMBLY_MAX_LABEL_LENGTH];
};

struct VMAssemblyArgument {
	uint8_t arg_type;

	uint8_t register_index;

	VMAssemblyImmediateConst immediate_const;
};

bool vm_text_read_arg(VMStatus* status, VMForwardStream* input_stream, VMAssemblyArgument* argument);
bool vm_text_write_arg(VMStatus* status, VMForwardStream* output_stream, const VMAssemblyArgument* argument);
