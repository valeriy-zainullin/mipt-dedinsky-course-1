#include "statement.h"

#include "constant.h"

#include <stdlib.h>

void ast_stmt_init(struct ast_stmt* stmt, enum ast_stmt_type stmt_type) {
	stmt->stmt_type = stmt_type;
}

void ast_return_stmt_init(struct ast_return_stmt* return_stmt, struct ast_constant* constant/*struct ast_expr* expr*/) {
	ast_stmt_init((struct ast_stmt*) return_stmt, AST_STMT_TYPE_RETURN_STMT);
	// return_stmt->expr = expr;
	return_stmt->constant = constant;
}

void ast_return_stmt_deinit(struct ast_return_stmt* return_stmt) {
	/*if (return_stmt->expr != NULL) {
		return_stmt->expr = ast_expr_delete(return_stmt->expr);
	}*/
	if (return_stmt->constant != NULL) {
		return_stmt->constant = ast_constant_delete(return_stmt->constant);
	}
}

struct ast_return_stmt* ast_return_stmt_new(struct ast_constant* constant/*struct ast_expr* expr*/) {
	struct ast_return_stmt* return_stmt = calloc(1, sizeof(struct ast_return_stmt));
	if (return_stmt == NULL) {
		return NULL;
	}
	
	// ast_return_stmt_init(return_stmt, expr);
	ast_return_stmt_init(return_stmt, constant);
	return return_stmt;
}

struct ast_return_stmt* ast_return_stmt_delete(struct ast_return_stmt* return_stmt) {
	ast_return_stmt_deinit(return_stmt);
	free(return_stmt);
	return NULL;
}

void ast_print_return_stmt(FILE* file, struct ast_return_stmt const* return_stmt, size_t indent_level) {
	(void) file;
	(void) return_stmt;
	(void) indent_level;
}