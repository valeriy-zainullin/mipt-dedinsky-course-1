#include "expression.h"

#include "print_indent.h"

#include "mycc-libs/vector.h"

#include <assert.h>
#include <stdlib.h>

struct ast_add_expr* ast_add_expr_new() {
	struct ast_add_expr* add_expr = calloc(1, sizeof(struct ast_add_expr));
	if (add_expr == NULL) {
		return NULL;
	}
	
	add_expr->mult_exprs = vector_new(0, sizeof(struct ast_mult_expr*));
	if (add_expr->mult_exprs == NULL) {
		free(add_expr);
		return NULL;
	}
	
	add_expr->ops = vector_new(0, sizeof(enum ast_add_expr_op));
	if (add_expr->ops == NULL) {
		add_expr->mult_exprs = vector_delete(add_expr->mult_exprs);
		free(add_expr);
		return NULL;
	}
	
	return add_expr;
}

struct ast_add_expr* ast_add_expr_delete(struct ast_add_expr* add_expr) {
	for (size_t i = 0; i < vector_get_size(add_expr->mult_exprs); ++i) {
		struct ast_mult_expr** mult_expr_storage = vector_get_item_ptr(add_expr->mult_exprs, i);
		*mult_expr_storage = ast_mult_expr_delete(*mult_expr_storage);
	}
	add_expr->mult_exprs = vector_delete(add_expr->mult_exprs);

	add_expr->ops = vector_delete(add_expr->ops);
	free(add_expr);
	
	return NULL;
}

void ast_add_expr_print(FILE* file, struct ast_add_expr const* add_expr, size_t indent_level) {
	if (vector_check_empty(add_expr->mult_exprs)) {
		return;
	}
	if (vector_get_size(add_expr->mult_exprs) == 1) {
		struct ast_mult_expr const* mult_expr = NULL;
		vector_get(add_expr->mult_exprs, 0, &mult_expr);
		ast_mult_expr_print(file, mult_expr, indent_level);
		return;
	}
	
	assert(vector_get_size(add_expr->mult_exprs) == vector_get_size(add_expr->ops) + 1);

	print_indent(file, indent_level);
	fprintf(file, "additive_expr:\n");
	
	struct ast_mult_expr const* mult_expr = NULL;
	vector_get(add_expr->mult_exprs, 0, &mult_expr);
	ast_mult_expr_print(file, mult_expr, indent_level + 1);

	for (size_t i = 1; i < vector_get_size(add_expr->mult_exprs); ++i) {
		enum ast_add_expr_op op = 0;
		vector_get(add_expr->ops, i - 1, &op);
		switch (op) {
			case AST_ADDITIVE_EXPR_OPERATION_ADDITION: {
				print_indent(file, indent_level);
				fprintf(file, "+\n");
				break;
			}
			
			case AST_ADDITIVE_EXPR_OPERATION_SUBTRACTION: {
				print_indent(file, indent_level);
				fprintf(file, "-\n");
				break;
			}
			
			default: assert(false); __builtin_unreachable();
		}
		
		vector_get(add_expr->mult_exprs, i, &mult_expr);
		ast_mult_expr_print(file, mult_expr, indent_level + 1);
	}
}
