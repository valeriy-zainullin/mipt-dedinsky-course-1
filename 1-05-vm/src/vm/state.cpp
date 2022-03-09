#include "vm/state.h"

#include "arg_type.h"
#include "assembly/operation.h"
#include "bytecode/operation.h"
#include "status.h"

#include "vector_vm.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool vm_execute_operation(VMStatus* status, VMState* state, const VMOperation* operation, void* arg, void* debugger_arg) {
	assert(status != NULL);
	assert(state != NULL);
	assert(operation != NULL);

	uint32_t argument_eval = 0; // evaluated if const or const + register or read if memory
	uint32_t* argument_memory = NULL; // Write

	if ((operation->arg_type & VM_COMMAND_ARG_USES_REGISTER) != 0) {
		uint8_t register_index = operation->register_index;
		if (register_index >= VM_MACHINE_NUM_REGISTERS) {
			return false;
		}
		argument_eval = state->registers[register_index];
		argument_memory = &state->registers[register_index];
	}
	if ((operation->arg_type & VM_COMMAND_ARG_USES_IMMEDIATE_CONST) != 0) {
		uint32_t immediate_const = operation->immediate_const;
		argument_eval += immediate_const;
		argument_memory = NULL;
	}
	if ((operation->arg_type & VM_COMMAND_ARG_USES_MEMORY) != 0) {
		if (argument_memory == NULL) {
			argument_memory = (uint32_t*) &state->memory[argument_eval];
			argument_eval = *argument_memory;
		}
	}

	#define ARGUMENT_EVAL   argument_eval
	#define ARGUMENT_MEMORY argument_memory
	#define MEMORY(ADDRESS) (int32_t*) &state->memory[ADDRESS]
	#define REGISTER(INDEX) (int32_t*) &state->registers[INDEX]
	
	#define STACK_PUSH_ANY(VARIABLE_PTR) {                 \
		uint32_t tmp_copy = 0;                             \
		memcpy(&tmp_copy, VARIABLE_PTR, sizeof(uint32_t)); \
		if (!vector_4b_push(&state->stack, tmp_copy)) {    \
			return false;                                  \
		}                                                  \
	}
	#define STACK_POP_ANY(VARIABLE_PTR)                                \
		if (!vector_4b_pop(&state->stack, (uint32_t*) VARIABLE_PTR)) { \
			return false;                                              \
		}
	
	#define STACK_PUSH_4B(VALUE) {  \
		uint32_t tmp_value = VALUE; \
		STACK_PUSH_ANY(&tmp_value); \
	}
	#define STACK_POP_4B(VARIABLE_PTR) { \
		STACK_POP_ANY(VARIABLE_PTR); \
	}
	
	#define STACK_PUSH_INT(VALUE) { \
		int32_t tmp_value = VALUE;  \
		STACK_PUSH_ANY(&tmp_value); \
	}
	#define STACK_POP_INT(VARIABLE_PTR) STACK_POP_ANY(VARIABLE_PTR)
	
	#define STACK_PUSH_FLOAT(VALUE) { \
		float tmp_value = VALUE;      \
		STACK_PUSH_ANY(&tmp_value);   \
	}
	#define STACK_POP_FLOAT(VARIABLE_PTR) STACK_POP_ANY(VARIABLE_PTR)
	
	#define STACK_REPUSH(INDEX) STACK_PUSH_4B(*vector_4b_get_item(&state->stack, vector_4b_get_length(&state->stack) - INDEX))
	
	#define OPERAND_4B(NAME)    uint32_t NAME = 0; STACK_POP_4B(&NAME)
	#define OPERAND_INT(NAME)   int32_t NAME = 0; STACK_POP_INT(&NAME)
	#define OPERAND_FLOAT(NAME) float NAME = 0; STACK_POP_FLOAT(&NAME)
	
	#define TEST_BIT(VALUE, BIT) (value & (((uint32_t) 1) << BIT)) != 0
	
	#define SET_IP(VALUE) state->ip = VALUE
	#define GET_IP(VALUE) state->ip
	
	#define IF(EXPR) if (EXPR) {
	#define ENDIF()  }
	
	#define READ_INT(VARIABLE)    int32_t VARIABLE = 0; scanf("%" SCNd32, &VARIABLE)
	#define READ_FLOAT(VARIABLE)  float VARIABLE = 0; scanf("%f", &VARIABLE)
	#define READ_BYTE(VARIABLE)   int32_t VARIABLE = getchar()
	#define READ_STRING(VARIABLE) int32_t VARIABLE = 0; scanf("%" SCNd32, &VARIABLE)

	#define SEND_INT(VALUE)       if (!vm_on_send_int(status, arg, VALUE)) { return false; }
	#define SEND_FLOAT(VALUE)     printf("%f", VALUE); fflush(stdout)
	#define SEND_BYTE(VALUE)      if (!vm_on_send_byte(status, arg, (uint8_t) VALUE)) { return false; }
	#define SEND_STRING(VALUE)    puts((char*) &state->memory[VALUE]); fflush(stdout)
	
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
	
	#define SQRT(VALUE) sqrtf(VALUE)
	
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
