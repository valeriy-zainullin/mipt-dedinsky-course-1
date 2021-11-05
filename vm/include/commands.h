// #define COMMAND(NAME, NUMBER, ALLOWED_ARG_TYPES...)
// Arg type is 3 bits: uses memory, uses register, uses immediate const.

#include "arg_type.h"

#include "support/macro_utils.h"

// Don't count code as separate arguments.
#define CODE(...) __VA_ARGS__

/*
		#define ARGUMENT_MEMORY argument_memory
		#define ARGUMENT argument
		#define MEMORY(ADDRESS) (int32_t*) &machine.memory[ADDRESS]
		#define REGISTER(INDEX) (int32_t*) &machine.registers[INDEX]
		#define STACK_POP(INDEX) // VERIFY!
		#define STACK_PUSH(VALUE) stack_int_push() // VERIFY!
		#define SEND_INT(VALUE) fprintf(output_stream, "%" PRNd32, VALUE);
		#define SEND_BYTE(VALUE) fputchar(output_stream, VALUE)
		#define HALT() break
		#define TMP_VARIABLE(variable, value) (variable = value);

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
	push,
	1,
	ARRAY_DEF(
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_REGISTER | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		VM_COMMAND_ARG_USES_MEMORY |                                VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_REGISTER                                      ,
		                             VM_COMMAND_ARG_USES_REGISTER | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		                                                            VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		                             VM_COMMAND_ARG_USES_REGISTER
	),
	(
		STACK_PUSH(ARGUMENT);
	),
)

COMMAND(
	pop,
	2,
	ARRAY_DEF(
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_REGISTER | VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		VM_COMMAND_ARG_USES_MEMORY |                                VM_COMMAND_ARG_USES_IMMEDIATE_CONST,
		VM_COMMAND_ARG_USES_MEMORY | VM_COMMAND_ARG_USES_REGISTER                                      ,
		                             VM_COMMAND_ARG_USES_REGISTER
	),
	CODE(
		*ARGUMENT_MEMORY = STACK_POP();
	),
)

COMMAND(
	add,
	3,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		TMP_VARIABLE(lhs, STACK_POP());
		TMP_VARIABLE(rhs, STACK_POP());
		STACK_PUSH(lhs + rhs);
	),
)
COMMAND(
	sub,
	4,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		TMP_VARIABLE(lhs, STACK_POP());
		TMP_VARIABLE(rhs, STACK_POP());
		STACK_PUSH(lhs - rhs);
	),
)
COMMAND(
	mul,
	5,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		TMP_VARIABLE(lhs, STACK_POP());
		TMP_VARIABLE(rhs, STACK_POP());
		STACK_PUSH(lhs * rhs);
	),
)
COMMAND(
	div,
	6,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		TMP_VARIABLE(lhs, STACK_POP());
		TMP_VARIABLE(rhs, STACK_POP());
		STACK_PUSH(lhs / rhs);
	),
)

COMMAND(
	shr,
	7,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		TMP_VARIABLE(lhs, STACK_POP());
		TMP_VARIABLE(rhs, STACK_POP());
		STACK_PUSH(lhs >> rhs);
	),
)
COMMAND(
	shl,
	8,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		TMP_VARIABLE(lhs, STACK_POP());
		TMP_VARIABLE(rhs, STACK_POP());
		STACK_PUSH(lhs << rhs);
	),
)
COMMAND(
	and,
	9,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	CODE(
		TMP_VARIABLE(lhs, STACK_POP());
		TMP_VARIABLE(rhs, STACK_POP());
		STACK_PUSH(lhs & rhs);
	),
)
COMMAND(
	or,
	10,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	(
		TMP_VARIABLE(lhs, STACK_POP());
		TMP_VARIABLE(rhs, STACK_POP());
		STACK_PUSH(lhs | rhs);
	),
)
COMMAND(
	xor,
	11,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	(
		TMP_VARIABLE(lhs, STACK_POP());
		TMP_VARIABLE(rhs, STACK_POP());
		STACK_PUSH(lhs ^ rhs);
	),
)

COMMAND(
	out,
	12,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	(
		SEND_INT(STACK_POP());
	),
)

COMMAND(
	outb,
	13,
	ARRAY_DEF(VM_COMMAND_ARG_NOT_PRESENT),
	(
		SEND_BYTE(STACK_POP() & 0xFF),
	),
)

#undef EXECUTION_CODE
