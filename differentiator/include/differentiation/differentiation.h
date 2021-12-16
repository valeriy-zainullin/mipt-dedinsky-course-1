#pragma once

#include "tree.h"

#include <stdbool.h>

typedef void (*BeforeDifferentiationFunction)(void* callback_arg, const Tree* tree);
typedef void (*OnDifferentiationStartedFunction)(void* callback_arg, const TreeNode* node);
typedef void (*OnDifferentiationEndedFunction)(void* callback_arg, const TreeNode* node, TreeNode** output_node);
typedef void (*AfterDifferentiationFunction)(void* callback_arg, const Tree* differentiated_tree);

struct DifferentiationCallbacks {
	BeforeDifferentiationFunction before_differentiation;
	OnDifferentiationStartedFunction on_differentiation_started;
	OnDifferentiationEndedFunction on_differentiation_ended;
	AfterDifferentiationFunction after_differentiation;
	void* arg;
};

bool differentiate(const Tree* input_tree, Tree* output_tree, DifferentiationCallbacks* callbacks);

