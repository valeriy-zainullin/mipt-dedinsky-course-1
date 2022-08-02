#pragma once

struct ast_stmt {
	enum {
		// jump-statement:
		AST_STMT_TYPE_RETURN_STMT
	} stmt_type;
};

struct ast_return_stmt {
	struct ast_stmt stmt;
	struct ast_expr* expr;
};

void ast_return_stmt_init(struct ast_expr* expr);
void ast_return_stmt_deinit(struct ast_expr* expr);

struct ast_return_stmt* ast_return_stmt_new(struct ast_expr* expr);
struct ast_return_stmt* ast_return_stmt_delete(struct ast_expr* expr);

void ast_print_return_stmt(FILE* file, struct ast_return_stmt const* return_stmt, size_t indent_level);
