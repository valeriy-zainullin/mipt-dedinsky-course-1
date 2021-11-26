#include <stddef.h>
#include <stdlib.h>

static const size_t MAX_TOKEN_LENGTH = 16;

enum TreeNodeType {
	TREE_NODE_TYPE_NUMBER,
	TREE_NODE_TYPE_OPERATION,
	TREE_NODE_TYPE_FUNCTION
};

struct TreeNode {
	TreeNodeType type;
	
	int number;
	// BigInteger number;
	char operation;
	char FUNCTION[MAX_TOKEN_LENGTH + 1];
	
	TreeNode* lhs;
	TreeNode* rhs;
	TreeNode* inner;
};

void differentiate(TreeNode* node, TreeNode** output);
void differentiate(TreeNode* node, TreeNode** output) {
	TreeNode* new_node = calloc(1, sizeof(TreeNode));
	if (new_node == NULL) {
		return;
	}
	
	switch (node->type) {
		case TREE_NODE_TYPE_NUMBER: {
			new_node->type = TREE_NODE_TYPE_NUMBER;
			new_node->number = node->number;
		}
		
		case TREE_NODE_TYPE_OPERATION: {
			new_node->type = TREE_NODE_TYPE_OPERATION;
			
			switch (node->operation) {
				case '*': {
					new_node->operation = '+';
					
					TreeNode* lhs = (TreeNode*) calloc(1, sizeof(TreeNode));
					if (lhs == NULL) {
						return;
					}
					lhs->type = TREE_NODE_TYPE_OPERATION;
					lhs->operation = '*';
					differentiate(node->lhs, &lhs->lhs);
					lhs->rhs = copy(node->rhs);
					new_node->lhs = lhs;
					
					TreeNode* rhs = (TreeNode*) calloc(1, sizeof(TreeNode));
					if (rhs == NULL) {
						return;
					}
					rhs->type = TREE_NODE_TYPE_OPERATION;
					rhs->operation = '*';
					rhs->lhs = copy(node->lhs);
					differentiate(node->rhs, &rhs->rhs);
					new_node->rhs = rhs;
				}
			}
		}
		
		case TREE_NODE_TYPE_FUNCTION: {
			if (strcmp(new_node->function, "sin") == 0) {
				new_node->type = TREE_NODE_TYPE_OPERATION;
				new_node->operation = '*';
				
				TreeNode* lhs = calloc(1, sizeof(TreeNode));
				lhs->type = TREE_NODE_TYPE_FUNCTION;
				lhs->function = "cos";
				lhs->inner = node->inner;
				new_node->lhs = lhs;
				
				differentiate(node->inner, &new_node->rhs);
			}

			else if (strcmp(new_node->function, "cos") == 0) {
				new_node->type = TREE_NODE_TYPE_OPERATION;
				new_node->operation = '*';
				
				TreeNode* lhs = calloc(1, sizeof(TreeNode));
				if (lhs == NULL) {
					return;
				}
				lhs->type = TREE_NODE_TYPE_OPERATION;
				lhs->operation = '*';
				lhs->lhs = calloc(1, sizeof(TreeNode));
				if (lhs->lhs == NULL) {
					// free
					return;
				}
				lhs->lhs->type = TREE_NODE_TYPE_NUMBER;
				lhs->lhs->number = -1;
				
				lhs->rhs = calloc(1, sizeof(TreeNode));
				lhs->rhs->type = TREE_NODE_TYPE_FUNCTION;
				lhs->rhs->function = "cos";
				lhs->rhs->inner = node->inner;
				
				new_node->lhs = lhs;
				
				differentiate(node->inner, &new_node->rhs);
			}
		}
	}
}

int main() {
	return 0;
}
