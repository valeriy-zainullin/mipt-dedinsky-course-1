#include "statement.h"

#include "expression.h"

#include <stdlib.h>

void ast_stmt_init(struct ast_stmt* stmt, enum ast_stmt_type stmt_type) {
	stmt->stmt_type = stmt_type;
}

void ast_return_stmt_init(struct ast_return_stmt* return_stmt, struct ast_bit_shift_expr* bit_shift_expr/*struct ast_expr* expr*/) {
	ast_stmt_init((struct ast_stmt*) return_stmt, AST_STMT_TYPE_RETURN_STMT);
	// return_stmt->expr = expr;
	return_stmt->bit_shift_expr = bit_shift_expr;
}

void ast_return_stmt_deinit(struct ast_return_stmt* return_stmt) {
	/*if (return_stmt->expr != NULL) {
		return_stmt->expr = ast_expr_delete(return_stmt->expr);
	}*/
	if (return_stmt->bit_shift_expr != NULL) {
		return_stmt->bit_shift_expr = ast_bit_shift_expr_delete(return_stmt->bit_shift_expr);
	}
}

struct ast_return_stmt* ast_return_stmt_new(struct ast_bit_shift_expr* bit_shift_expr/*struct ast_expr* expr*/) {
	struct ast_return_stmt* return_stmt = calloc(1, sizeof(struct ast_return_stmt));
	if (return_stmt == NULL) {
		return NULL;
	}
	
	// ast_return_stmt_init(return_stmt, expr);
	ast_return_stmt_init(return_stmt, bit_shift_expr);
	return return_stmt;
}

struct ast_return_stmt* ast_return_stmt_delete(struct ast_return_stmt* return_stmt) {
	ast_return_stmt_deinit(return_stmt);
	free(return_stmt);
	return NULL;
}

void ast_return_stmt_print(FILE* file, struct ast_return_stmt const* return_stmt, size_t indent_level) {
	fprintf(file, "return_stmt");
	if (return_stmt->bit_shift_expr != NULL) {
		fprintf(file, ":\n");
		ast_bit_shift_expr_print(file, return_stmt->bit_shift_expr, indent_level + 1);
	} else {
		fprintf(file, "\n");
	}
}