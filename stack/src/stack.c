#define INCLUDED_FROM_STACK_IMPLEMENTATION
#include "stack.h"

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define STACK_CANARY_TYPE unsigned long long
#define STACK_CANARY_SIZE sizeof(unsigned long long)

struct StackImpl {
	const STACK_CANARY_TYPE canary;
	size_t size;
	size_t capacity;
	STACK_ITEM_TYPE* data[0];
	// data...
	// const STACK_CANARY_TYPE canary;
};
typedef struct StackImpl StackImpl;
#define STACK_IMPL_REAL_SIZE(STACK_IMPL_PTR) (sizeof(StackImpl) + (STACK_IMPL_PTR)->capacity * sizeof(STACK_ITEM_TYPE) + sizeof(STACK_CANARY_TYPE))
#define STACK_IMPL_CANARY_AT_THE_END(STACK_IMPL_PTR) (* (STACK_CANARY_TYPE*) ((STACK_IMPL_PTR) + sizeof(StackImpl) + stack_impl->capacity * sizeof(STACK_ITEM_TYPE)))

static unsigned long long CANARY_VALUE = 0xCD07B10913AE98FALL;

bool STACK_INIT_FUNCTION_NAME(Stack* stack_ptr) {
	assert(stack_ptr != NULL);

	const size_t STACK_IMPL_INITIAL_SIZE = sizeof(StackImpl) + STACK_CANARY_SIZE;
	*stack_ptr = malloc(STACK_IMPL_INITIAL_SIZE);
	if (*stack_ptr == NULL) {
		return false;
	}
	memset(*stack_ptr, 0, STACK_IMPL_INITIAL_SIZE);

	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;
	* (unsigned long long*) &stack_impl_ptr->canary = CANARY_VALUE;
	STACK_IMPL_CANARY_AT_THE_END(stack_impl_ptr) = CANARY_VALUE;
}

static bool stack_ensure_has_space_for_new_item(Stack* stack_ptr) {
	assert(stack_ptr != NULL);
	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;

	if (stack_impl_ptr->size + 1 <= stack_impl_ptr->capacity) {
		return false;
	}

	size_t new_capacity = stack_ptr

	void* new_stack_ptr = realloc(*stack_ptr, sizeof(StackImpl) + sizeof(char) * new_capacity + CANARY_SIZE);
	if (*new_stack_ptr == NULL) {
		return false;
	}

	*stack_ptr = new_stack_ptr;
	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;
	stack_impl_ptr->capacity = new_capacity;
	STACK_IMPL_CANARY_AT_THE_END(stack_impl_ptr) = CANARY_VALUE;

	return true;
}

bool STACK_PUSH_FUNCTION_NAME(Stack* stack_ptr, STACK_ITEM_ACCEPTANCE_TYPE item) {
	assert(stack_ptr != NULL);

	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;
	if (stack_impl_ptr->size + TYPE_SIZE > stack_impl_ptr->capacity) {
		if (!stack_expand(stack_ptr)) {
			return false;
		}
	}

#if defined(STACK_ACCEPTS_ITEMS_BY_POINTERS)
	stack_impl_ptr->data[stack_impl_ptr->size] = *item;
#else
	stack_impl_ptr->data[stack_impl_ptr->size] = item;
#endif
	stack_impl_ptr->size += 1;

	return true;
}

static void stack_shrink_if_needed(Stack* stack_ptr) {
	assert(stack_ptr != NULL);

	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;
	if (stack_impl_ptr->size <= stack_impl_ptr->capacity / 4) {
		void* new_stack_ptr = realloc(*stack_ptr, STACK_IMPL_REAL_SIZE(stack_impl_ptr));
		if (new_stack_ptr == NULL) {
			// Failed to shrink.
			return;
		}
		*stack_ptr = new_stack_ptr;
	}
}

bool STACK_POP_FUNCTION_NAME(Stack* stack_ptr, STACK_ITEM_TYPE* item_ptr) {
	assert(stack_ptr != NULL);
	assert(item_ptr != NULL);

	if (stack_impl_ptr->size == 0) {
		return false;
	}

	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;
	*item_ptr = stack_impl_ptr->data[stack_impl_ptr->size - 1];
	stack_impl_ptr->size -= 1;

	stack_shrink_if_needed(stack_ptr);

	return true;
}
