#include "executor.h"

#include "arg_type.h"

#include <stdio.h>

#define READ(VARIABLE) \
	if (fread(&VARIABLE, sizeof(VARIABLE), 1, input_stream) < 1) { \
		return; \
	}

static void vm_process_exception() {
	while (true);
}

void vm_execute(FILE* input_stream, FILE* output_stream) {
	while (true) {
		uint8_t operation;
		READ(operation);

		uint8_t arg_type = operation >> 5;
		uint8_t command_number = operation & ((1 << 5) - 1);

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

		uint8_t* 
		if (arg_type & (COMMAND_ARG_USES_))

	}
}