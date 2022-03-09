#pragma once

#include "tree.h"

#include <stddef.h>
#include <stdio.h>

struct LatexWriter {
	FILE* stream;
	const char* output_file;
	char* tmp_file;
	
	bool is_debug;
	int step;
};

bool latex_writer_init(LatexWriter* latex_writer, const char* output_file, bool is_debug);
void latex_writer_deinit(LatexWriter* latex_writer);

void latex_writer_before_differentiation(void* latex_writer, const Tree* tree, Tree* simplified_tree);
void latex_writer_on_differentiation_started(void* latex_writer, const TreeNode* node);
void latex_writer_on_differentiation_ended(void* latex_writer, const TreeNode* node, TreeNode** output_node);
void latex_writer_after_differentiation(void* latex_writer, const Tree* tree);

void latex_writer_print_expression(LatexWriter* graphviz_writer, const TreeNode* node);
