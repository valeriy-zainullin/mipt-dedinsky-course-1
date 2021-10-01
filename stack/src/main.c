#include "stack.h"

#include <assert.h>
#include <stdio.h>

int main() {
	puts(TMP2);
	StackInt stack;
	if (!stack_int_init(&stack)) {
		printf("Failed to initialize stack.\n");
	}
	return 0;
}
