#pragma once

struct ast_external_decl_node {
	enum ast_external_decl_type {
		AST_EXTERNAL_DECL_TYPE_FUNCTION,
		AST_EXTERNAL_DECL_TYPE_VARIABLE
	} decl_type;
};

void ast_print_external_decl_node(FILE* file, struct ast_external_decl_node const* external_decl, size_t indent_level);
