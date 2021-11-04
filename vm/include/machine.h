#pragma once

#include "stack.h"

#include <stddef.h>
#include <stdint.h>

static const size_t VM_MEMORY_SIZE = 64 * 1024 * 1024;
static const size_t VM_NUMBER_OF_REGISTERS = 3;

struct Machine {
	uint8_t memory[VM_MEMORY_SIZE];
	int32_t registers[VM_NUMBER_OF_REGISTERS];
	int32_t ip;
	StackInt stack;
};
