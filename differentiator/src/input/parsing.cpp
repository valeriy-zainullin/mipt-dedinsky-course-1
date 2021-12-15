#include "input/parsing.h"

#include "input/reader.h"
#include "tree.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

// number = '0' | ('1'..'9' {'0'..'9'})
// OLD: // function = ("sin" | 'c' ("os" | "tg") | "tg" | "ln") '(' 1st_order_sequence ')'
// name = 'a'..'z' ['a'..'z' ['a'..'z' ... ] ]
//   Длина не более 16 (TREE_MAX_NAME_LENGTH).
// expression_in_braces = '(' 1st_order_sequence ')'
// value = ['+' | '-'] (number | name ['(' 1st_order_sequence ')'] | expression_in_braces)
//   Если есть скобки у name, значит это функция. Поддерживаются функции sin, cos, tg, ctg, ln
// 3rd_order_sequence = value
// 2nd_order_sequence = 3rd_order_sequence {('*' | '/') 3rd_order_sequence}
// 1st_order_sequence = 2nd_order_sequence {('+' | '-') 2nd_order_sequence}
// expression = 1st_order_sequence

static bool parsing_read_1st_order_sequence(Reader* reader, TreeNode** output_node);

static bool parsing_read_number(Reader* reader, int* number) {
	*number = 0;

	char next_character = reader_get_next_char(reader);
	if (next_character == '0') {
		*number = 0;
	} else if ('0' <= next_character && next_character <= '9') {
		for (size_t i = 0; i < 9 && '0' <= next_character && next_character <= '9'; ++i) {
			*number = *number * 10 + (reader_read(reader) - '0');
			next_character = reader_get_next_char(reader);
		}
	} else {
		return false;
	}
	
	return true;
}

static bool parsing_read_name(Reader* reader, char* output) {
	char next_char = reader_get_next_char(reader);
	if (next_char < 'a' || next_char > 'z') {
		return false;
	}
	
	*output = reader_read(reader);
	output += 1;
	
	for (size_t i = 0; i < TREE_MAX_NAME_LENGTH - 1; ++i) {
		next_char = reader_get_next_char(reader);
		if (next_char < 'a' || next_char > 'z') {
			break;
		}
		
		*output = reader_read(reader);
		output += 1;
	}
	
	return true;
}

static bool parsing_read_value(Reader* reader, TreeNode** output_node) {
	char next_character = 0;

	bool is_negative = false;
	next_character = reader_get_next_char(reader);
	if (next_character == '+' || next_character == '-') {
		char sign = reader_read(reader);
		if (sign == '-') {
			is_negative = true;
		}
	}
	
	next_character = reader_get_next_char(reader);
	if ('0' <= next_character && next_character <= '9') {
		int number = 0;
		if (!parsing_read_number(reader, &number)) {
			return false;
		}
		
		if (!tree_node_make_number_node(output_node, number)) {
			return false;
		}
	} else if ('a' <= next_character && next_character <= 'z') {
		char name[TREE_MAX_NAME_LENGTH + 1] = {};
		if (!parsing_read_name(reader, name)) {
			return false;
		}
		
		if (reader_get_next_char(reader) == '(') {
			reader_read(reader);
			
			parsing_read_1st_order_sequence(reader, output_node);
			
			if (reader_read(reader) != ')') {
				tree_node_deinit_deallocate_subtree(output_node);
				return false;
			}
			
			TreeNode* inner = *output_node;
			if (!tree_node_make_function_node(output_node, name, inner)) {
				tree_node_deinit_deallocate_subtree(&inner);
				return false;
			}
		} else {
			if (!tree_node_make_variable_node(output_node, name)) {
				return false;
			}
		}
	} else if (next_character == '(') {
		reader_read(reader);
		
		parsing_read_1st_order_sequence(reader, output_node);
		
		if (reader_read(reader) != ')') {
			tree_node_deinit_deallocate_subtree(output_node);
			return false;
		}
	}
	
	if (is_negative) {
	
		TreeNode* lhs = NULL;
		if (!tree_node_make_number_node(&lhs, -1)) {
			tree_node_deinit_deallocate_subtree(output_node);
			return false;
		}
		
		TreeNode* rhs = *output_node;
		if (!tree_node_make_operation_node(output_node, '*', lhs, rhs)) {
			tree_node_deinit_deallocate_subtree(&lhs);
			tree_node_deinit_deallocate_subtree(&rhs);
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
	
	while (reader_get_next_char(reader) == '*' || reader_get_next_char(reader) == '/') {
		char operation = reader_read(reader);
		
		TreeNode* rhs = NULL;
		if (!parsing_read_3rd_order_sequence(reader, &rhs)) {
			tree_node_deinit_deallocate_subtree(output_node);
			return false;
		}
		
		TreeNode* lhs = *output_node;
		if (!tree_node_make_operation_node(output_node, operation, lhs, rhs)) {
			tree_node_deinit_deallocate_subtree(&lhs);
			tree_node_deinit_deallocate_subtree(&rhs);
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
	
	while (reader_get_next_char(reader) == '+' || reader_get_next_char(reader) == '-') {
		char operation = reader_read(reader);
		
		TreeNode* rhs = NULL;
		if (!parsing_read_2nd_order_sequence(reader, &rhs)) {
			tree_node_deinit_deallocate_subtree(&rhs);
			return false;
		}
		
		TreeNode* lhs = *output_node;
		if (!tree_node_make_operation_node(output_node, operation, lhs, rhs)) {
			tree_node_deinit_deallocate_subtree(&lhs);
			tree_node_deinit_deallocate_subtree(&rhs);
			return false;
		}
	}

	return true;	
}

bool parsing_read_expression(Reader* reader, Tree* output_tree) {
	assert(reader != NULL);
	assert(output_tree != NULL);
	
	TreeNode** root = &output_tree->root;
		
	return parsing_read_1st_order_sequence(reader, root);
}

