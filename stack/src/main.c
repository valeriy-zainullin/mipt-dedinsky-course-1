#include "stack.h"

#include <assert.h>
#include <stdio.h>

int main() {
	StackInt stack;
	if (!stack_int_init(&stack)) {
		printf("Failed to initialize stack.\n");
	}
	// * ((size_t*) ((unsigned long long*) stack + 3) + 1) -= 1;
	for (int i = 0; i < 100; ++i) {
		assert(stack_int_push(&stack, i));
	}
	for (int i = 100 - 1; i >= 0; --i) {
		int value = 0;
		assert(stack_int_pop(&stack, &value));
		assert(value == i);
		for (int j = 0; j < 100; ++j) {
			assert(stack_int_push(&stack, j));
		}
		for (int j = 100 - 1; j >= 0; --j) {
			value = 0;
			assert(stack_int_pop(&stack, &value));
			assert(value == j);
		}
	}
	stack_int_deinit(&stack);
	return 0;
}
