#pragma once

#include "code_block.h"

// Decl is not an external_decl. Nor the other way around.
// The name implies the connection external_decl is a child of decl, but external_decl is above
// decl in parser syntax, so decl should be a child of external_decl. There's just no
// child-parent relationship.
struct ast_external_decl {
	enum ast_external_decl_type {
		AST_EXTERNAL_DECL_TYPE_FUNCTION,
		AST_EXTERNAL_DECL_TYPE_VARIABLE
	} external_decl_type;
};

struct ast_func_def {
	struct ast_external_decl external_decl;
	char* return_type; // TODO: make typeid later, will be faster to check, maybe also the language allows shadowing of types, dunno.
	char* func_name;
	struct ast_code_block* code_block;
};

void ast_print_external_decl(FILE* file, struct ast_external_decl const* external_decl, size_t indent_level);
