// #define COMMAND(NAME, NUMBER, ALLOWED_ARG_TYPES...)
// Arg type is 3 bits: uses memory, uses register, uses immediate const.

#include "arg_type.h"

#include "support/macro_utils.h"

// Don't count code as separate arguments.
#define CODE(...) __VA_ARGS__

/*
		#define ARGUMENT_EVAL argument
		#define ARGUMENT_MEMORY argument_memory
		#define MEMORY(ADDRESS) (int32_t*) &machine.memory[ADDRESS]
		#define REGISTER(INDEX) (int32_t*) &machine.registers[INDEX]
		#define STACK_POP(INDEX) // VERIFY!
		#define STACK_PUSH(VALUE) stack_int_push() // VERIFY!
		#define OPERAND(NAME) int32_t NAME = 0; STACK_POP(&NAME)
		#define SET_IP(VALUE) state->ip = VALUE
		#define GET_IP(VALUE) state->ip = VALUE
		#define IF(EXPR) if (EXPR) {
		#define ENDIF() }
		#define SEND_INT(VALUE) fprintf(output_stream, "%" PRNd32, VALUE);
		#define SEND_BYTE(VALUE) fputchar(output_stream, VALUE)
		#define TRAP() vm_trap_to_debugger(status, state)
		#define HALT() break
		#define TMP_VARIABLE(variable, value) (variable = value);
		#define READ(VARIABLE) int32_t VARIABLE = 0; scanf("%" SCNd32, &VARIABLE)
		#define SQRT(VALUE) (int32_t) sqrt((double) (VALUE))
		#define SEND_STRING(VALUE) puts(VALUE)

*/

COMMAND(
	halt,
	0,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		HALT();
	),
)

COMMAND(
	trap,
	1,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		TRAP();
	),
)

COMMAND(
	push,
	2,
	ARRAY_DEF(
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_REGISTER | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		VM_COMMAND_ARG_USES_MEMORY |                                VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_REGISTER                                      ,
		                             VM_COMMAND_ARG_USES_REGISTER | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		                                                            VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		                             VM_COMMAND_ARG_USES_REGISTER
	),
	CODE(
		STACK_PUSH(ARGUMENT_EVAL);
	),
)

COMMAND(
	pop,
	3,
	ARRAY_DEF(
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_REGISTER | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		VM_COMMAND_ARG_USES_MEMORY |                                VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_REGISTER                                      ,
		                             VM_COMMAND_ARG_USES_REGISTER
	),
	CODE(
		STACK_POP(ARGUMENT_MEMORY);
	),
)

COMMAND(
	add,
	4,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND(lhs);
		OPERAND(rhs);
		STACK_PUSH(lhs + rhs);
	),
)
COMMAND(
	sub,
	5,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND(lhs);
		OPERAND(rhs);
		STACK_PUSH(lhs - rhs);
	),
)
COMMAND(
	mul,
	6,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND(lhs);
		OPERAND(rhs);
		STACK_PUSH(lhs * rhs);
	),
)
COMMAND(
	div,
	7,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND(lhs);
		OPERAND(rhs);
		STACK_PUSH(lhs / rhs);
	),
)

COMMAND(
	shr,
	8,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND(lhs);
		OPERAND(rhs);
		STACK_PUSH(lhs >> rhs);
	),
)
COMMAND(
	shl,
	9,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND(lhs);
		OPERAND(rhs);
		STACK_PUSH(lhs << rhs);
	),
)
COMMAND(
	and,
	10,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND(lhs);
		OPERAND(rhs);
		STACK_PUSH(lhs & rhs);
	),
)
COMMAND(
	or,
	11,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND(lhs);
		OPERAND(rhs);
		STACK_PUSH(lhs | rhs);
	),
)
COMMAND(
	xor,
	12,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND(lhs);
		OPERAND(rhs);
		STACK_PUSH(lhs ^ rhs);
	),
)

COMMAND(
	out,
	13,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND(value);
		SEND_INT(value);
	),
)

COMMAND(
	outb,
	14,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND(value);
		SEND_BYTE(value & 0xFF);
	),
)

COMMAND(
	jmp,
	15,
	ARRAY_DEF(VM_COMMAND_ARG_USES_IMMEDIATE_CONST, VM_COMMAND_ARG_USES_REGISTER),
	CODE(
		SET_IP(ARGUMENT_EVAL);
	),
)

COMMAND(
	jae,
	16,
	ARRAY_DEF(VM_COMMAND_ARG_USES_IMMEDIATE_CONST, VM_COMMAND_ARG_USES_REGISTER),
	CODE(
		OPERAND(lhs);
		OPERAND(rhs);
		IF(lhs >= rhs)
			SET_IP(ARGUMENT_EVAL);
		ENDIF()
	),
)

COMMAND(
	jbe,
	17,
	ARRAY_DEF(VM_COMMAND_ARG_USES_IMMEDIATE_CONST, VM_COMMAND_ARG_USES_REGISTER),
	CODE(
		OPERAND(lhs);
		OPERAND(rhs);
		IF(lhs <= rhs)
			SET_IP(ARGUMENT_EVAL);
		ENDIF()
	),
)

COMMAND(
	je,
	18,
	ARRAY_DEF(VM_COMMAND_ARG_USES_IMMEDIATE_CONST, VM_COMMAND_ARG_USES_REGISTER),
	CODE(
		OPERAND(lhs);
		OPERAND(rhs);
		IF(lhs == rhs)
			SET_IP(ARGUMENT_EVAL);
		ENDIF()
	),
)

COMMAND(
	jne,
	19,
	ARRAY_DEF(VM_COMMAND_ARG_USES_IMMEDIATE_CONST, VM_COMMAND_ARG_USES_REGISTER),
	CODE(
		OPERAND(lhs);
		OPERAND(rhs);
		IF(lhs != rhs)
			SET_IP(ARGUMENT_EVAL);
		ENDIF()
	),
)

COMMAND(
	call,
	20,
	ARRAY_DEF(VM_COMMAND_ARG_USES_IMMEDIATE_CONST, VM_COMMAND_ARG_USES_REGISTER),
	CODE(
		STACK_PUSH(GET_IP());
		SET_IP(ARGUMENT_EVAL);
	),
)

COMMAND(
	ret,
	21,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND(value);
		SET_IP(value);
	),
)

COMMAND(
	read,
	22,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		READ(value);
		STACK_PUSH(value);
	),
)

COMMAND(
	sqrt,
	23,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND(value);
		STACK_PUSH(SQRT(value));
	),
)

COMMAND(
	outs,
	24,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND(value);
		SEND_STRING(value);
	),
)
