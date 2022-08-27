#pragma once

#include "mycc-libs/vector.h"

#include "location.h"

#include <stdbool.h>
#include <stdio.h>

struct ast_translation_unit_node {
	// struct ast_location location;
	struct vector* external_decls;
};

bool ast_translation_unit_node_init(struct ast_translation_unit_node* translation_unit);
struct ast_translation_unit_node* ast_translation_unit_node_new();
struct ast_translation_unit_node* ast_translation_unit_node_delete(struct ast_translation_unit_node* translation_unit);

void ast_print_translation_unit_node(FILE* file, struct ast_translation_unit_node const* translation_unit, size_t indent_level);
