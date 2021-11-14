#include "tree/tree.h"

#include "tree/stack.h"

#include <assert.h>

#define LOG_ERROR(...) fprintf(stderr, __VA_ARGS__)

bool tree_visit_depth_first(Tree* tree, TreeOnNodeVisitedCallback callback, void* arg) {
	assert(tree != NULL);
	assert(callback != NULL);

	StackTreeNode* stack = NULL;
	if (!stack_tree_node_init(&stack)) {
		LOG_ERROR("Failed to allocate tree node stack.\n");
		return false;
	}

	stack_tree_node_push(&stack, tree->root);

	while (true) {
		
		TreeNode* node = NULL;
		
		if (!stack_tree_node_pop(&stack, &node)) {
			break;
		}

		int directions = callback(node, arg);

		if ((directions & TREE_DIRECTION_RIGHT) != 0 && !stack_tree_node_push(&stack, node->right)) {
			LOG_ERROR("Failed to push right of %p onto the stack.\n", node);
			return false;
		}

		if ((directions & TREE_DIRECTION_LEFT) != 0 && !stack_tree_node_push(&stack, node->left)) {
			LOG_ERROR("Failed to push left of %p onto the stack.\n", node);
			return false;
		}


	}

	stack_tree_node_deinit(&stack);
}