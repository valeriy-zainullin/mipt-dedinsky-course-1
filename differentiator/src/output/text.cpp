#include "output/text.h"

#include "output/operations.h"
#include "tree.h"

#include <stdio.h>

void text_writer_init(TextWriter* text_writer, FILE* output_stream) {
	text_writer->stream = output_stream;
}

void text_writer_deinit(TextWriter* text_writer) {
	(void) text_writer;
}

void text_writer_before_differentiation(TextWriter* text_writer, Tree* tree) {
	fprintf(text_writer->stream, "Исходное выражение: ");
	text_writer->parent_operation_order = OPERATION_ORDER_MAX;
	text_writer_print_expression(text_writer, tree->root);
	fprintf(text_writer->stream, ".\n");
}

void text_writer_on_differentiation_started(void* callback_arg, TreeNode* node) {
	(void) callback_arg;
	(void) node;
}

void text_writer_on_differentiation_ended(void* callback_arg, TreeNode* node, TreeNode** output_node) {
	TextWriter* text_writer = (TextWriter*) callback_arg;
	
	fprintf(text_writer->stream, "(");
	text_writer->parent_operation_order = OPERATION_ORDER_MAX;
	text_writer_print_expression(text_writer, node);
	fprintf(text_writer->stream, ")' = ");
	text_writer->parent_operation_order = OPERATION_ORDER_MAX;
	text_writer_print_expression(text_writer, *output_node);
	fprintf(text_writer->stream, ".\n");
}

void text_writer_after_differentiation(TextWriter* text_writer, Tree* tree) {
	fprintf(text_writer->stream, "Производная исходного выражения: ");
	text_writer->parent_operation_order = OPERATION_ORDER_MAX;
	text_writer_print_expression(text_writer, tree->root);
	fprintf(text_writer->stream, ".\n");
}

void text_writer_print_expression(TextWriter* text_writer, TreeNode* node) {
	assert(node != NULL);

	switch (node->type) {
		case TREE_NODE_TYPE_OPERATION: {
			int this_operation_order = operation_get_operation_order(node->operation);
			bool put_braces = text_writer->parent_operation_order < this_operation_order;
			if (put_braces) {
				fprintf(text_writer->stream, "(");
			}
			
			text_writer->parent_operation_order = this_operation_order;
			text_writer_print_expression(text_writer, node->lhs);
			
			fprintf(text_writer->stream, "%c", node->operation);
			
			text_writer->parent_operation_order = this_operation_order;
			text_writer_print_expression(text_writer, node->rhs);
			
			if (put_braces) {
				fprintf(text_writer->stream, ")");
			}
			
			break;
		}
		
		case TREE_NODE_TYPE_FUNCTION: {
			fprintf(text_writer->stream, "%s(", node->name);
			
			text_writer->parent_operation_order = OPERATION_ORDER_MAX;
			text_writer_print_expression(text_writer, node->inner);
			
			fprintf(text_writer->stream, ")");
			
			break;
		}
		
		case TREE_NODE_TYPE_VARIABLE: {
			fprintf(text_writer->stream, "%s", node->name);
			
			break;
		}
		
		case TREE_NODE_TYPE_NUMBER: {
			fprintf(text_writer->stream, "%d", node->number);
			
			break;
		}
		
		default: assert(false); UNREACHABLE;
	}
}

