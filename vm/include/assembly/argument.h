#pragma once

#include "assembly/label.h"
#include "status.h"
#include "support/forward_stream.h"

#include <stddef.h>
#include <inttypes.h>

#define VM_IMMEDIATE_CONST_SCANF_FORMAT "%10" SCNd32

// 3 registers: ax, bx, cx.
#define VM_REGISTER_NAME_SCANF_FORMAT " %1[a-c]x"

static const size_t VM_MAX_REGISTER_INDEX = 2;

struct VmAssemblyImmediateConst {
	int32_t value;

	bool is_label;
	char label[VM_ASSEMBLY_MAX_LABEL_LENGTH];
};

struct VmAssemblyArgument {
	uint8_t arg_type;

	uint8_t register_index;

	VmAssemblyImmediateConst immediate_const;
};

bool vm_text_read_arg(VmStatus* status, VmForwardStream* input_stream, VmAssemblyArgument* argument);
bool vm_text_write_arg(VmStatus* status, VmForwardStream* output_stream, const VmAssemblyArgument* argument);
