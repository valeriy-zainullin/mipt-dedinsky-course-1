#include "tree/tree.h"

#include "tree/stack.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define LOG_ERROR_FOR_NODE(NODE, ...) fprintf(stderr, "Node@%p: ", (void*) NODE); LOG_ERROR(__VA_ARGS__)

void tree_node_init(TreeNode** node, const char* value) {
	*node = (TreeNode*) calloc(1, sizeof(TreeNode));
	if (*node == NULL) {
		return false;
	}
	
	object_node->left = NULL;
	object_node->right = NULL;
	strcpy(object_node->value, line);

	return true;
}

void tree_node_deinit(TreeNode** node) {
	memset(*node, 0, sizeof(TreeNode));
	free(*node);
	*node = NULL;
}

void tree_init(Tree* tree) {
	tree->root = NULL;
}

bool tree_deinit(Tree* tree) {
	if (tree->root == NULL) {
		return true;
	}

	StackTreeNode stack = {};

	if (!stack_tree_node_init(&stack)) {
		stack_tree_node_deinit(&stack);
		LOG_ERROR("Failed to allocate stack");
		return false;
	}

	if (!stack_tree_node_push(&stack, tree->root)) {
		stack_tree_node_deinit(&stack);
		LOG_ERROR("Failed to push the root onto the stack");
		return false;
	}

	while (true) {
		TreeNode* node = NULL;

		if (!stack_tree_node_pop(&stack, &node)) {
			break;
		}

		if (node->right != NULL && !stack_tree_node_push(&stack, node->right)) {
			stack_tree_node_deinit(&stack);
			LOG_ERROR_FOR_NODE(node->right, "failed to push onto the stack");
			return false;
		}

		if (node->left != NULL && !stack_tree_node_push(&stack, node->left)) {
			stack_tree_node_deinit(&stack);
			LOG_ERROR_FOR_NODE(node->left, "failed to push onto the stack");
			return false;
		}

		tree_node_deinit(&node);
	}

	stack_tree_node_deinit(&stack);

	return true;
}

bool tree_visit_depth_first(Tree* tree, TreeOnNodeVisitedCallback callback, void* arg) {
	assert(tree != NULL);
	assert(callback != NULL);

	if (tree->root == NULL) {
		return true;
	}

	StackTreeNode stack = {};
	if (!stack_tree_node_init(&stack)) {
		LOG_ERROR("Failed to allocate tree node stack.\n");
		return false;
	}

	// Удалось занести?
	stack_tree_node_push(&stack, tree->root);

	while (true) {
		
		TreeNode* node = NULL;
		
		if (!stack_tree_node_pop(&stack, &node)) {
			break;
		}

		int directions = callback(node, arg);

		if (node->right != NULL && (directions & TREE_DIRECTION_RIGHT) != 0 && !stack_tree_node_push(&stack, node->right)) {
			LOG_ERROR_FOR_NODE(node->right, "failed to push the right child of %p.\n", (void*) node);
			return false;
		}

		if (node->left != NULL && (directions & TREE_DIRECTION_LEFT) != 0 && !stack_tree_node_push(&stack, node->left)) {
			LOG_ERROR_FOR_NODE(node->left, "failed to push the left child of %p.\n", (void*) node);
			return false;
		}


	}

	stack_tree_node_deinit(&stack);

	return true;
}