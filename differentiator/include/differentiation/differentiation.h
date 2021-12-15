#pragma once

#include "tree.h"

#include <stdbool.h>

typedef void (*OnDifferentiationStartedFunction)(void* callback_arg, TreeNode* node);
typedef void (*OnDifferentiationEndedFunction)(void* callback_arg, TreeNode* node, TreeNode** output_node);

struct DifferentiationCallbacks {
	OnDifferentiationStartedFunction on_differentiation_started;
	OnDifferentiationEndedFunction on_differentiation_ended;
	void* arg;
};

bool differentiate(Tree* input_tree, Tree* output_tree, DifferentiationCallbacks* callbacks);

