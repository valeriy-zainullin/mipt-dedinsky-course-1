#pragma once

#include "bytecode/operation.h"
#include "status.h"

#include "stack.h"

#include <stddef.h>
#include <stdint.h>

static const size_t VM_MACHINE_MEMORY_SIZE = 64 * 1024 * 1024;
static const size_t VM_MACHINE_NUM_REGISTERS = 3;

static const size_t VM_MACHINE_VIDEO_MEMORY_SIZE = 3 * 1024 * 768;
static const size_t VM_MACHINE_TRUE_MEMORY_SIZE = VM_MACHINE_MEMORY_SIZE - VM_MACHINE_VIDEO_MEMORY_SIZE;

struct VmState {
	uint8_t memory[VM_MACHINE_MEMORY_SIZE];
	int32_t registers[VM_MACHINE_NUM_REGISTERS];
	int32_t ip;
	StackInt stack;
};

bool vm_on_send_int(VmStatus* status, void* arg, int32_t value);
bool vm_on_send_byte(VmStatus* status, void* arg, uint8_t value);

bool vm_on_trap_to_debugger(VmStatus* status, VmState* state, bool* continue_execution, void* debugger_arg);

bool vm_execute_operation(VmStatus* status, VmState* state, const VmOperation* operation, void* arg, void* debugger_arg);
