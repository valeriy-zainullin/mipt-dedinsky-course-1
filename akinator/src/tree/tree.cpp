#include "tree/tree.h"

#include "tree/macro_utils.h"
#include "tree/stack.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

bool tree_node_init(TreeNode** node, const char* value) {
	*node = (TreeNode*) calloc(1, sizeof(TreeNode));
	if (*node == NULL) {
		return false;
	}
	
	(*node)->left = NULL;
	(*node)->right = NULL;
	strncpy((*node)->value, value, TREE_MAX_STRING_LENGTH);

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

static int tree_deinit_callback_on_enter(TreeNode* node, void* arg) {
	(void) node;
	(void) arg;
	return TREE_DIRECTION_LEFT | TREE_DIRECTION_RIGHT;	
}

static void take_deinit_callback_on_leave(TreeNode* node, void* arg) {
	(void) arg;
	tree_node_deinit(&node);
}

bool tree_deinit(Tree* tree) {
	return tree_visit_depth_first(tree, tree_deinit_callback_on_enter, take_deinit_callback_on_leave, NULL);
}

bool tree_visit_depth_first(Tree* tree, TreeOnNodeEnteredCallback on_node_entered_callback, TreeOnNodeLeftCallback on_node_left_callback, void* arg) {
	assert(tree != NULL);
	assert(on_node_entered_callback != NULL);
	assert(on_node_left_callback != NULL);

	if (tree->root == NULL) {
		return true;
	}

	StackVisitingIteration stack = {};
	if (!stack_visiting_iteration_init(&stack)) {
		LOG_ERROR("Failed to allocate tree node stack.\n");
		return false;
	}

	{
		VisitingIteration iteration = {};
		iteration.node = tree->root;
		iteration.type = ITERATION_TYPE_ENTERING;
		// Удалось занести?
		stack_visiting_iteration_push(&stack, iteration);
	}

	while (true) {
		
		VisitingIteration iteration = {};
		
		if (!stack_visiting_iteration_pop(&stack, &iteration)) {
			break;
		}
		
		TreeNode* node = iteration.node;
		IterationType iteration_type = iteration.type;
		
		switch (iteration_type) {
			case ITERATION_TYPE_ENTERING: {

				int directions = on_node_entered_callback(node, arg);
				
				VisitingIteration leaving_iteration = {};
				leaving_iteration.node = node;
				leaving_iteration.type = ITERATION_TYPE_LEAVING;
				
				if (!stack_visiting_iteration_push(&stack, leaving_iteration)) {
					LOG_ERROR_FOR_NODE(node, "failed to push node onto the stack.\n");
					stack_visiting_iteration_deinit(&stack);
					return false;
				}
				
				VisitingIteration child_iteration = {};
				child_iteration.type = ITERATION_TYPE_ENTERING;

				child_iteration.node = node->right;
				if (node->right != NULL && (directions & TREE_DIRECTION_RIGHT) != 0 && !stack_visiting_iteration_push(&stack, child_iteration)) {
					LOG_ERROR_FOR_NODE(node->right, "failed to push the right child of %p.\n", (void*) node);
					stack_visiting_iteration_deinit(&stack);
					return false;
				}

				child_iteration.node = node->left;
				if (node->left != NULL && (directions & TREE_DIRECTION_LEFT) != 0 && !stack_visiting_iteration_push(&stack, child_iteration)) {
					LOG_ERROR_FOR_NODE(node->left, "failed to push the left child of %p.\n", (void*) node);
					stack_visiting_iteration_deinit(&stack);
					return false;
				}
				
				break;
			}
			
			case ITERATION_TYPE_LEAVING: {

				if (on_node_left_callback != NULL) {
					on_node_left_callback(node, arg);
				}
				
				break;
			}
			
			default:
				UNREACHABLE;
		}
	}

	stack_visiting_iteration_deinit(&stack);

	return true;
}
