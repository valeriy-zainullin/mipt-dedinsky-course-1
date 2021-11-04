#pragma once

#include "assembly/argument.h"
#include "assembly/command.h"
#include "status.h"
#include "support/forward_stream.h"

#include <stdbool.h>
#include <stdio.h>

struct VmAssemblyOperation {
	char command[VM_ASSEMBLY_MAX_COMMAND_LENGTH + 1];
	VmAssemblyArgument argument;
};

bool vm_text_read_operation(VmStatus* status, VmForwardStream* input_stream, VmAssemblyOperation* operation);
bool vm_text_write_operation(VmStatus* status, VmForwardStream* output_stream, const VmAssemblyOperation* operation);
