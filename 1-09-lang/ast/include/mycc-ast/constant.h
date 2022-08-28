#pragma once

#include "expression.h"

#include <stdbool.h>

struct ast_constant {
	struct ast_primary_expr primary_expr;
	enum ast_constant_type {
		AST_CONSTANT_TYPE_INTEGER_CONSTANT,
		AST_CONSTANT_TYPE_FLOATING_CONSTANT,
		AST_CONSTANT_TYPE_ENUMERATION_CONSTANT,
		AST_CONSTANT_TYPE_CHARACTER_CONSTANT
	} constant_type;
};

void ast_constant_init(struct ast_constant* constant, enum ast_constant_type constant_type);

void ast_constant_print(FILE* file, struct ast_constant const* constant, size_t indent_level);

struct ast_constant* ast_constant_delete(struct ast_constant* constant);

struct ast_int_constant {
	struct ast_constant constant;
	enum ast_int_constant_base {
		AST_INT_CONSTANT_BASE_OCTAL,
		AST_INT_CONSTANT_BASE_DECIMAL,
		AST_INT_CONSTANT_BASE_HEXADECIMAL
	} base;
	enum ast_int_constant_c_type {
		AST_INT_CONSTANT_INT_TYPE,
		AST_INT_CONSTANT_LONG_TYPE,
		AST_INT_CONSTANT_LONG_LONG_TYPE,
		AST_INT_CONSTANT_UNSIGNED_INT_TYPE,
		AST_INT_CONSTANT_UNSIGNED_LONG_TYPE,
		AST_INT_CONSTANT_UNSIGNED_LONG_LONG_TYPE
	} c_type;
	char* value;
};

bool ast_int_constant_init_from_token(struct ast_int_constant* int_constant, char const* text);
void ast_int_constant_deinit();

struct ast_int_constant* ast_int_constant_new_from_token(char const* text);
struct ast_int_constant* ast_int_constant_delete(struct ast_int_constant* int_constant);

void ast_print_int_constant(FILE* file, struct ast_int_constant const* int_constant, size_t indent_level);

struct ast_float_constant {
	struct ast_constant constant;
	enum ast_float_constant_base {
		AST_FLOAT_CONSTANT_DECIMAL,
		AST_FLOAT_CONSTANT_HEXADECIMAL
	} base;
	char const* value;
};