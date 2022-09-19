#pragma once

#include "code_block.h"

#include <stdio.h>

struct ast_stmt {
	// struct ast_code_block_item code_block_item;
	enum ast_stmt_type {
		// jump-statement:
		AST_STMT_TYPE_RETURN_STMT
	} stmt_type;
};

void ast_stmt_init(struct ast_stmt* stmt, enum ast_stmt_type stmt_type);

struct ast_return_stmt {
	struct ast_stmt stmt;
	// struct ast_expr* expr;
	struct ast_bit_shift_expr* bit_shift_expr;
};

void ast_return_stmt_init(struct ast_return_stmt* return_stmt, struct ast_bit_shift_expr* bit_shift_expr/*struct ast_expr* expr*/);
void ast_return_stmt_deinit(struct ast_return_stmt* return_stmt);

struct ast_return_stmt* ast_return_stmt_new(struct ast_bit_shift_expr* bit_shift_expr/*struct ast_expr* expr*/);
struct ast_return_stmt* ast_return_stmt_delete(struct ast_return_stmt* return_stmt);

void ast_return_stmt_print(FILE* file, struct ast_return_stmt const* return_stmt, size_t indent_level);
