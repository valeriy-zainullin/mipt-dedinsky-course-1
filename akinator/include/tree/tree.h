#pragma once

#include "tree/macro_utils.h"

#include "support/macro_utils.h"

struct TreeNode;

struct Tree {
	TreeNode* root;
};

#define TREE_MAX_STRING_LENGTH_MACRO 256
static const size_t TREE_MAX_STRING_LENGTH = TREE_MAX_STRING_LENGTH_MACRO;

struct TreeNode {
	TreeNode* left;
	TreeNode* right;

	char value[TREE_MAX_STRING_LENGTH + 1];
};

bool tree_node_init(TreeNode** node, const char* value);
void tree_node_deinit(TreeNode** node);

MAY_BE_UNUSED static const int TREE_DIRECTION_NONE = 0;
MAY_BE_UNUSED static const int TREE_DIRECTION_LEFT = 1;
MAY_BE_UNUSED static const int TREE_DIRECTION_RIGHT = 2;

typedef int (*TreeOnNodeEnteredCallback)(TreeNode* node, void* arg);
typedef void (*TreeOnNodeLeftCallback)(TreeNode* node, void* arg);

void tree_init(Tree* tree);
bool tree_deinit(Tree* tree);

bool tree_visit_depth_first(Tree* tree, TreeOnNodeEnteredCallback on_entered_callback, TreeOnNodeLeftCallback on_left_callback, void* arg);
