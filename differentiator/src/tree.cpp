#include "tree.h"

#include <stddef.h>

void tree_node_allocate(TreeNode** node) {
	assert(node != NULL);
	
	*node = (TreeNode*) calloc(1, sizeof(TreeNode));
	
	return *node != NULL;
}

void tree_node_init(TreeNode* node) {}

void tree_node_deinit(TreeNode* node) {}

void tree_node_deallocate(TreeNode** node) {
	assert(node != NULL);
	assert(*node != NULL);

	free(*node);
	*node = NULL;
}

bool tree_node_make_number_node(TreeNode** node, int number) {
	assert(node != NULL);
	
	if (!tree_node_allocate(node)) {
		return false;
	}
			
	(*node)->type = TREE_NODE_TYPE_NUMBER;
	(*node)->number = number;
	
	return true;
}

bool tree_node_make_operation_node(TreeNode** node, char operation, TreeNode* lhs, TreeNode* rhs) {
	assert(node != NULL);
	
	if (!tree_node_allocate(node)) {
		return false;
	}
	
	(*node)->type = TREE_NODE_TYPE_OPERATION;
	(*node)->operation = operation;
	(*node)->lhs = lhs;
	(*node)->rhs = rhs;
	
	return true;
}

bool tree_node_make_function_node(TreeNode** node, char* function, TreeNode* inner) {
	assert(node != NULL);
	
	if (!tree_node_allocate(node)) {
		return false;
	}
	
	(*node)->type = TREE_NODE_TYPE_FUNCTION;
	(*node)->operation = operation;
	(*node)->inner = inner;
	
	return true;
}

