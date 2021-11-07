#include "assembly/argument.h"
#include "assembly/operation.h"
#include "status.h"
#include "support/forward_stream.h"
#include "vm/state.h"
#include "vm/vm.h"

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>

struct Debugger {
	uint32_t nops_to_skip;
	bool exit_requested;
};

static void print_operation(uint8_t* operation_ptr) {
	VmOperation operation = {};

	VmForwardStream stream = {};
	stream.bytes = operation_ptr;
	stream.length = sizeof(uint8_t) + sizeof(uint8_t) + sizeof(int32_t);
	stream.offset = 0;

	VmStatus status = VM_SUCCESS;

	if (!vm_bytecode_read_operation(&status, &stream, &operation)) {
		printf("Failed to decode the operation.\n");
		return;
	}

	VmForwardStream forward_stream = {};
	char buffer[1024] = {};
	forward_stream.bytes = (uint8_t*) buffer;
	forward_stream.offset = 0;
	forward_stream.length = 1024;
	
	VmAssemblyOperation assembly_operation = {};

	#define COMMAND(NAME, INDEX, ...)                  \
		if (operation.command_index == INDEX) {        \
			strcpy(assembly_operation.command, #NAME); \
		} else
	#include "commands.h"
	#undef command
	{
		printf("Failed to decode the operation.\n");
		return;
	}

	assembly_operation.argument.arg_type = operation.arg_type;
	assembly_operation.argument.register_index = operation.register_index;
	assembly_operation.argument.immediate_const.value = operation.immediate_const;
	
	if (!vm_text_write_operation(&status, &forward_stream, &assembly_operation)) {
		printf("Failed to decode the operation.\n");
		return;
	}

	fwrite(buffer, sizeof(*buffer), forward_stream.offset, stdout);
	fprintf(stdout, "\n");
	fflush(stdout);
}

static void read_commands(VmState* state, Debugger* debugger) {
	while (true) {
		printf("(db) ");
		fflush(stdout);

		char line[1023 + 1] = {};
		if (scanf(" %1023[^\r\n]", line) != 1) {
			printf("Failed to read the command.\n");
			fflush(stdout);
			continue;
		}

		int num_chars_read = 0;		
		
		num_chars_read = 0;		
		sscanf(line, " info stack %n", &num_chars_read);
		if (num_chars_read != 0) {
			stack_int_dump(&state->stack);
			fflush(stdout);
			continue;
		}

		num_chars_read = 0;		
		sscanf(line, " info ip %n", &num_chars_read);
		if (num_chars_read != 0) {
			printf("ip = %" PRId32 ".\n", state->ip);
			fflush(stdout);
			continue;
		}

		num_chars_read = 0;		
		sscanf(line, " info registers %n", &num_chars_read);
		if (num_chars_read != 0) {
			for (size_t i = 0; i <= VM_MAX_REGISTER_INDEX; ++i) {
				printf("%cx = %" SCNd32 "(0x%" SCNx32 ").\n", (char) ('a' + i), state->registers[i], (uint32_t) state->registers[i]);
			}
			fflush(stdout);
			continue;
		}

		char format_letter[2] = {};
		char size_letter[2] = {};
		int32_t repeat_count = 0;
		uint32_t address = 0;

		num_chars_read = 0;		
		// o(octal), x(hex), d(decimal), u(unsigned decimal), t(binary), f(float), a(address),
		// i(instruction), c(char), s(string) and z(hex, zero padded on the left).
		// b(byte), h(halfword), w(word), g(giant, 8 bytes).
		sscanf(line, " x /%" SCNd32 "%[oxdutfaicsz]%[bhwg] %" SCNu32 " %n", &repeat_count, format_letter, size_letter, &address, &num_chars_read);
		if (num_chars_read != 0) {
			if (format_letter[0] != 'x' || size_letter[0] != 'b' || repeat_count < 0) {
				printf("Unsupported arguments. Sorry.\n");
				fflush(stdout);
				continue;
			}

			for (size_t i = 0; i < (size_t) repeat_count; ++i) {
				printf("%02" PRIx8 " ", state->memory[i]);
			}
			printf("\n");
			fflush(stdout);
			continue;
		}

		num_chars_read = 0;		
		sscanf(line, " si %" SCNu32 "%n", &debugger->nops_to_skip, &num_chars_read);
		if (num_chars_read != 0) {
			break;
		}

		num_chars_read = 0;		
		sscanf(line, " si %n", &num_chars_read);
		if (num_chars_read != 0) {
			debugger->nops_to_skip = 1;
			break;
		}

		num_chars_read = 0;		
		sscanf(line, " q %n", &num_chars_read);
		if (num_chars_read != 0) {
			debugger->exit_requested = true;
			break;
		}

		printf("Unsupported command.\n");
		fflush(stdout);
	}
}

bool vm_on_trap_to_debugger(VmStatus* status, VmState* state, bool* continue_execution, void* debugger_arg) {
	assert(status != NULL);
	assert(state != NULL);
	assert(continue_execution != NULL);

	((void) status);

	Debugger* debugger = (Debugger*) debugger_arg;

	debugger->nops_to_skip = 0;

	printf("Debugee requested a trap.\n");

	print_operation(&state->memory[state->ip]);

	read_commands(state, debugger);

	if (debugger->exit_requested) {
		*continue_execution = false;
		return true;
	}

	return true;
}

bool vm_on_debugger_notified(VmStatus* status, VmState* state, bool* continue_execution, void* debugger_arg) {
	assert(status != NULL);
	assert(state != NULL);
	assert(continue_execution != NULL);

	((void) status);

	Debugger* debugger = (Debugger*) debugger_arg;

	print_operation(&state->memory[state->ip]);

	read_commands(state, debugger);

	if (debugger->exit_requested) {
		*continue_execution = false;
		return true;
	}

	if (debugger->nops_to_skip > 0) {
		debugger->nops_to_skip -= 1;
		return true;
	}

	return true;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		return 1;
	}
	const char* input_file = argv[1];

	FILE* input_stream = fopen(input_file, "rb");
	if (input_stream == NULL) {
		return 4;
	}

	Debugger debugger = {};
	debugger.nops_to_skip = 0;
	debugger.exit_requested = false;

	vm_execute(input_stream, stdin, stdout, &debugger);

	// return (int) CLI_EXIT_CODE_SUCCESS;
	return 0;
}