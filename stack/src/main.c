#include "stack.h"

#include <assert.h>
#include <stdio.h>

int main() {
	StackInt stack;
	if (!stack_int_init(&stack)) {
		printf("Failed to initialize stack.\n");
	}
	stack_int_deinit(&stack);
	return 0;
}
