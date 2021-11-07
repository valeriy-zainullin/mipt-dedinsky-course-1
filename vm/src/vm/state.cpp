#include "vm/state.h"

#include "arg_type.h"
#include "assembly/operation.h"
#include "bytecode/operation.h"
#include "status.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool vm_execute_operation(VmStatus* status, VmState* state, const VmOperation* operation, void* arg, void* debugger_arg) {
	assert(status != NULL);
	assert(state != NULL);
	assert(operation != NULL);

	int32_t argument_eval = 0; // evaluated if const or const + register or read if memory
	int32_t* argument_memory = NULL; // Write

	if ((operation->arg_type & VM_COMMAND_ARG_USES_REGISTER) != 0) {
		uint8_t register_index = operation->register_index;
		if (register_index >= VM_MACHINE_NUM_REGISTERS) {
			return false;
		}
		argument_eval = state->registers[register_index];
		argument_memory = &state->registers[register_index];
	}
	if ((operation->arg_type & VM_COMMAND_ARG_USES_IMMEDIATE_CONST) != 0) {
		int32_t immediate_const = operation->immediate_const;
		argument_eval += immediate_const;
		argument_memory = NULL;
	}
	if ((operation->arg_type & VM_COMMAND_ARG_USES_MEMORY) != 0) {
		if (argument_memory == NULL) {
			argument_memory = (int32_t*) &state->memory[argument_eval];
			argument_eval = *argument_memory;
		}
	}

	#define ARGUMENT_EVAL argument_eval
	#define ARGUMENT_MEMORY argument_memory
	#define MEMORY(ADDRESS) (int32_t*) &state->memory[ADDRESS]
	#define REGISTER(INDEX) (int32_t*) &state->registers[INDEX]
	#define STACK_POP(VARIABLE_PTR) if (!stack_int_pop(&state->stack, VARIABLE_PTR)) { return false; }
	#define STACK_PUSH(VALUE) if (!stack_int_push(&state->stack, VALUE)) { return false; }
	#define STACK_PUSH_FLOAT(VALUE) if (!stack_int_push(&state->stack, * (float*) &VALUE)) { return false; }
	#define OPERAND(NAME) int32_t NAME = 0; STACK_POP(&NAME)
	#define OPERAND_FLOAT(NAME) int32_t int_ ## NAME = 0; STACK_POP(&NAME); float NAME = * (float*) & int_ ## NAME;
	#define SET_IP(VALUE) state->ip = VALUE
	#define GET_IP(VALUE) state->ip
	#define IF(EXPR) if (EXPR) {
	#define ENDIF() }
	#define SEND_INT(VALUE) if (!vm_on_send_int(status, arg, VALUE)) { return false; };
	#define SEND_BYTE(VALUE) if (!vm_on_send_byte(status, arg, (uint8_t) VALUE)) { return false; };
	#define HALT() *status = VM_STATUS_HALT_REQUESTED; return true
	#define TRAP()                                                                       \
		bool continue_execution = true;                                                  \
		if (!vm_on_trap_to_debugger(status, state, &continue_execution, debugger_arg)) { \
			return false;                                                                \
		}                                                                                \
		if (!continue_execution) {                                                       \
			*status = VM_STATUS_HALT_REQUESTED;                                          \
			return true;                                                                 \
		}
	#define READ(VARIABLE) int32_t VARIABLE = 0; scanf("%" SCNd32, &VARIABLE)
	#define READ_FLOAT(VARIABLE) float VARIABLE = 0; scanf("%f", &VARIABLE)
	#define SQRT(VALUE) (int32_t) sqrt((double) (VALUE))
	#define SEND_STRING(VALUE) puts((char*) &state->memory[VALUE]); fflush(stdout)
	#define SEND_FLOAT(VALUE) printf("%f", VALUE); fflush(stdout);
	#define COMMAND(NAME, INDEX, ALLOWED_ARG_TYPES, EXECUTION_CODE, ...) \
		if (operation->command_index == INDEX) {                         \
			EXECUTION_CODE                                               \
		} else
	#include "commands.h"
	#undef COMMAND
	{
		return false;
	}

	return true;
}
