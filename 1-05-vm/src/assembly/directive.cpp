#include "assembly/directive.h"

#include "status.h"
#include "support/forward_stream.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

bool vm_text_lookahead_line_is_directive(unsigned char* line, size_t length) {
	return length >= 1 && * (char*) line == VM_ASSEMBLY_DIRECTIVE_PREFIX;
}

static bool read_literal_character(VMForwardStream* stream, char* string, size_t* length) {
	char next_char = vm_read_char(stream);

	if (next_char != VM_EOF && next_char != '"') {
		string[*length] = next_char;
	} else {
		return false;
	}

	*length += 1;

	return true;
}

// Directive = '.' directive_name ' ' {' '} {argument}
// Escape_character = '\' ('\' | 'n' | 'r' | | 't' | byte | "')
// Literal_character = ^('\' | '"')
static bool read_escape_character(VMForwardStream* stream, char* string, size_t* length) {
	if (vm_read_char(stream) != '\\') {
		return false;
	}

	char next_char = vm_peek_char(stream);

	if (next_char == '\\') {
		string[*length] = '\\';
		vm_read_char(stream);

	} else if (next_char == 'n') {
		string[*length] = '\n';
		vm_read_char(stream);

	} else if (next_char == 'r') {
		string[*length] = '\r';
		vm_read_char(stream);

	} else if (next_char == 't') {
		string[*length] = '\t';
		vm_read_char(stream);

	} else if ('0' <= next_char && next_char <= '9') {
		uint8_t value = 0;
		sscanf((char*) stream->bytes, "%" SCNu8, &value);
		* (uint8_t*) &string[*length] = value;
		length += sizeof(uint8_t) - 1;

	} else if (next_char == '"') {
		string[*length] = '"';
		vm_read_char(stream);

	} else {
		return false;
	}

	*length += 1;

	return true;
}

// Escape_character = '\' ('\' | '\n' | '\r' | | '\t' | byte | "')
// Literal_character = ^('\' | '"')
// String = '"'  (escape_character | literal_character) {escape_character | literal_character} '"'
static bool read_string(VMForwardStream* stream, char* string, size_t* length) {
	if (vm_read_char(stream) != '"') {
		return false;
	}

	if (vm_peek_char(stream) == '\\') {
		if (!read_escape_character(stream, string, length)) {
			return false;
		}
	} else {
		if (!read_literal_character(stream, string, length)) {
			return false;
		}
	}

	while (*length + 1 <= VM_ASSEMBLY_DIRECTIVE_MAX_STRING_ARG_LENGTH && vm_peek_char(stream) != VM_EOF && vm_peek_char(stream) != '"') {
		if (vm_peek_char(stream) == '\\') {
			if (!read_escape_character(stream, string, length)) {
				return false;
			}
		} else {
			if (!read_literal_character(stream, string, length)) {
				return false;
			}
		}
	}

	if (vm_read_char(stream) != '"') {
		return false;
	}

	return true;
}

// String = '"'  (escape_character | literal_character) {escape_character | literal_character} '"'
// Number = ['-'] 0'..'9' ... // SCANF_INT32_FORMAT 
// Argument = (string | number) {' '} [',' {' '}]
bool vm_text_read_directive_arg(VMStatus* status, VMForwardStream* stream, VMAssemblyDirectiveArgument* directive_argument) {
	((void) status);

	char next_char = vm_peek_char(stream);

	if (next_char == '"') {
		directive_argument->argument_type = VM_ASSEMBLY_DIRECTIVE_ARG_STRING;
		if (!read_string(stream, directive_argument->string, &directive_argument->string_length)) {
			*status = VM_ASSEMBLY_ERROR_INVALID_DIRECTIVE;
			return false;
		}
	} else if (next_char == '-' || ('0' <= next_char && next_char <= '9')) {
		directive_argument->argument_type = VM_ASSEMBLY_DIRECTIVE_ARG_NUMBER;

		int num_read = 0;
		// Must end with a null byte then.
		if (sscanf((char*) stream->bytes, "%10" SCNd32 "%n", &directive_argument->number, &num_read) != 1) {
			*status = VM_ASSEMBLY_ERROR_INVALID_DIRECTIVE;
			return false;
		}

		stream->bytes += num_read;
		stream->offset += num_read;
		stream->length -= num_read;
	}

	while (vm_peek_char(stream) == ' ') {
		vm_read_char(stream);
	}

	if (vm_peek_char(stream) == ',') {
		vm_read_char(stream);

		while (vm_peek_char(stream) == ' ') {
			vm_read_char(stream);
		}
	}

	return true;
}

bool vm_text_read_directive(VMStatus* status, VMForwardStream* stream, VMAssemblyDirective* directive) {
	char prefix = 0;

	if (!vm_read_bytes(stream, (uint8_t*) &prefix, sizeof(char))) {
		*status = VM_ASSEMBLY_ERROR_INVALID_DIRECTIVE;
		return false;
	}

	int num_bytes_read = 0;
	sscanf((char*) stream->bytes, VM_ASSEMBLY_DIRECTIVE_SCANF_FORMAT "%n", directive->name, &num_bytes_read);
	if (num_bytes_read == 0) {
		*status = VM_ASSEMBLY_ERROR_INVALID_DIRECTIVE;
		return false;
	}

	vm_advance_stream(stream, num_bytes_read);

	vm_skip_repeats(stream, ' ');

	for (size_t i = 0; stream->length != 0 && i < VM_ASSEMBLY_DIRECTIVE_MAX_NUMBER_OF_ARGUMENTS; ++i) {

		if (!vm_text_read_directive_arg(status, stream, &directive->arguments[i])) {
			return false;
		}

		directive->num_arguments += 1;
	}

	return true;
}
