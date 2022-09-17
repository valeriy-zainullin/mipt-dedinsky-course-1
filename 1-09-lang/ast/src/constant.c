#include "constant.h"

#include "print_indent.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

static bool string_startswith(char const* string, char const* prefix) {
	return strncmp(string, prefix, strlen(prefix)) == 0;
}

static bool check_is_int_suffix_char(char character) {
	return character == 'u' || character == 'U' || character == 'l' || character == 'L';
}

void ast_constant_init(struct ast_constant* constant, enum ast_constant_type constant_type) {
	ast_primary_expr_init((struct ast_primary_expr*) constant, AST_PRIMARY_EXPR_CONSTANT);
	constant->constant_type = constant_type;
}

struct ast_constant* ast_constant_delete(struct ast_constant* constant) {
	switch (constant->constant_type) {
		case AST_CONSTANT_TYPE_INTEGER_CONSTANT: {
			return (struct ast_constant*) ast_int_constant_delete((struct ast_int_constant*) constant);
		}
		
		case AST_CONSTANT_TYPE_FLOATING_CONSTANT:
		case AST_CONSTANT_TYPE_ENUMERATION_CONSTANT:
		case AST_CONSTANT_TYPE_CHARACTER_CONSTANT:
		default: {
			printf("ast_constant_delete: unimplemented.\n");
			return NULL;
		}
	}
}

bool ast_int_constant_init_from_token(struct ast_int_constant* int_constant, char const* text) {
	assert(strlen(text) != 0);
	
	enum ast_int_constant_base base = AST_INT_CONSTANT_BASE_DECIMAL;
	if (string_startswith(text, "0x") || string_startswith(text, "0X")) {
		base = AST_INT_CONSTANT_BASE_HEXADECIMAL;
		text += 2;
	} else if (string_startswith(text, "0")) {
		base = AST_INT_CONSTANT_BASE_OCTAL;
	}
	
	size_t suffix_start = strlen(text);
	while (suffix_start > 0 && check_is_int_suffix_char(text[suffix_start - 1])) {
		suffix_start -= 1;
	}
	
	char* constant_value = calloc(strlen(text) + 1, sizeof(char));
	if (constant_value == NULL) {
		return false;
	}
	strcpy(constant_value, text);
	constant_value[suffix_start] = '\0';
	
	bool is_unsigned = false;
	bool is_long = false;
	bool is_long_long = false;
	for (size_t i = suffix_start; text[i] != '\0'; ++i) {
		if ((text[i] == 'u' || text[i] == 'U') && !is_unsigned) {
			is_unsigned = true;
			continue;
		} else if ((text[i] == 'l' || text[i] == 'L') && !is_long && !is_long_long) {
			is_long = true;
			continue;
		} else if ((text[i] == 'l' || text[i] == 'L') && is_long && !is_long_long) {
			is_long = false;
			is_long_long = true;
			continue;
		}
		
		// Invalid character. Ignored in release build, but error in debug build.
		// Should never happen in correct execution of the compiler.
		// Tokenizer checks syntax of integer constants, here we just extract information.
		assert(false);
	}
	
	enum ast_int_constant_c_type c_type = AST_INT_CONSTANT_INT_TYPE;
	if (!is_unsigned) {
		if (is_long) {
			c_type = AST_INT_CONSTANT_LONG_TYPE;
		} else if (is_long_long) {
			c_type = AST_INT_CONSTANT_LONG_LONG_TYPE;
		} else {
			c_type = AST_INT_CONSTANT_INT_TYPE;
		}
	} else {
		if (is_long) {
			c_type = AST_INT_CONSTANT_UNSIGNED_LONG_TYPE;
		} else if (is_long_long) {
			c_type = AST_INT_CONSTANT_UNSIGNED_LONG_LONG_TYPE;
		} else {
			c_type = AST_INT_CONSTANT_UNSIGNED_INT_TYPE;
		}
	}
		
	ast_constant_init((struct ast_constant*) int_constant, AST_CONSTANT_TYPE_INTEGER_CONSTANT);
	
	int_constant->base = base;
	int_constant->c_type = c_type;
	int_constant->value = constant_value;
	
	return int_constant;
}

void ast_int_constant_deinit(struct ast_int_constant* int_constant) {
	free(int_constant->value);
	int_constant->value = NULL;
}

struct ast_int_constant* ast_int_constant_new_from_token(char const* text) {
	struct ast_int_constant* int_constant = calloc(1, sizeof(struct ast_int_constant));
	if (int_constant == NULL) {
		return false;
	}
	
	if (!ast_int_constant_init_from_token(int_constant, text)) {
		free(int_constant);
		return false;
	}
	
	return int_constant;
}

struct ast_int_constant* ast_int_constant_delete(struct ast_int_constant* int_constant) {
	ast_int_constant_deinit(int_constant);
	free(int_constant);
	return NULL;
}

void ast_int_constant_print(FILE* file, struct ast_int_constant const* int_constant, size_t indent_level) {
	print_indent(file, indent_level);

	fprintf(file, "integer_constant(");
	
	switch (int_constant->base) {
		case AST_INT_CONSTANT_BASE_OCTAL: {
			fprintf(file, "octal,");
			break;
		}
		
		case AST_INT_CONSTANT_BASE_DECIMAL: {
			fprintf(file, "decimal,");
			break;
		}
		
		case AST_INT_CONSTANT_BASE_HEXADECIMAL: {
			fprintf(file, "hexadecimal,");
			break;
		}
		
		// All possible cases are handled.
		default: assert(false); __builtin_unreachable();
	}
	
	switch (int_constant->c_type) {
		case AST_INT_CONSTANT_INT_TYPE: {
			fprintf(file, "int");
			break;
		}
		
		case AST_INT_CONSTANT_LONG_TYPE: {
			fprintf(file, "long");
			break;
		}

		case AST_INT_CONSTANT_LONG_LONG_TYPE: {
			fprintf(file, "long long");
			break;
		}
		
		case AST_INT_CONSTANT_UNSIGNED_INT_TYPE: {
			fprintf(file, "unsigned int");
			break;
		}

		case AST_INT_CONSTANT_UNSIGNED_LONG_TYPE: {
			fprintf(file, "unsigned long");
			break;
		}
		
		case AST_INT_CONSTANT_UNSIGNED_LONG_LONG_TYPE: {
			fprintf(file, "unsigned long long");
			break;
		}
	}
	
	fprintf(file, ") = %s\n", int_constant->value);
}

void ast_constant_print(FILE* file, struct ast_constant const* constant, size_t indent_level) {
	switch (constant->constant_type) {
		case AST_CONSTANT_TYPE_INTEGER_CONSTANT: {
			ast_int_constant_print(file, (struct ast_int_constant*) constant, indent_level);
			return;
		}
		
		case AST_CONSTANT_TYPE_FLOATING_CONSTANT: {
			return;
		}
		
		case AST_CONSTANT_TYPE_ENUMERATION_CONSTANT: {
			return;
		}
		
		case AST_CONSTANT_TYPE_CHARACTER_CONSTANT: {
			return;
		}
		
		// All possible cases are handled.
		default: assert(false); __builtin_unreachable();
	}
}