#pragma once

#include "tree.h"

#include <stdbool.h>
#include <stdio.h>

struct TextWriter {
	FILE* stream;
	bool is_debug;
	int parent_operation_order;
};

bool text_writer_init(TextWriter* text_writer, const char* output_file);
void text_writer_deinit(TextWriter* text_writer);

void text_writer_before_differentiation(void* text_writer, const Tree* tree, Tree* simlified_tree);
void text_writer_on_differentiation_started(void* text_writer, const TreeNode* node);
void text_writer_on_differentiation_ended(void* text_writer, const TreeNode* node, TreeNode** output_node);
void text_writer_after_differentiation(void* text_writer, const Tree* tree);

void text_writer_print_expression(TextWriter* text_writer, const TreeNode* node);
