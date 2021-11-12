#pragma once

#include "bytecode/operation.h"
#include "status.h"

#include "stack_vm.h"

#include <stddef.h>
#include <stdint.h>

static const size_t VM_MACHINE_MEMORY_SIZE = 64 * 1024 * 1024;
static const size_t VM_MACHINE_STACK_SIZE = 4 * 1024 * 1024;
static const size_t VM_MACHINE_NUM_REGISTERS = 4;
static const size_t VM_MACHINE_STACK_REGISTER_INDEX = VM_MACHINE_NUM_REGISTERS - 1;

static const size_t VM_MACHINE_VIDEO_MEMORY_SIZE = 3 * 1024 * 768;
static const size_t VM_MACHINE_TRUE_MEMORY_SIZE = VM_MACHINE_MEMORY_SIZE - VM_MACHINE_VIDEO_MEMORY_SIZE;

struct VMState {
	uint8_t memory[VM_MACHINE_MEMORY_SIZE];
	int32_t registers[VM_MACHINE_NUM_REGISTERS];
	int32_t ip;
	Stack4b stack;
};

bool vm_on_send_int(VMStatus* status, void* arg, int32_t value);
bool vm_on_send_byte(VMStatus* status, void* arg, uint8_t value);

bool vm_on_trap_to_debugger(VMStatus* status, VMState* state, bool* continue_execution, void* debugger_arg);

bool vm_execute_operation(VMStatus* status, VMState* state, const VMOperation* operation, void* arg, void* debugger_arg);
