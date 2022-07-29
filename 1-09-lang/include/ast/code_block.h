#pragma once

#include "statement.h"

#include "vector.h"

#include <stdbool.h>
#include <stdio.h>

struct ast_code_block_item_node {
	/*
	  CODESTYLE: Writing full names in enums and file names.
	  E.g. statement.h, statement.c, AST_CODE_BLOCK_STATEMENT_ITEM for ast_stmt;
	  declaration.h, declaration.c, AST_CODE_BLOCK_DECLARATION_ITEM for ast_decl if such would exist.
	  So there's at least one place it's fully diciphered.
	  For derived classes their short names are written. For example, ast_label_decl, label_decl.h and etc.
	*/
	enum ast_code_block_item_type {
		AST_CODE_BLOCK_DECLARATION_ITEM,
		AST_CODE_BLOCK_STATEMENT_ITEM
	} item_type;
	union {
		struct ast_stmt* stmt;
	};
};

struct ast_code_block_node {
	struct vector* items;
};

bool ast_code_block_node_init(struct ast_code_block_node* code_block);
struct ast_code_block_node* ast_code_block_node_new();
struct ast_code_block_node* ast_code_block_node_delete(struct ast_code_block_node* code_block);

void ast_print_code_block_node(FILE* file, struct ast_code_block_node const* code_block, size_t indent_level);
