#include "vm/vm.h"

#include "arg_type.h"
#include "bytecode/operation.h"
#include "status.h"
#include "vm/state.h"

#include "stack_vm.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

static bool vm_read_program(FILE* program, VMState* state) {
	assert(program != NULL);
	assert(state != NULL);

	fread(state->memory, sizeof(*state->memory), sizeof(state->memory) / sizeof(*state->memory), program);

	if (ferror(program)) {
		return false;
	}

	if (!feof(program) && fgetc(program) != EOF) {
		return false;
	}

	return true;
}

bool vm_on_send_int(VMStatus* status, void* arg, int32_t value) {
	(void) status;

	FILE* output_stream = (FILE*) arg;
	fprintf(output_stream, "%" PRId32, value);
	fflush(stdout);

	return true;
}

bool vm_on_send_byte(VMStatus* status, void* arg, uint8_t value) {
	(void) status;

	FILE* output_stream = (FILE*) arg;
	fwrite(&value, sizeof(value), 1, output_stream);
	fflush(stdout);

	return true;
}

void vm_execute(FILE* program, FILE* input_stream, FILE* output_stream, void* debugger_arg) {
	assert(program != NULL);
	assert(input_stream != NULL);
	assert(output_stream != NULL);

	VMState* state = (VMState*) calloc(1, sizeof(VMState));
	if (state == NULL) {
		return;
	}

	stack_4b_init(&state->stack, state->memory + VM_MACHINE_MEMORY_SIZE - VM_MACHINE_STACK_SIZE, VM_MACHINE_STACK_SIZE / sizeof(uint32_t));
	state->registers[VM_MACHINE_STACK_REGISTER_INDEX] = (uint32_t) ((uint8_t*) stack_4b_get_content_address(&state->stack) - state->memory);

	vm_read_program(program, state);

	VMStatus status = VM_SUCCESS;

	while (true) {

		bool continue_execution = true;
		vm_on_debugger_notified(&status, state, &continue_execution, debugger_arg);
		if (!continue_execution) {
			break;
		}

		VMOperation operation = {};

		VMForwardStream stream = {};
		stream.bytes = &state->memory[state->ip];
		stream.length = sizeof(state->memory) / sizeof(*state->memory) - state->ip;
		stream.offset = 0;

		if (!vm_bytecode_read_operation(&status, &stream, &operation)) {
			break;
		}

		assert(stream.offset <= INT32_MAX);
		state->ip += (int32_t) stream.offset;

		if (!vm_execute_operation(&status, state, &operation, output_stream, debugger_arg)) {
			break;
		}

		if (status == VM_STATUS_HALT_REQUESTED) {
			break;
		}

	}

	stack_4b_deinit(&state->stack);
	free(state);
}