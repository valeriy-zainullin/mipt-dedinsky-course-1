#include "expression.h"

#include "constant.h"
#include "print_indent.h"

#include <assert.h>

void ast_primary_expr_init(struct ast_primary_expr* primary_expr, enum ast_primary_expr_type primary_expr_type) {
	primary_expr->primary_expr_type = primary_expr_type;
}

struct ast_primary_expr* ast_primary_expr_delete(struct ast_primary_expr* primary_expr) {
	switch (primary_expr->primary_expr_type) {
		case AST_PRIMARY_EXPR_IDENTIFIER: {
			assert(false); // Unimplemented.
			return NULL;
			break;
		}
		
		case AST_PRIMARY_EXPR_CONSTANT: {
			return (struct ast_primary_expr*) ast_constant_delete((struct ast_constant*) primary_expr);
			break;
		}
		
		case AST_PRIMARY_EXPR_STRING_LITERAL: {
			assert(false); // Unimplemented.
			return NULL;
			break;
		}
		
		case AST_PRIMARY_EXPR_WRAPPED_EXPR: {
			assert(false); // Unimplemented.
			return NULL;
			break;
		}
		
		// All possible cases are handled.
		default: assert(false); __builtin_unreachable();
	}
}

void ast_primary_expr_print(FILE* file, struct ast_primary_expr const* primary_expr, size_t indent_level) {
	switch (primary_expr->primary_expr_type) {
		case AST_PRIMARY_EXPR_IDENTIFIER: {
			assert(false); // Unimplemented.
			break;
		}
		
		case AST_PRIMARY_EXPR_CONSTANT: {
			ast_constant_print(file, (struct ast_constant const *) primary_expr, indent_level);
			break;
		}
		
		case AST_PRIMARY_EXPR_STRING_LITERAL: {
			assert(false); // Unimplemented.
			break;
		}
		
		case AST_PRIMARY_EXPR_WRAPPED_EXPR: {
			assert(false); // Unimplemented.
			break;
		}
		
		// All possible cases are handled.
		default: assert(false); __builtin_unreachable();
	}
}
