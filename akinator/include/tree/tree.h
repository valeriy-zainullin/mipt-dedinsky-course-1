#pragma once

#include "macro_utils.h"

struct TreeNode;

struct Tree {
	TreeNode* root;
};

#define TREE_MAX_STRING_LENGTH_MACRO 256
static const int TREE_MAX_STRING_LENGTH = TREE_MAX_STRING_LENGTH_MACRO;

struct TreeNode {
	TreeNode* left;
	TreeNode* right;

	char value[TREE_MAX_STRING_LENGTH + 1];
};

bool tree_node_init(TreeNode** node, const char* value);
void tree_node_deinit(TreeNode** node);

MAY_BE_UNUSED static int TREE_DIRECTION_NONE = 0;
MAY_BE_UNUSED static int TREE_DIRECTION_LEFT = 1;
MAY_BE_UNUSED static int TREE_DIRECTION_RIGHT = 2;

typedef int (*TreeOnNodeVisitedCallback)(TreeNode* node, void* arg);

void tree_init(Tree* tree);
bool tree_deinit(Tree* tree);

bool tree_visit_depth_first(Tree* tree, TreeOnNodeVisitedCallback callback, void* arg);
