#pragma once

#include <stdbool.h>

struct ast_constant {
	/* struct ast_primary_expr* primary_expr; */
	enum ast_constant_type {
		AST_CONSTANT_TYPE_INTEGER_CONSTANT,
		AST_CONSTANT_TYPE_FLOATING_CONSTANT,
		AST_CONSTANT_TYPE_ENUMERATION_CONSTANT,
		AST_CONSTANT_TYPE_CHARACTER_CONSTANT
	} constant_type;
};

struct ast_int_constant {
	struct ast_constant constant;
	enum ast_int_constant_base {
		AST_INT_CONSTANT_OCTAL,
		AST_INT_CONSTANT_DECIMAL,
		AST_INT_CONSTANT_HEXADECIMAL
	} base;
	char const* value;
};

bool ast_int_constant_init_from_token(struct ast_int_constant* int_constant, char const* text);
void ast_int_constant_deinit();

struct ast_int_constant* ast_int_constant_new_from_token(char const* text);
struct ast_int_constant* ast_int_constant_delete(struct ast_int_constant* int_constant);

struct ast_float_constant {
	struct ast_constant constant;
	enum ast_float_constant_base {
		AST_FLOAT_CONSTANT_DECIMAL,
		AST_FLOAT_CONSTANT_HEXADECIMAL
	} base;
	char const* value;
};