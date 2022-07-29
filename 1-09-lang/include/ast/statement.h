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