#include "expression.h"

#include "print_indent.h"

#include "mycc-libs/vector.h"

#include <assert.h>
#include <stdlib.h>

struct ast_bit_shift_expr* ast_bit_shift_expr_new() {
	struct ast_bit_shift_expr* bit_shift_expr = calloc(1, sizeof(struct ast_bit_shift_expr));
	if (bit_shift_expr == NULL) {
		return NULL;
	}
	
	bit_shift_expr->add_exprs = vector_new(0, sizeof(struct ast_add_expr*));
	if (bit_shift_expr->add_exprs == NULL) {
		free(bit_shift_expr);
		return NULL;
	}
	
	bit_shift_expr->ops = vector_new(0, sizeof(enum ast_bit_shift_expr_op));
	if (bit_shift_expr->ops == NULL) {
		bit_shift_expr->add_exprs = vector_delete(bit_shift_expr->add_exprs);
		free(bit_shift_expr);
		return NULL;
	}
	
	return bit_shift_expr;
}

struct ast_bit_shift_expr* ast_bit_shift_expr_delete(struct ast_bit_shift_expr* bit_shift_expr) {
	for (size_t i = 0; i < vector_get_size(bit_shift_expr->add_exprs); ++i) {
		struct ast_add_expr** add_expr_storage = vector_get_item_ptr(bit_shift_expr->add_exprs, i);
		*add_expr_storage = ast_add_expr_delete(*add_expr_storage);
	}
	bit_shift_expr->add_exprs = vector_delete(bit_shift_expr->add_exprs);

	bit_shift_expr->ops = vector_delete(bit_shift_expr->ops);
	free(bit_shift_expr);
	
	return NULL;
}

void ast_bit_shift_expr_print(FILE* file, struct ast_bit_shift_expr const* bit_shift_expr, size_t indent_level) {
	if (vector_check_empty(bit_shift_expr->add_exprs)) {
		return;
	}
	if (vector_get_size(bit_shift_expr->add_exprs) == 1) {
		struct ast_add_expr const* add_expr = NULL;
		vector_get(bit_shift_expr->add_exprs, 0, &add_expr);
		ast_add_expr_print(file, add_expr, indent_level);
		return;
	}
	
	assert(vector_get_size(bit_shift_expr->add_exprs) == vector_get_size(bit_shift_expr->ops) + 1);

	print_indent(file, indent_level);
	fprintf(file, "bit_shift_expr:\n");
	
	struct ast_add_expr const* add_expr = NULL;
	vector_get(bit_shift_expr->add_exprs, 0, &add_expr);
	ast_add_expr_print(file, add_expr, indent_level + 1);

	for (size_t i = 1; i < vector_get_size(bit_shift_expr->add_exprs); ++i) {
		enum ast_bit_shift_expr_op op = 0;
		vector_get(bit_shift_expr->ops, i - 1, &op);
		switch (op) {
			case AST_BIT_SHIFT_EXPR_OPERATION_SHIFT_RIGHT: {
				print_indent(file, indent_level);
				fprintf(file, ">>\n");
				break;
			}
			
			case AST_BIT_SHIFT_EXPR_OPERATION_SHIFT_LEFT: {
				print_indent(file, indent_level);
				fprintf(file, "<<\n");
				break;
			}
			
			default: assert(false); __builtin_unreachable();
		}
		
		vector_get(bit_shift_expr->add_exprs, i, &add_expr);
		ast_add_expr_print(file, add_expr, indent_level + 1);
	}
}
