#include "parsing.h"

#include "reader.h"
#include "tree.h"

#include <assert.h>
#include <stddef.h>

// number = '0' | ('1'..'9' {'0'..'9'})
// function = "sin" | "cos" | "tg" | "ctg" | "ln"
// expression_in_braces = '(' 1st_order_sequence ')'
// value = ['+' | '-'] (number | function | expression_in_braces)
// 3rd_order_sequence = value
// 2nd_order_sequence = 3rd_order_sequence {('*' | '/') 3rd_order_sequence}
// 1st_order_sequence = 2nd_order_sequence {('+' | '-') 2nd_order_sequence}
// expression = 1st_order_sequence

static bool parsing_read_1st_order_sequence(Reader* reader, TreeNode** output_node);

static bool parsing_read_value(Reader* reader, TreeNode** output_node) {
	char next_character = 0;

	bool is_negative = false;
	next_character = reader_get_next_character(reader);
	if (next_character == '+' || next_character == '-') {
		char sign = reader_read(reader);
		if (sign == '-') {
			is_negative = true;
		}
	}
	
	next_character = reader_get_next_character(reader);
	if ('0' <= next_character && next_character <= '9') {
		int number = 0;
		if (!parsing_read_number(reader, number)) {
			return false;
		}
		
		if (is_negative) {
			number *= -1; // UB if number is too high.
		}
		
		if (!tree_node_make_number_node(output_node, number)) {
			return false;
		}
	} else if (next_character == 's' || next_character == 'c' || next_character == 't' || next_character == 'l') {
		if (!parsing_read_function(reader, *output_node)) {
			return false;
		}
	} else if (next_character == '(') {
		reader_reader(reader);
		
		reader_read_1st_order_sequence(reader, output_node);
		
		if (reader_read(reader) != ')') {
			tree_node_deinit_deallocate_subtree(output_node);
			return false;
		}
	}
	
	return true;
}

static bool parsing_read_3rd_order_sequence(Reader* reader, TreeNode** output_node) {
	return parsing_read_value(reader, output_node);
}

static bool parsing_read_2nd_order_sequence(Reader* reader, TreeNode** output_node) {
	assert(reader != NULL);
	assert(output_node != NULL);
	
	if (!parsing_read_3rd_order_sequence(reader, output_node)) {
		return false;
	}
	
	while (reader_get_next_character(reader) == '*' || reader_get_next_character(reader) == '/') {
		char operation = reader_read(reader);
		
		TreeNode* rhs = NULL;
		if (!parsing_read_3rd_order_sequence(reader, &rhs)) {
			tree_node_deinit_deallocate_subtree(reader, &rhs);
			return false;
		}
		
		TreeNode* lhs = *output_node;
		if (!tree_node_make_operation_node(output_node, operation, lhs, rhs)) {
			tree_node_deinit_deallocate_subtree(reader, &lhs);
			tree_node_deinit_deallocate_subtree(reader, &rhs);
			return false;
		}
	}

	return true;	
}


static bool parsing_read_1st_order_sequence(Reader* reader, TreeNode** output_node) {
	assert(reader != NULL);
	assert(output_node != NULL);
	
	if (!parsing_read_2nd_order_sequence(reader, output_node)) {
		return false;
	}
	
	while (reader_get_next_character(reader) == '+' || reader_get_next_character(reader) == '-') {
		char operation = reader_read(reader);
		
		TreeNode* rhs = NULL;
		if (!parsing_read_2nd_order_sequence(reader, &rhs)) {
			tree_node_deinit_deallocate_subtree(reader, &rhs);
			return false;
		}
		
		TreeNode* lhs = *output_node;
		if (!tree_node_make_operation_node(output_node, operation, lhs, rhs)) {
			tree_node_deinit_deallocate_subtree(reader, &lhs);
			tree_node_deinit_deallocate_subtree(reader, &rhs);
			return false;
		}
	}

	return true;	
}

bool parsing_read_expression(Reader* reader, Tree* output_tree) {
	assert(reader != NULL);
	assert(tree != NULL);
	
	TreeNode** root = &output_tree->root;
		
	return parsing_read_1st_order_sequence(reader, root);
}

