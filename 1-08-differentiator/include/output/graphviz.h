#pragma once

#include "tree.h"

#include <stdbool.h>
#include <stdio.h>

struct GraphvizWriter {
	FILE* stream;
	const char* output_file;
	char* tmp_file;

	bool is_debug;
	int step;
};

bool graphviz_writer_init(GraphvizWriter* graphviz_writer, const char* output_file, bool is_debug);
void graphviz_writer_deinit(GraphvizWriter* graphviz_writer);

void graphviz_writer_before_differentiation(void* graphviz_writer, const Tree* tree, Tree* simplified_tree);
void graphviz_writer_on_differentiation_started(void* graphviz_writer, const TreeNode* node);
void graphviz_writer_on_differentiation_ended(void* graphviz_writer, const TreeNode* node, TreeNode** output_node);
void graphviz_writer_after_differentiation(void* graphviz_writer, const Tree* tree);

void graphviz_writer_print_expression(GraphvizWriter* graphviz_writer, const TreeNode* node);
