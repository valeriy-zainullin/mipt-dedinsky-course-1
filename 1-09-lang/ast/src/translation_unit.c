#include "translation_unit.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*
bool ast_translation_unit_node_init(struct ast_translation_unit_node* translation_unit) {
	translation_unit->external_decls = vector_new(0, sizeof(struct ast_external_decl_node));
	if (translation_unit->external_decls == NULL) {
		return false;
	}
	
	return true;
}

struct ast_translation_unit_node* ast_translation_unit_node_new() {
	struct ast_translation_unit_node* translation_unit = calloc(1, sizeof(struct ast_translation_unit_node));
	if (translation_unit == NULL) {
		return NULL;
	}
	
	if (!ast_translation_unit_node_init(translation_unit)) {
		free(translation_unit);
		return NULL;
	}
	
	return translation_unit;
}

struct ast_translation_unit_node* ast_translation_unit_node_delete(struct ast_translation_unit_node* translation_unit) {
	#if DEBUG_BUILD
	if (translation_unit == NULL) {
		fprintf(stderr, "Warning: deallocation of already freed object struct ast_translation_unit_node.\n");
		return NULL;
	}
	#endif

	free(translation_unit);
	return NULL;
}

static void print_indent(FILE* file, size_t indent_level) {
	while (indent_level > 0) {
		fprintf(file, "  ");
		indent_level -= 1;
	}
}

void ast_print_translation_unit_node(FILE* file, struct ast_translation_unit_node const* translation_unit, size_t indent_level) {
	print_indent(file, indent_level);
	fprintf(file, "translation_unit(file=\"\")\n");
	
	print_indent(file, indent_level);
	fprintf(file, "external_decls:\n");
	
	for (size_t i = 0; i < vector_get_size(translation_unit->external_decls); ++i) {
		ast_print_external_decl_node(file, (struct ast_external_decl_node const*) vector_get_const_item_ptr(translation_unit->external_decls, i), indent_level + 1);
	}
}

void ast_print_external_decl_node(FILE* file, struct ast_external_decl_node const* external_decl, size_t indent_level) {
	switch (external_decl->decl_type) {
		case AST_EXTERNAL_DECL_TYPE_FUNCTION: {
			(void) file;
			(void) indent_level;
			// ast_print_function_decl_node(file, (struct ast_function_decl_node const*) external_decl, indent_level);
			break;
		}
		
		case AST_EXTERNAL_DECL_TYPE_VARIABLE: {
			// ast_print_variable_decl_node(file, (struct ast_variable_decl_node const*) external_decl, indent_level);
			break;
		}
		
		default: {
			#if defined(DEBUG_BUILD)
				// WARN("invalid external decl type"); // "Warning: %s. File %s, line %d, function %s.\n"
				fprintf(stderr, "Warning: invalid external decl type.\n");
			#endif

			assert(false);
			__builtin_unreachable();
		}
	}
}
*/