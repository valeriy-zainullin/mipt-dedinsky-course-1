#include "differentiation.h"

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

bool differentiate(TreeNode* node, TreeNode** output, void* callback_arg) {
	// bool success = false;
	// TreeNode** dsl_output = ...;
	#define OPERATION_NODE(OPERATION, LHS, RHS) {                         \
		TreeNode* dsl_cur_node = (TreeNode*) calloc(1, sizeof(TreeNode)); \
		if (dsl_cur_node == NULL) {                                       \
			success = false;                                              \
		} else {                                                          \
			dsl_cur_node->type = TREE_NODE_TYPE_OPERATION;                \
			dsl_cur_node->operation = OPERATION;                          \
			TreeNode** dsl_old_output = dsl_output;                       \
			TreeNode** dsl_output = &dsl_cur_node->lhs;                   \
			LHS                                                           \
			if (!success) {                                               \
				free(dsl_cur_node);                                       \
			} else {                                                      \
				dsl_output = &dsl_cur_node->rhs;                          \
				RHS                                                       \
				if (!success) {                                           \
					free(dsl_cur_node->lhs);                              \
					free(dsl_cur_node);                                   \
				} else {                                                  \
					*dsl_old_output = dsl_cur_node;                       \
				}                                                         \
			}                                                             \
		}                                                                 \
	}
	
	// bool success = false;
	// TreeNode** dsl_output = ...;
	#define NUMBER_NODE(NUMBER) {                                         \
		TreeNode* dsl_cur_node = (TreeNode*) calloc(1, sizeof(TreeNode)); \
		if (dsl_cur_node == NULL) {                                       \
			success = false;                                              \
		} else {                                                          \
			dsl_cur_node->type = TREE_NODE_TYPE_NUMBER;                   \
			dsl_cur_node->number = NUMBER;                                \
			*dsl_output = dsl_cur_node;                                   \
		}                                                                 \
	}
	
	// bool success = false;
	// TreeNode** dsl_output = ...;
	#define FUNCTION_NODE(FUNCTION, INNER) {                              \
		TreeNode* dsl_cur_node = (TreeNode*) calloc(1, sizeof(TreeNode)); \
		if (dsl_cur_node == NULL) {                                       \
			success = false;                                              \
		} else {                                                          \
			dsl_cur_node->type = TREE_NODE_TYPE_FUNCTION;                 \
			strcpy(dsl_cur_node->function, FUNCTION);                     \
			TreeNode** dsl_old_output = dsl_output;                       \
			TreeNode** dsl_output = &dsl_cur_node->inner;                 \
			INNER                                                         \
			if (!success) {                                               \
				free(dsl_cur_node);                                       \
			} else {                                                      \
				*dsl_old_output = dsl_cur_node;                           \
			}                                                             \
		}                                                                 \
	}

	#define LHS   *dsl_output = node->lhs;
	#define RHS   *dsl_output = node->rhs;
	#define INNER *dsl_output = node->inner;
	
	// bool success = false;
	// TreeNode** dsl_output = ...;
	#define DIFFERENTIATE(INNER) {                                 \
		TreeNode** dsl_old_output = dsl_output;                    \
		TreeNode* dsl_cur_node = NULL;                             \
		TreeNode** dsl_output = &dsl_cur_node;                     \
		INNER                                                      \
		if (success) {                                             \
			success = differentiate(dsl_cur_node, dsl_old_output); \
		}                                                          \
	}
	
	on_differentiation_started(node, callback_arg);
	
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
		#define FUNCTION_RULE(FUNCTION, CONVOLUTION)       \
				if (strcmp(node->function, FUNCTION) == 0) \
					CONVOLUTION                            \
				else
		#define END_FUNCTION_RULES()                 \
				{                                    \
					assert(false); success = false;  \
				}                                    \
				break;                               \
			}

		case TREE_NODE_TYPE_NUMBER: NUMBER_NODE(node->number); break;
		
		#include "rules.h"
		
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
	
	on_differentiation_ended(node, output, callback_arg);
	
	return success;
}

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning( pop )
#endif

