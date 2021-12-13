#pragma once

#include "tree.h"

#include <stdbool.h>

bool differentiate(TreeNode* node, TreeNode** output, void* callback_arg);

void on_differentiation_started(TreeNode* node, void* callback_arg);
void on_differentiation_ended(TreeNode* node, TreeNode** output_node, void* callback_arg);

