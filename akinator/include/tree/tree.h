#pragma once

#include "macro_utils.h"

struct TreeNode;

struct Tree {
	TreeNode* root;
};

struct TreeNode {
	TreeNode* left;
	TreeNode* right;
};

MAY_BE_UNUSED static int TREE_DIRECTION_NONE = 0;
MAY_BE_UNUSED static int TREE_DIRECTION_LEFT = 1;
MAY_BE_UNUSED static int TREE_DIRECTION_RIGHT = 2;

typedef int (*TreeOnNodeVisitedCallback)(TreeNode* node, void* arg);

void tree_visit_depth_first(Tree* tree, TreeOnNodeVisitedCallback callback);
