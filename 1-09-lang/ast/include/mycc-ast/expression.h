#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

struct ast_assign_expr; // TODO: delete when there will be a vector
struct ast_expr {
	// struct vector* assign_exprs; // Assignment expressions are separated with commas.
	struct ast_assign_expr* assign_expr;
};

bool ast_expr_init(struct ast_expr* expr);
void ast_expr_deinit();

struct ast_expr* ast_expr_new();
struct ast_expr* ast_expr_delete(struct ast_expr* expr);

void ast_print_expr(FILE* file, struct ast_expr const* expr, size_t indent_level);

struct ast_assign_expr;

struct ast_log_or_expr;
struct ast_cond_expr /*{
	// struct vector* log_or_exprs;
	struct ast_log_or_expr* log_or_expr;
}*/;

struct ast_log_and_expr;
struct ast_log_or_expr /*{
	// struct vector* log_and_exprs;
	struct ast_log_and_expr log_and_expr;
}*/;

struct ast_bit_or_expr;
struct ast_log_and_expr /*{
	// struct vector* bit_or_exprs;
	struct ast_bit_or_expr bit_or_expr;
}*/;

struct ast_bit_or_expr;

struct ast_bit_xor_expr;

struct ast_bit_and_expr;

struct ast_equal_expr;

struct ast_rel_expr;

enum ast_bit_shift_expr_op {
	AST_BIT_SHIFT_EXPR_OPERATION_SHIFT_RIGHT,
	AST_BIT_SHIFT_EXPR_OPERATION_SHIFT_LEFT,
};
struct ast_bit_shift_expr {
	struct vector* add_exprs;
	struct vector* ops;
};

struct ast_bit_shift_expr* ast_bit_shift_expr_new();
struct ast_bit_shift_expr* ast_bit_shift_expr_delete(struct ast_bit_shift_expr* bit_shift_expr);

void ast_bit_shift_expr_print(FILE* file, struct ast_bit_shift_expr const* bit_shift_expr, size_t indent_level);

enum ast_add_expr_op {
	AST_ADDITIVE_EXPR_OPERATION_ADDITION,
	AST_ADDITIVE_EXPR_OPERATION_SUBTRACTION,
};
struct ast_add_expr {
	struct vector* mult_exprs;
	struct vector* ops;
};

struct ast_add_expr* ast_add_expr_new();
struct ast_add_expr* ast_add_expr_delete(struct ast_add_expr* add_expr);

void ast_add_expr_print(FILE* file, struct ast_add_expr const* add_expr, size_t indent_level);

enum ast_mult_expr_op {
	AST_MULTIPLICATIVE_EXPR_OPERATION_MULTIPLICATION,
	AST_MULTIPLICATIVE_EXPR_OPERATION_DIVISION,
	AST_MULTIPLICATIVE_EXPR_OPERATION_MODULO
};
struct ast_mult_expr {
	// struct vector* cast_exprs;
	struct vector* primary_exprs;
	struct vector* ops;
};

struct ast_mult_expr* ast_mult_expr_new();
struct ast_mult_expr* ast_mult_expr_delete(struct ast_mult_expr* mult_expr);

void ast_mult_expr_print(FILE* file, struct ast_mult_expr const* mult_expr, size_t indent_level);

struct ast_cast_expr /*{
	enum {
	} type;
}*/;

struct ast_unary_expr /*{
	struct {
		
	} unary_expr_type;
}*/;

struct ast_init_list_expr;

struct ast_postfix_expr_op {
	enum ast_postfix_expr_op_type {
		AST_POSTFIX_EXPR_OPERATION_INDEXATION,
		AST_POSTFIX_EXPR_OPERATION_FUNCTION_CALL,
		AST_POSTFIX_EXPR_OPERATION_STRUCT_MEMBER_ACCESS,
		AST_POSTFIX_EXPR_OPERATION_STRUCT_PTR_MEMBER_ACCESS,
		AST_POSTFIX_EXPR_OPERATION_INCREMENT,
		AST_POSTFIX_EXPR_OPERATION_DECREMENT
	} op_type;
};
struct ast_indexation_op {
	struct ast_postfix_expr_op postfix_expr_op;
	// struct ast_expr* index_expr;
};
struct ast_func_call_op {
	struct ast_postfix_expr_op postfix_expr_op;
	// struct ast_arg_expr_list* args;
};
struct ast_struct_member_access_op {
	struct ast_postfix_expr_op postfix_expr_op;
	// struct ast_identifier member_idfr;
};
struct ast_structp_member_access_op {
	struct ast_postfix_expr_op postfix_expr_op;
	// struct ast_identifier member_idfr;
};
struct ast_primary_expr;
struct ast_postfix_expr {
	struct ast_primary_expr* primary_expr;
	struct ast_init_list_expr* init_list_expr;
	struct vector* op_sequence;
};

struct ast_primary_expr {
	enum ast_primary_expr_type {
		AST_PRIMARY_EXPR_IDENTIFIER,
		AST_PRIMARY_EXPR_CONSTANT,
		AST_PRIMARY_EXPR_STRING_LITERAL,
		AST_PRIMARY_EXPR_WRAPPED_EXPR    // Parenthesized expr.
	} primary_expr_type;
};

void ast_primary_expr_init(struct ast_primary_expr* primary_expr, enum ast_primary_expr_type primary_expr_type);
struct ast_primary_expr* ast_primary_expr_delete(struct ast_primary_expr* primary_expr);

void ast_primary_expr_print(FILE* file, struct ast_primary_expr const* primary_expr, size_t indent_level);
