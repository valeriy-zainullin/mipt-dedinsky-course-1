#pragma once

#include "tree/tree.h"

enum IterationType {
	ITERATION_TYPE_ENTERING,
	ITERATION_TYPE_LEAVING
};

struct VisitingIteration {
	TreeNode* node;
	IterationType type;
};
