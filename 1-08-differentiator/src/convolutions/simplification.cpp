#include "convolutions/simplification.h"

#include "tree.h"

#include <assert.h>
#include <stdbool.h>

// TODO: codestyle для C. _ref.
//static void simplify(TreeNode** node_ref, bool* simplified) {
static void simplify_node(TreeNode** node_ref, bool* error, bool* simplified) {
	(void) simplify;
	TreeNode* node = *node_ref;
	
	#define REPLACE_NODE_WITH_LHS() {                    \
		*node_ref = node->lhs;                           \
		                                                 \
		tree_node_deinit_deallocate_subtree(&node->rhs); \
		                                                 \
		tree_node_deinit(node);                          \
		tree_node_deallocate(&node);                     \
		                                                 \
		*simplified = true;                              \
		return;                                          \
	}
	
	#define REPLACE_NODE_WITH_RHS() {                    \
		*node_ref = node->rhs;                           \
		                                                 \
		tree_node_deinit_deallocate_subtree(&node->lhs); \
		                                                 \
		tree_node_deinit(node);                          \
		tree_node_deallocate(&node);                     \
		                                                 \
		*simplified = true;                              \
		return;                                          \
	}
	
	// TODO: if we fail to make node, we bork node_ref. Should we exit recursive simplification immediately?
	#define REPLACE_NODE_WITH_NUMBER(NUMBER) {               \
		if (!tree_node_make_number_node(node_ref, NUMBER)) { \
			*error = true;                                   \
			return;                                          \
		}                                                    \
		                                                     \
		tree_node_deinit_deallocate_subtree(&node);          \
		                                                     \
		*simplified = true;                                  \
		return;                                              \
	}
	
	if (node->type == TREE_NODE_TYPE_OPERATION && node->operation == '*') {
		assert(node->lhs != NULL);
		assert(node->rhs != NULL);
		
		if (node->lhs->type == TREE_NODE_TYPE_NUMBER && node->rhs->type == TREE_NODE_TYPE_NUMBER) {
			REPLACE_NODE_WITH_NUMBER(node->lhs->number * node->rhs->number);
		}
		
		if (node->lhs->type == TREE_NODE_TYPE_NUMBER && node->lhs->number == 0) {
			REPLACE_NODE_WITH_NUMBER(0);
		}
		
		if (node->rhs->type == TREE_NODE_TYPE_NUMBER && node->rhs->number == 0) {
			REPLACE_NODE_WITH_NUMBER(0);
		}
		
		if (node->lhs->type == TREE_NODE_TYPE_NUMBER && node->lhs->number == 1) {
			REPLACE_NODE_WITH_RHS();
		}
		
		if (node->rhs->type == TREE_NODE_TYPE_NUMBER && node->rhs->number == 1) {
			REPLACE_NODE_WITH_LHS();
		}
		
		if (node->rhs->type == TREE_NODE_TYPE_NUMBER) {
			TreeNode* tmp = node->lhs;
			node->lhs = node->rhs;
			node->rhs = tmp;

			*simplified = true;
			return;
		}
		
		if (
			node->lhs->type == TREE_NODE_TYPE_NUMBER &&
			node->rhs->type == TREE_NODE_TYPE_OPERATION &&
			node->rhs->operation == '*' &&
			node->rhs->lhs->type == TREE_NODE_TYPE_NUMBER
		) {
			node->rhs->lhs->number *= node->lhs->number;
			REPLACE_NODE_WITH_RHS();
		}
	}
	
	if (node->type == TREE_NODE_TYPE_OPERATION && node->operation == '+') {
		assert(node->lhs != NULL);
		assert(node->rhs != NULL);
		
		if (node->lhs->type == TREE_NODE_TYPE_NUMBER && node->rhs->type == TREE_NODE_TYPE_NUMBER) {
			REPLACE_NODE_WITH_NUMBER(node->lhs->number + node->rhs->number);
			return;
		}
		
		if (node->lhs->type == TREE_NODE_TYPE_NUMBER && node->lhs->number == 0) {
			REPLACE_NODE_WITH_RHS();
			return;
		}
		
		if (node->rhs->type == TREE_NODE_TYPE_NUMBER && node->rhs->number == 0) {
			REPLACE_NODE_WITH_LHS();
			return;
		}
	}
}

static void simplify_recursively(TreeNode** node_ref, bool* error, bool* simplified) {
	TreeNode* node = *node_ref;
	
	if (node->type == TREE_NODE_TYPE_OPERATION) {
		simplify_recursively(&node->lhs, error, simplified);
		simplify_recursively(&node->rhs, error, simplified);
	} else if (node->type == TREE_NODE_TYPE_FUNCTION) {
		simplify_recursively(&node->inner, error, simplified);
	}
	
	simplify_node(node_ref, error, simplified);
}

bool simplify(TreeNode** node_ref) {
	bool error = false;
	bool simplified = true;

	do {
		simplified = false;
		simplify_recursively(node_ref, &error, &simplified);
	} while (simplified && !error);

	return !error;
}
