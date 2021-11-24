#pragma once

#include "akinator/vector.h"
#include "tree/tree.h"

enum AkinatorMode {
	AKINATOR_MODE_GUESS,
	AKINATOR_MODE_COMPARE,
	AKINATOR_MODE_GET_DEFINITION,
	AKINATOR_MODE_DUMP,
	AKINATOR_MODE_EXIT,
	AKINATOR_MODE_COUNT
};

AkinatorMode akinator_read_mode();


int akinator_find_node_by_value_callback_on_enter(TreeNode* node, void* arg);
void akinator_find_node_by_value_callback_on_leave(TreeNode* node, void* arg);
TreeNode* akinator_find_node_by_value(Tree* tree, char* value, VectorString* characteristics);

int akinator_guess_object_callback(TreeNode* node, void* arg);
void akinator_process_guess_request(Tree* tree);

void akinator_process_comparison_request(Tree* tree);
void akinator_process_definition_request(Tree* tree);

