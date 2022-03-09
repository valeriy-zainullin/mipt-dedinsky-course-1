#pragma once

#include "assembly/argument.h"
#include "assembly/command.h"
#include "status.h"
#include "support/forward_stream.h"

#include <stdbool.h>
#include <stdio.h>

struct VMAssemblyOperation {
	char command[VM_ASSEMBLY_MAX_COMMAND_LENGTH + 1];
	VMAssemblyArgument argument;
};

bool vm_text_read_operation(VMStatus* status, VMForwardStream* input_stream, VMAssemblyOperation* operation);
bool vm_text_write_operation(VMStatus* status, VMForwardStream* output_stream, const VMAssemblyOperation* operation);
