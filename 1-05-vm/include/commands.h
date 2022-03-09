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
		#define STACK_PUSH_FLOAT(VALUE) stack_int_push() // VERIFY!
		#define OPERAND(NAME) int32_t NAME = 0; STACK_POP(&NAME)
		#define OPERAND_FLOAT(NAME) int32_t int_ ## NAME = 0; STACK_POP(&NAME); float NAME = * (float*) & int_ ## NAME;
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
		#define READF(VARIABLE) float VARIABLE = 0; scanf("%f", &VARIABLE)
		#define SQRT(VALUE) (int32_t) sqrt((double) (VALUE))
		#define SEND_STRING(VALUE) puts(VALUE)
		#define SEND_FLOAT(VALUE) printf("%f", VALUE)
		#define FLOATING_POINT_COMMANDS(...)
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
		STACK_PUSH_4B(ARGUMENT_EVAL);
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
		STACK_POP_4B(ARGUMENT_MEMORY);
	),
)

COMMAND(
	repush,
	4,
	ARRAY_DEF(
		VM_COMMAND_ARG_USES_IMMEDIATE_CONST
	),
	CODE(
		STACK_REPUSH(ARGUMENT_EVAL);
	),
)

COMMAND(
	readi,
	5,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		READ_INT(value);
		STACK_PUSH_INT(value);
	),
)

COMMAND(
	readf,
	6,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		READ_FLOAT(value);
		STACK_PUSH_FLOAT(value);
	),
)

COMMAND(
	readb,
	7,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		READ_BYTE(value);
		STACK_PUSH_INT(value);
	),
)

COMMAND(
	writei,
	8,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND_INT(value);
		SEND_INT(value);
	),
)

COMMAND(
	writef,
	9,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND_FLOAT(value);
		SEND_FLOAT(value);
	),
)

COMMAND(
	writeb,
	10,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND_INT(value);
		SEND_BYTE(value & 0xFF);
	),
)

COMMAND(
	addi,
	11,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND_INT(lhs);
		OPERAND_INT(rhs);
		STACK_PUSH_INT(lhs + rhs);
	),
)
COMMAND(
	subi,
	12,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND_INT(lhs);
		OPERAND_INT(rhs);
		STACK_PUSH_INT(lhs - rhs);
	),
)
COMMAND(
	muli,
	13,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND_INT(lhs);
		OPERAND_INT(rhs);
		STACK_PUSH_INT(lhs * rhs);
	),
)
COMMAND(
	divi,
	14,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND_INT(lhs);
		OPERAND_INT(rhs);
		STACK_PUSH_INT(lhs / rhs);
	),
)

COMMAND(
	addf,
	15,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND_FLOAT(lhs);
		OPERAND_FLOAT(rhs);
		STACK_PUSH_FLOAT(lhs + rhs);
	),
)
COMMAND(
	subf,
	16,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND_FLOAT(lhs);
		OPERAND_FLOAT(rhs);
		STACK_PUSH_FLOAT(lhs - rhs);
	),
)
COMMAND(
	mulf,
	17,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND_FLOAT(lhs);
		OPERAND_FLOAT(rhs);
		STACK_PUSH_FLOAT(lhs * rhs);
	),
)
COMMAND(
	divf,
	18,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND_FLOAT(lhs);
		OPERAND_FLOAT(rhs);
		STACK_PUSH_FLOAT(lhs / rhs);
	),
)

COMMAND(
	sqrtf,
	19,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND_FLOAT(value);
		STACK_PUSH_FLOAT(SQRT(value));
	),
)

/* COMMAND(
	shr,
	12,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND_4B(lhs);
		OPERAND_4B(rhs);
		STACK_PUSH_4B(lhs >> rhs);
	),
)
COMMAND(
	shl,
	13,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND_4B(lhs);
		OPERAND_4B(rhs);
		STACK_PUSH_4B(lhs << rhs);
	),
)*/
COMMAND(
	and,
	20,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND_4B(lhs);
		OPERAND_4B(rhs);
		STACK_PUSH_4B(lhs & rhs);
	),
)
COMMAND(
	or,
	21,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND_4B(lhs);
		OPERAND_4B(rhs);
		STACK_PUSH_4B(lhs | rhs);
	),
)/*
COMMAND(
	xor,
	16,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND_4B(lhs);
		OPERAND_4B(rhs);
		STACK_PUSH_4B(lhs ^ rhs);
	),
)*/

COMMAND(
	test,
	22,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND_4B(value);
		OPERAND_4B(bit);
		STACK_PUSH_4B(TEST_BIT(value, bit));
	),
)

COMMAND(
	jmp,
	23,
	ARRAY_DEF(VM_COMMAND_ARG_USES_IMMEDIATE_CONST, VM_COMMAND_ARG_USES_REGISTER),
	CODE(
		SET_IP(ARGUMENT_EVAL);
	),
)

COMMAND(
	jaei,
	24,
	ARRAY_DEF(VM_COMMAND_ARG_USES_IMMEDIATE_CONST, VM_COMMAND_ARG_USES_REGISTER),
	CODE(
		OPERAND_INT(lhs);
		OPERAND_INT(rhs);
		IF(lhs >= rhs)
			SET_IP(ARGUMENT_EVAL);
		ENDIF()
	),
)

COMMAND(
	jbei,
	25,
	ARRAY_DEF(VM_COMMAND_ARG_USES_IMMEDIATE_CONST, VM_COMMAND_ARG_USES_REGISTER),
	CODE(
		OPERAND_INT(lhs);
		OPERAND_INT(rhs);
		IF(lhs <= rhs)
			SET_IP(ARGUMENT_EVAL);
		ENDIF()
	),
)

COMMAND(
	jaef,
	26,
	ARRAY_DEF(VM_COMMAND_ARG_USES_IMMEDIATE_CONST, VM_COMMAND_ARG_USES_REGISTER),
	CODE(
		OPERAND_FLOAT(lhs);
		OPERAND_FLOAT(rhs);
		IF(lhs >= rhs)
			SET_IP(ARGUMENT_EVAL);
		ENDIF()
	),
)

COMMAND(
	jbef,
	27,
	ARRAY_DEF(VM_COMMAND_ARG_USES_IMMEDIATE_CONST, VM_COMMAND_ARG_USES_REGISTER),
	CODE(
		OPERAND_FLOAT(lhs);
		OPERAND_FLOAT(rhs);
		IF(lhs <= rhs)
			SET_IP(ARGUMENT_EVAL);
		ENDIF()
	),
)

COMMAND(
	je,
	28,
	ARRAY_DEF(VM_COMMAND_ARG_USES_IMMEDIATE_CONST, VM_COMMAND_ARG_USES_REGISTER),
	CODE(
		OPERAND_4B(lhs);
		OPERAND_4B(rhs);
		IF(lhs == rhs)
			SET_IP(ARGUMENT_EVAL);
		ENDIF()
	),
)

COMMAND(
	jne,
	29,
	ARRAY_DEF(VM_COMMAND_ARG_USES_IMMEDIATE_CONST, VM_COMMAND_ARG_USES_REGISTER),
	CODE(
		OPERAND_4B(lhs);
		OPERAND_4B(rhs);
		IF(lhs != rhs)
			SET_IP(ARGUMENT_EVAL);
		ENDIF()
	),
)

COMMAND(
	call,
	30,
	ARRAY_DEF(VM_COMMAND_ARG_USES_IMMEDIATE_CONST, VM_COMMAND_ARG_USES_REGISTER),
	CODE(
		STACK_PUSH_4B(GET_IP());
		SET_IP(ARGUMENT_EVAL);
	),
)

COMMAND(
	ret,
	31,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		OPERAND_4B(value);
		SET_IP(value);
	),
)



