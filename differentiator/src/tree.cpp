#include "tree.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

bool tree_node_allocate(TreeNode** node) {
	assert(node != NULL);
	
	*node = (TreeNode*) calloc(1, sizeof(TreeNode));
	
	return *node != NULL;
}

void tree_node_init(TreeNode* node) {
	// memset? All fields zero.
	(void) node;
	node->lhs = NULL;
	node->rhs = NULL;
	node->inner = NULL;
	// TODO: other fields.
}

void tree_node_deinit(TreeNode* node) {
	// memset? All fields zero.
	(void) node;
}

void tree_node_deallocate(TreeNode** node) {
	assert(node != NULL);
	assert(*node != NULL);

	free(*node);
	*node = NULL;
}

bool tree_node_copy_subtree(TreeNode* node, TreeNode** output_node) {
	assert(node != NULL);
	
	switch (node->type) {
		case TREE_NODE_TYPE_OPERATION: {
			TreeNode* lhs = NULL;
			if (!tree_node_copy_subtree(node->lhs, &lhs)) {
				return false;
			}
			
			TreeNode* rhs = NULL;
			if (!tree_node_copy_subtree(node->rhs, &rhs)) {
				tree_node_deinit_deallocate_subtree(&lhs);
				return false;
			}
			
			if (!tree_node_make_operation_node(output_node, node->operation, lhs, rhs)) {
				tree_node_deinit_deallocate_subtree(&lhs);
				tree_node_deinit_deallocate_subtree(&rhs);
				return false;
			}
			
			break;
		}
		
		case TREE_NODE_TYPE_FUNCTION: {
			TreeNode* inner = NULL;
			if (!tree_node_copy_subtree(node->inner, &inner)) {
				return false;
			}
			
			if (!tree_node_make_function_node(output_node, node->name, inner)) {
				tree_node_deinit_deallocate_subtree(&inner);
				return false;
			}
			
			break;
		}
		
		case TREE_NODE_TYPE_NUMBER: {
			if (!tree_node_make_number_node(output_node, node->number)) {
				return false;
			}
			
			break;
		}
		
		case TREE_NODE_TYPE_VARIABLE: {
			if (!tree_node_make_variable_node(output_node, node->name)) {
				return false;
			}
			
			break;
		}
		
		default: assert(false); UNREACHABLE;
	}

	return true;
}

void tree_node_deinit_deallocate_subtree(TreeNode** node) {
	if ((*node)->lhs != NULL) {
		tree_node_deinit_deallocate_subtree(&(*node)->lhs);
	}

	if ((*node)->rhs != NULL) {
		tree_node_deinit_deallocate_subtree(&(*node)->rhs);
	}
	
	if ((*node)->inner != NULL) {
		tree_node_deinit_deallocate_subtree(&(*node)->inner);
	}
	
	tree_node_deinit(*node);
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

