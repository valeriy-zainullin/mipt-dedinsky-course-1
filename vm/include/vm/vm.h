#pragma once

#include "status.h"
#include "vm/state.h"

#include <stdbool.h>
#include <stdio.h>

bool vm_on_debugger_notified(VMStatus* status, VMState* state, bool* continue_execution, void* debugger_arg);

void vm_execute(FILE* program, FILE* input_stream, FILE* output_stream, void* debugger_arg);