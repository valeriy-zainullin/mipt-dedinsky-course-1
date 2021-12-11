#pragma once

#include <stddef.h>

enum TreeNodeType {
	TREE_NODE_TYPE_NUMBER,
	TREE_NODE_TYPE_OPERATION,
	TREE_NODE_TYPE_FUNCTION
};

static const size_t TREE_MAX_FUNCTION_LENGTH = 16;

struct TreeNode {
	TreeNodeType type;
	
	int number;
	//BigInteger number;
	char operation;
	char function[TREE_MAX_FUNCTION_LENGTH + 1];
	
	TreeNode* lhs;
	TreeNode* rhs;
	TreeNode* inner;
};

void tree_node_allocate(TreeNode** node);
void tree_node_init(TreeNode* node);
void tree_node_deinit(TreeNode* node);
void tree_node_deallocate(TreeNode** node);

bool tree_node_make_number_node(TreeNode** node, int number);
bool tree_node_make_operation_node(TreeNode** node, char operation, TreeNode* lhs, TreeNode* rhs);
bool tree_node_make_function_node(TreeNode** node, const char* function, TreeNode* inner);

