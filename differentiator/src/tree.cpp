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

void tree_node_deinit_deallocate_subtree(TreeNode** node) {
	if ((*node)->lhs != NULL) {
		tree_node_deinit_deallocate_subtree(&(*node)->lhs);
	}

	if ((*node)->rhs != NULL) {
		tree_node_deinit_deallocate_subtree(&(*node)->rhs);
	}
	
	if ((*node->inner != NULL) {
		tree_node_deinit_deallocate_subtree(&(*node)->inner);
	}
	
	tree_node_deinit(&node);
	tree_node_deallocate(node);
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

bool tree_node_make_variable_node(TreeNode** node, const char* name) {
	assert(node != NULL);
	assert(strlen(name) <= TREE_MAX_NAME_LENGTH);
	
	if (!tree_node_allocate(node)) {
		return false;
	}
	
	(*node)->type = TREE_NODE_TYPE_VARIABLE;
	strcpy((*node)->name, name);
	
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

bool tree_node_make_function_node(TreeNode** node, const char* function, TreeNode* inner) {
	assert(node != NULL);
	assert(strlen(function) <= TREE_MAX_NAME_LENGTH);
	
	if (!tree_node_allocate(node)) {
		return false;
	}
	
	(*node)->type = TREE_NODE_TYPE_FUNCTION;
	strcpy((*node)->name, function);
	(*node)->inner = inner;
	
	return true;
}

