#pragma once

#include <stddef.h>

enum TreeNodeType {
	TREE_NODE_TYPE_NUMBER,
	TREE_NODE_TYPE_OPERATION,
	TREE_NODE_TYPE_FUNCTION
};

static const size_t MAX_FUNCTION_LENGTH = 16;

struct TreeNode {
	TreeNodeType type;
	
	int number;
	// BigInteger number;
	char operation;
	char function[MAX_FUNCTION_LENGTH + 1];
	
	TreeNode* lhs;
	TreeNode* rhs;
	TreeNode* inner;
};

