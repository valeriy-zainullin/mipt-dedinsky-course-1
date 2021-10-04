#define INCLUDED_FROM_STACK_IMPLEMENTATION
#include "stack.h"

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define CANARY_TYPE unsigned long long
#define CANARY_SIZE sizeof(unsigned long long)
static unsigned long long CANARY_VALUE = 0xCD07B10913AE98FALL;

struct StackImpl {
	const CANARY_TYPE canary;
	unsigned long long struct_hash;
	unsigned long long data_hash;
	size_t size;
	size_t capacity;
	STACK_ITEM_TYPE data[1];
	// data...
	// const CANARY_TYPE canary;
};
typedef struct StackImpl StackImpl;
#define STACK_IMPL_SIZE_FOR_CAPACITY(CAPACITY) (sizeof(StackImpl) + (CAPACITY - 1) * sizeof(STACK_ITEM_TYPE) + sizeof(CANARY_TYPE))
#define STACK_IMPL_CANARY_AT_THE_END(STACK_IMPL_PTR) (* (const CANARY_TYPE*) (STACK_IMPL_PTR->data + STACK_IMPL_PTR->capacity))

// www.cse.yorku.ca/~oz/hash.html
static unsigned long long hash(unsigned char* bytes, size_t length) {
	unsigned long hash = 5381;
	for (size_t i = 0; i < length; ++i) {
		hash = ((hash << 5) + hash) + bytes[i];
	}
	return hash;
}

static unsigned long long calculate_struct_hash(StackImpl* stack_impl_ptr) {
	return hash((unsigned char*) &stack_impl_ptr->size, (size_t) ((char*) stack_impl_ptr->data - (char*) &stack_impl_ptr->size));
}

static unsigned long long calculate_data_hash(StackImpl* stack_impl_ptr) {
	return hash((unsigned char*) (stack_impl_ptr->data + 1), (stack_impl_ptr->capacity - 1) * sizeof(STACK_ITEM_TYPE));
}

//static unsigned long long validate_hashes

static bool validate_hashes(StackImpl* stack_impl_ptr) {
	
}

#define VALIDATE_STACK(STACK_IMPL_PTR, SOURCE_LOCATION) \
	if (!validate_stack(STACK_IMPL_PTR)) {                \
		dump_stack(STACK_IMPL_PTR);                         \
		fprintf(stderr, "Stack is not valid.\n");           \
		exit(STACK_VERIFICATION_FAILED_EXIT_CODE);          \
	}

static void update_hashes(StackImpl* stack_impl_ptr) {
	// Now we are using the fact that we are zeroing new items and the struct itself when initializing it as we are zeroing padding bytes as well.
	stack_impl_ptr->struct_hash = calculate_struct_hash(stack_impl_ptr);
	stack_impl_ptr->data_hash = calculate_data_hash(stack_impl_ptr);
}

bool STACK_INIT_FUNCTION_NAME(STACK_TYPE_NAME* stack_ptr) {
	assert(stack_ptr != NULL);

	static const size_t STACK_IMPL_INITIAL_CAPACITY = 1; // Can't be zero, because we allocate space for 1 element in the structure itself.
	const size_t STACK_IMPL_INITIAL_SIZE = STACK_IMPL_SIZE_FOR_CAPACITY(STACK_IMPL_INITIAL_CAPACITY);
	*stack_ptr = malloc(STACK_IMPL_INITIAL_SIZE);
	if (*stack_ptr == NULL) {
		return false;
	}
	memset(*stack_ptr, 0, STACK_IMPL_INITIAL_SIZE);

	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;
	stack_impl_ptr->size = 0;
	stack_impl_ptr->capacity = STACK_IMPL_INITIAL_CAPACITY;
	// Dropping constnesses and initializing canaries.
	* (CANARY_TYPE*) &stack_impl_ptr->canary = CANARY_VALUE;
	* (CANARY_TYPE*) &STACK_IMPL_CANARY_AT_THE_END(stack_impl_ptr) = CANARY_VALUE;
	update_hashes(stack_impl_ptr);
	return true;
}

void STACK_DEINIT_FUNCTION_NAME(STACK_TYPE_NAME* stack_ptr) {
	assert(stack_ptr != NULL);

	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;
	VALIDATE_STACK(stack_impl_ptr);

	free(*stack_ptr);
	*stack_ptr = NULL;
}

static bool ensure_stack_has_space_for_new_item(STACK_TYPE_NAME* stack_ptr) {
	assert(stack_ptr != NULL);
	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;

	if (stack_impl_ptr->size + 1 <= stack_impl_ptr->capacity) {
		return false;
	}

	size_t new_capacity = stack_impl_ptr->capacity * 2;

	void* new_stack_ptr = realloc(*stack_ptr, STACK_IMPL_SIZE_FOR_CAPACITY(new_capacity));
	if (new_stack_ptr == NULL) {
		return false;
	}
	*stack_ptr = new_stack_ptr;
	stack_impl_ptr = (StackImpl*) *stack_ptr;

	// Zeroing new items as we will be referencing them for hashing and dumping.
	// Otherwise we don't know the value stored there. And C standard says it is undefined behaviour to read uninitialized data.
	memset(stack_impl_ptr->data + stack_impl_ptr->capacity, 0, (new_capacity - stack_impl_ptr->capacity) * sizeof(STACK_ITEM_TYPE));

	stack_impl_ptr->capacity = new_capacity;
	* (CANARY_TYPE*) STACK_IMPL_CANARY_AT_THE_END(stack_impl_ptr) = CANARY_VALUE;

	return true;
}

bool STACK_PUSH_FUNCTION_NAME(STACK_TYPE_NAME* stack_ptr, STACK_ITEM_ACCEPTANCE_TYPE item) {
	assert(stack_ptr != NULL);

	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;
	VALIDATE_STACK(stack_impl_ptr);

	if (!ensure_stack_has_space_for_new_item(stack_ptr)) {
		return false;
	}
	stack_impl_ptr = (StackImpl*) *stack_ptr;

#if defined(STACK_ACCEPTS_ITEMS_BY_POINTERS)
	stack_impl_ptr->data[stack_impl_ptr->size] = *item;
#else
	stack_impl_ptr->data[stack_impl_ptr->size] = item;
#endif
	stack_impl_ptr->size += 1;
	update_hashes(stack_impl_ptr);

	return true;
}

static void shrink_stack_if_needed(STACK_TYPE_NAME* stack_ptr) {
	assert(stack_ptr != NULL);

	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;
	if (stack_impl_ptr->size <= stack_impl_ptr->capacity / 4) {
		size_t new_capacity = stack_impl_ptr->capacity / 4;
		if (new_capacity < 1) {
			// We can't have capacity be less than 1, as one element is allocated in the structure itself.
			new_capacity = 1;
		}
		void* new_stack_ptr = realloc(*stack_ptr, STACK_IMPL_SIZE_FOR_CAPACITY(new_capacity));
		if (new_stack_ptr == NULL) {
			// Failed to shrink.
			return;
		}
		*stack_ptr = new_stack_ptr;
	}
}

bool STACK_POP_FUNCTION_NAME(STACK_TYPE_NAME* stack_ptr, STACK_ITEM_TYPE* item_ptr) {
	assert(stack_ptr != NULL);
	assert(item_ptr != NULL);

	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;
	VALIDATE_STACK(stack_impl_ptr);
	if (stack_impl_ptr->size == 0) {
		return false;
	}
	*item_ptr = stack_impl_ptr->data[stack_impl_ptr->size - 1];
	stack_impl_ptr->size -= 1;

	shrink_stack_if_needed(stack_ptr);
	stack_impl_ptr = (StackImpl*) *stack_ptr;

	update_hashes(stack_impl_ptr);

	return true;
}
