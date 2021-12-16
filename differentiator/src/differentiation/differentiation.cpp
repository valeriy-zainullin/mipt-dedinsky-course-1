#include "differentiation/differentiation.h"

#include "tree.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#if defined(__GNUC__) || defined(__clang__)
#define UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#define UNREACHABLE __assume(0)
#endif

// Shadowing is allowed for this function. It is used for DSL.

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#elif defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4456 ) // объявление "идентификатор" скрывает предыдущее локальное объявление
#endif

// TODO: codestyle для C. _ref.
static void simplify(TreeNode** node_ref) {
	(void) simplify;
	TreeNode* node = *node_ref;
	
	#define REPLACE_NODE_WITH_LHS() {                    \
		*node_ref = node->lhs;                           \
		                                                 \
		tree_node_deinit_deallocate_subtree(&node->rhs); \
		                                                 \
		tree_node_deinit(node);                          \
		tree_node_deallocate(&node);                     \
	}
	
	#define REPLACE_NODE_WITH_RHS() {                    \
		*node_ref = node->rhs;                           \
		                                                 \
		tree_node_deinit_deallocate_subtree(&node->lhs); \
		                                                 \
		tree_node_deinit(node);                          \
		tree_node_deallocate(&node);                     \
	}
	
	#define REPLACE_NODE_WITH_NUMBER(NUMBER) {               \
		if (!tree_node_make_number_node(node_ref, NUMBER)) { \
			return;                                          \
		}                                                    \
		                                                     \
		tree_node_deinit_deallocate_subtree(&node);          \
	}
	
	if (node->type == TREE_NODE_TYPE_OPERATION && node->operation == '*') {
		assert(node->lhs != NULL);
		assert(node->rhs != NULL);
		
		if (node->lhs->type == TREE_NODE_TYPE_NUMBER && node->rhs->type == TREE_NODE_TYPE_NUMBER) {
			REPLACE_NODE_WITH_NUMBER(node->lhs->number * node->rhs->number);
			return;
		}
		
		if (node->lhs->type == TREE_NODE_TYPE_NUMBER && node->lhs->number == 0) {
			REPLACE_NODE_WITH_NUMBER(0);
			return;
		}
		
		if (node->rhs->type == TREE_NODE_TYPE_NUMBER && node->rhs->number == 0) {
			REPLACE_NODE_WITH_NUMBER(0);
			return;
		}
		
		if (node->lhs->type == TREE_NODE_TYPE_NUMBER && node->lhs->number == 1) {
			REPLACE_NODE_WITH_RHS();
			return;
		}
		
		if (node->rhs->type == TREE_NODE_TYPE_NUMBER && node->rhs->number == 1) {
			REPLACE_NODE_WITH_LHS();
			return;
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

static void simplify_recursively(TreeNode** node_ref) {
	TreeNode* node = *node_ref;
	
	if (node->type == TREE_NODE_TYPE_OPERATION) {
		simplify_recursively(&node->lhs);
		simplify_recursively(&node->rhs);
	} else if (node->type == TREE_NODE_TYPE_FUNCTION) {
		simplify_recursively(&node->inner);
	}
	
	simplify(node_ref);
}

static bool differentiate_node(const TreeNode* node, TreeNode** output, DifferentiationCallbacks* callbacks) {
	// bool success = false;
	// TreeNode** dsl_output = ...;
	#define OPERATION_NODE(OPERATION, LHS, RHS) {                                             \
		TreeNode** dsl_old_output = dsl_output;                                               \
		                                                                                      \
		TreeNode* lhs = NULL;                                                                 \
		dsl_output = &lhs;                                                                    \
		LHS                                                                                   \
		                                                                                      \
		if (success) {                                                                        \
			TreeNode* rhs = NULL;                                                             \
			dsl_output = &rhs;                                                                \
			RHS                                                                               \
			                                                                                  \
			if (!success) {                                                                   \
				tree_node_deinit_deallocate_subtree(&lhs);                                    \
			} else if (!tree_node_make_operation_node(dsl_old_output, OPERATION, lhs, rhs)) { \
				success = false;                                                              \
				                                                                              \
				tree_node_deinit_deallocate_subtree(&lhs);                                    \
				tree_node_deinit_deallocate_subtree(&rhs);                                    \
			} else {                                                                          \
				simplify(dsl_old_output);                                                     \
			}                                                                                 \
		}                                                                                     \
	}
	
	// bool success = false;
	// TreeNode** dsl_output = ...;
	#define NUMBER_NODE(NUMBER) {                              \
		if (!tree_node_make_number_node(dsl_output, NUMBER)) { \
			success = false;                                   \
		}                                                      \
	}
	
	// bool success = false;
	// TreeNode** dsl_output = ...;
	#define FUNCTION_NODE(FUNCTION, INNER) {                                  \
		TreeNode** dsl_old_output = dsl_output;                               \
		                                                                      \
		TreeNode* inner = NULL;                                               \
		dsl_output = &inner;                                                  \
		INNER                                                                 \
		                                                                      \
		if (!tree_node_make_function_node(dsl_old_output, FUNCTION, inner)) { \
			success = false;                                                  \
			                                                                  \
			tree_node_deinit_deallocate_subtree(&inner);                      \
		} else {                                                              \
			simplify(dsl_old_output);                                         \
		}                                                                     \
	}

	#define LHS {                                             \
		if (!tree_node_copy_subtree(node->lhs, dsl_output)) { \
			success = false;                                  \
		}                                                     \
		simplify_recursively(dsl_output);                       \
	}
	#define RHS {                                             \
		if (!tree_node_copy_subtree(node->rhs, dsl_output)) { \
			success = false;                                  \
		}                                                     \
		simplify_recursively(dsl_output);                       \
	}
	#define INNER {                                             \
		if (!tree_node_copy_subtree(node->inner, dsl_output)) { \
			success = false;                                    \
		}                                                       \
		simplify_recursively(dsl_output);                       \
	}
	
	// bool success = false;
	// TreeNode** dsl_output = ...;
	#define DIFFERENTIATE(INNER) {                                                    \
		TreeNode** dsl_old_output = dsl_output;                                       \
		TreeNode* dsl_cur_node = NULL;                                                \
		TreeNode** dsl_output = &dsl_cur_node;                                        \
		INNER                                                                         \
		if (success) {                                                                \
			success = differentiate_node(dsl_cur_node, dsl_old_output, callbacks);    \
		}                                                                             \
	}
	
	(*callbacks->on_differentiation_started)(callbacks->arg, node);
	
	bool success = true;
	TreeNode** dsl_output = output;

	switch (node->type) {
	
		#define BEGIN_OPERATION_RULES()      \
			case TREE_NODE_TYPE_OPERATION: { \
				switch (node->operation) {
		#define OPERATION_RULE(OPERATION, CONVOLUTION) \
					case OPERATION: CONVOLUTION break;
		#define END_OPERATION_RULES()                        \
					default: assert(false); success = false; \
				}                                            \
				break;                                       \
			}

		#define BEGIN_FUNCTION_RULES()      \
			case TREE_NODE_TYPE_FUNCTION: {
		#define FUNCTION_RULE(FUNCTION, CONVOLUTION)            \
				if (strcmp(node->name, FUNCTION) == 0)          \
					CONVOLUTION                                 \
				else
		#define END_FUNCTION_RULES()                 \
				{                                    \
					assert(false); success = false;  \
				}                                    \
				break;                               \
			}

		case TREE_NODE_TYPE_NUMBER: NUMBER_NODE(0); break;
		case TREE_NODE_TYPE_VARIABLE: NUMBER_NODE(1); break;
		
		#include "differentiation/rules.h"
		
		default: assert(false); UNREACHABLE;
		
		#undef BEGIN_OPERATION_RULES
		#undef OPERATION_RULE
		#undef END_OPERATION_RULES
		#undef BEGIN_FUNCTION_RULES
		#undef FUNCTION_RULE
		#undef END_FUNCTION_RULES
	}
		
	#undef OPERATION_NODE
	#undef NUMBER_NODE
	#undef FUNCTION_NODE
	
	(*callbacks->on_differentiation_ended)(callbacks->arg, node, output);
	
	return success;
}

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning( pop )
#endif

bool differentiate(const Tree* input_tree, Tree* output_tree, DifferentiationCallbacks* callbacks) {
	(*callbacks->before_differentiation)(callbacks->arg, input_tree);
	
	if (!differentiate_node(input_tree->root, &output_tree->root, callbacks)) {
		return false;
	}
	
	(*callbacks->after_differentiation)(callbacks->arg, output_tree);

	return true;
}

