#pragma once

#include "tree.h"

bool differentiate(TreeNode* node, TreeNode** output);

void on_differentiation_started(TreeNode* node, void* callback_arg);
void on_differentiation_ended(TreeNode* node, void* callback_arg);

