#include "vm/vm.h"

#include "arg_type.h"

#include "stack.h"

#include <stdio.h>
#include <inttypes.h>

static bool vm_read_program(FILE* program, Machine* machine) {
	assert(program != nullptr);
	assert(machine != nullptr);

	fread(machine->memory, sizeof(*machine->memory), sizeof(machine->memory) / sizeof(*machine->memory), program);

	if (ferror(program)) {
		return false;
	}

	if (!feof(program) && fgetc(program) != EOF) {
		return false;
	}

	return true;
}

bool vm_execute(FILE* program, FILE* input_stream, FILE* output_stream) {
	assert(program != nullptr);
	assert(input_stream != nullptr);
	assert(output_stream != nullptr);
	
	Machine machine = {};

	stack_int_init(&machine->stack);
	
	vm_read_program(program, &machine);

	VmStatus status = VM_SUCCESS;

	while (true) {

		VmOperation operation = {};

		VmForwardStream stream = {};
		stream.bytes = &machine->memory[machine->ip];
		stream.length = sizeof(machine->memory) / sizeof(*machine->memory) - machine->ip;
		stream.offset = 0;

		vm_bytecode_read_operation(&status, &stream, &operation);

		int32_t* argument_memory = nullptr;
		int32_t argument = 0;

		if ((operation.arg_type & COMMAND_ARG_USES_REGISTER) != 0) {
			uint8_t register_index = READ(sizeof(uint8_t));
			if (register_index >= VM_MACHINE_NUM_REGISTERS) {
				vm_process_exception();
			}
			argument = machine.registers[register_index];
			argument_memory = &machine.registers[register_index];
		}
		if ((arg_type & COMMAND_ARG_USES_IMMEDIATE_CONST) != 0) {
			int32_t immediate_const = READ(sizeof(int32_t));
			argument += immediate_const;
			arg_memory = NULL;
		}
		if ((arg_type & COMMAND_ARG_USES_MEMORY) != 0) {
			if (argument_memory == NULL) {
				argument_memory = (int32_t*) &processor.memory[argument];
				argument = *argument_memory;
			}
		}

		#define ARG_MEMORY argument_memory
		#define ARG argument
		#define MEMORY(ADDRESS) (int32_t*) &machine.memory[ADDRESS]
		#define REGISTER(INDEX) (int32_t*) &machine.registers[INDEX]
		#define STACK_POP(INDEX) () // VERIFY!
		#define STACK_PUSH(VALUE) stack_int_push(&machine->stack, ) // VERIFY!
		#define SEND_INT(VALUE) fprintf(output_stream, "%" PRNd32, VALUE);
		#define SEND_BYTE(VALUE) fputchar(output_stream, VALUE)
		#define COMMAND(NAME, INDEX, ALLOWED_ARG_TYPES, EXECUTION_CODE, ...) \
			if (operation.command_index == INDEX) {
				EXECUTION_CODE;
			} else
		#include "commands.h"
		#undef COMMAND
		{

		}

	}

	stack_int_deinit(&machine->stack);
}