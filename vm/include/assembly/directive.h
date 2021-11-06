#pragma once

#include "status.h"
#include "support/forward_stream.h"

#include <stddef.h>

#define VM_ASSEMBLY_DIRECTIVE_SCANF_FORMAT "%8s"
static const size_t VM_ASSEMBLY_MAX_DIRECTIVE_LENGTH = 8;

static const char VM_ASSEMBLY_DIRECTIVE_PREFIX = '.';

static const char VM_ASSEMBLY_DIRECTIVES[][VM_ASSEMBLY_MAX_DIRECTIVE_LENGTH + 1] = {"db"};
static const size_t VM_ASSEMBLY_DIRECTIVE_MAX_NUMBER_OF_ARGUMENTS = 10;
static const size_t VM_ASSEMBLY_DIRECTIVE_MAX_STRING_ARG_LENGTH = 256;

enum VmAssemblyDirectiveType {
	VM_ASSEMBLY_DIRECTIVE_ARG_STRING,
	VM_ASSEMBLY_DIRECTIVE_ARG_NUMBER
};

struct VmAssemblyDirectiveArgument {
	VmAssemblyDirectiveType argument_type;

	char string[VM_ASSEMBLY_DIRECTIVE_MAX_STRING_ARG_LENGTH + 1];
	size_t string_length;

	uint8_t byte;
	int32_t number;
};

struct VmAssemblyDirective {
	char name[VM_ASSEMBLY_MAX_DIRECTIVE_LENGTH + 1];
	VmAssemblyDirectiveArgument arguments[VM_ASSEMBLY_DIRECTIVE_MAX_NUMBER_OF_ARGUMENTS];
	size_t num_arguments;
};

// Directive = '.' directive_name ' ' {' '} {argument}
// Byte = ('0' ['0' .. '9' ['0' .. '9']]) | ('1' ['0' .. '9' ['0' .. '9']]) | ('2' (['0'..'4' ['0'..'9']] | ['5' ['0' .. '5']] | ['6' .. '9']))
// Escape_character = '\' ('\' | '\n' | '\r' | | '\t' | byte | "')
// Literal_character = ^('\' | '"')
// String = '"'  (escape_character | literal_character) {escape_character | literal_character} '"'
// Number = ['-'] 0'..'9' ... // SCANF_INT32_FORMAT 
// Argument = string | number
bool vm_text_lookahead_line_is_directive(unsigned char* line, size_t length);
bool vm_text_read_directive_arg(VmStatus* status, VmForwardStream* stream, VmAssemblyDirectiveArgument* directive_argument);
bool vm_text_read_directive(VmStatus* status, VmForwardStream* stream, VmAssemblyDirective* directive);