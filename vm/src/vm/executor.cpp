#include "executor.h"

#include "arg_type.h"

#include <stdio.h>
#include <inttypes.h>

#define READ(VARIABLE) \
	if (fread(&VARIABLE, sizeof(VARIABLE), 1, input_stream) < 1) { \
		return; \
	}

static void vm_process_exception() {
	while (true);
}

struct Machine {

	uint8_t memory[VM_MACHINE_MEMORY_SIZE];
	uint8_t code[VM_MACHINE_CODE_SIZE];

	int32_t registers[VM_MACHINE_NUM_REGISTERS];

	uint32_t ip;
};

static bool vm_check_arg(uint8_t command_number, uint8_t arg_type) {
	
}

bool read_arg(int32_t* arg_imm_const, int32_t** arg_memory, uint8_t arg_type, FILE* input_stream) {
	if (arg_type)
}

void vm_execute(FILE* input_stream, FILE* output_stream) {
	Machine machine = {};
	while (true) {
		uint8_t operation;
		READ(operation);

		uint8_t command_number = operation & ((1 << 5) - 1);
		uint8_t arg_type = operation >> 5;

		bool found_arg_type = false;
		#define COMMAND(NAME, NUMBER, ALLOWED_ARG_TYPES, ...) \
			if (command_number == NUMBER) {
				uint8_t allowed_arg_types[] = ALLOWED_ARG_TYPES;
				for (size_t i = 0; i < sizeof(allowed_arg_types) / sizeof(uint8_t); ++i) {
					if (allowed_arg_types[i] == arg_type) {
						found_arg_type = true;
						break;
					}
				}
			} else
		#include "arch/commands.h"
		#undef COMMAND
		{
			vm_process_exception();
		}

		if (!found_arg_type) {
			vm_process_exception();
		}

		int32_t arg_imm_const = 0;
		int32_t* arg_memory = NULL;

		read_arg(&arg_imm_const, &arg_memory, arg_type, input_stream);
		if ((arg_type & COMMAND_ARG_USES_REGISTER) != 0) {
			uint8_t register_index = READ(sizeof(uint8_t));
			if (register_index >= VM_MACHINE_NUM_REGISTERS) {
				vm_process_exception();
			}
			arg_imm_const = processor.registers[register_index];
			arg_memory = &processor.registers[register_index];
		}
		if ((arg_type & COMMAND_ARG_USES_IMMEDIATE_CONST) != 0) {
			int32_t immediate_const = READ(sizeof(int32_t));
			arg_imm_const += immediate_const;
			arg_memory = NULL;
		}
		if ((arg_type & COMMAND_ARG_USES_MEMORY) != 0) {
			if (arg_memory != NULL) {
				arg_imm_const = * arg_memory;
				arg_memory = (int32_t*) arg_memory;
			}
			arg_imm_const = (int32_t*) &processor.memory[arg_imm_const];
		}

	}
}