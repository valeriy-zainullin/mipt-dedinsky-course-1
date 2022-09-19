#include "expression.h"

#include "print_indent.h"

#include "mycc-libs/vector.h"

#include <assert.h>
#include <stdlib.h>

struct ast_mult_expr* ast_mult_expr_new() {
	struct ast_mult_expr* mult_expr = calloc(1, sizeof(struct ast_mult_expr));
	if (mult_expr == NULL) {
		return NULL;
	}
	
	mult_expr->primary_exprs = vector_new(0, sizeof(struct ast_primary_expr*));
	if (mult_expr->primary_exprs == NULL) {
		free(mult_expr);
		return NULL;
	}
	
	mult_expr->ops = vector_new(0, sizeof(enum ast_mult_expr_op));
	if (mult_expr->ops == NULL) {
		mult_expr->primary_exprs = vector_delete(mult_expr->primary_exprs);
		free(mult_expr);
		return NULL;
	}
	
	return mult_expr;
}

struct ast_mult_expr* ast_mult_expr_delete(struct ast_mult_expr* mult_expr) {
	for (size_t i = 0; i < vector_get_size(mult_expr->primary_exprs); ++i) {
		struct ast_primary_expr** primary_expr_storage = vector_get_item_ptr(mult_expr->primary_exprs, i);
		*primary_expr_storage = ast_primary_expr_delete(*primary_expr_storage);
	}
	mult_expr->primary_exprs = vector_delete(mult_expr->primary_exprs);

	mult_expr->ops = vector_delete(mult_expr->ops);
	free(mult_expr);
	
	return NULL;
}

void ast_mult_expr_print(FILE* file, struct ast_mult_expr const* mult_expr, size_t indent_level) {
	if (vector_check_empty(mult_expr->primary_exprs)) {
		return;
	}
	if (vector_get_size(mult_expr->primary_exprs) == 1) {
		struct ast_primary_expr const* primary_expr = NULL;
		vector_get(mult_expr->primary_exprs, 0, &primary_expr);
		ast_primary_expr_print(file, primary_expr, indent_level);
		return;
	}
	
	assert(vector_get_size(mult_expr->primary_exprs) == vector_get_size(mult_expr->ops) + 1);

	print_indent(file, indent_level);
	fprintf(file, "multiplicative_expr:\n");
	
	struct ast_primary_expr const* primary_expr = NULL;
	vector_get(mult_expr->primary_exprs, 0, &primary_expr);
	ast_primary_expr_print(file, primary_expr, indent_level + 1);

	for (size_t i = 1; i < vector_get_size(mult_expr->primary_exprs); ++i) {
		enum ast_mult_expr_op op = 0;
		vector_get(mult_expr->ops, i - 1, &op);
		switch (op) {
			case AST_MULTIPLICATIVE_EXPR_OPERATION_MULTIPLICATION: {
				print_indent(file, indent_level);
				fprintf(file, "*\n");
				break;
			}
			
			case AST_MULTIPLICATIVE_EXPR_OPERATION_DIVISION: {
				print_indent(file, indent_level);
				fprintf(file, "/\n");
				break;
			}
			
			case AST_MULTIPLICATIVE_EXPR_OPERATION_MODULO: {
				print_indent(file, indent_level);
				fprintf(file, "%%\n");
				break;
			}
			
			default: assert(false); __builtin_unreachable();
		}
		
		vector_get(mult_expr->primary_exprs, i, &primary_expr);
		ast_primary_expr_print(file, primary_expr, indent_level + 1);
	}
}
