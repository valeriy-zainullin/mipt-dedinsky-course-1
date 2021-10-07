#define INCLUDED_FROM_STACK_IMPLEMENTATION
#include "stack.h"

#include "variable_location.h"

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
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
	VariableLocation definition_location;
	STACK_ITEM_TYPE data[1];
	// data...
	// const CANARY_TYPE canary;
};
typedef struct StackImpl StackImpl;
#define STACK_IMPL_SIZE_FOR_CAPACITY(CAPACITY) (sizeof(StackImpl) + (CAPACITY - 1) * sizeof(STACK_ITEM_TYPE) + sizeof(CANARY_TYPE))
#define STACK_IMPL_CANARY_AT_THE_END(STACK_IMPL_PTR) (* (const CANARY_TYPE*) (STACK_IMPL_PTR->data + STACK_IMPL_PTR->capacity))

// www.cse.yorku.ca/~oz/hash.html
static unsigned long long hash(const unsigned char* bytes, size_t length) {
	unsigned long long hash = 5381;
	for (size_t i = 0; i < length; ++i) {
		hash = ((hash << 5) + hash) + bytes[i];
	}
	return hash;
}

static unsigned long long calculate_struct_hash(const StackImpl* stack_impl_ptr) {
	return hash((const unsigned char*) &stack_impl_ptr->size, (size_t) ((char*) stack_impl_ptr->data - (char*) &stack_impl_ptr->size));
}

static unsigned long long calculate_data_hash(const StackImpl* stack_impl_ptr) {
	return hash((const unsigned char*) stack_impl_ptr->data, stack_impl_ptr->capacity * sizeof(STACK_ITEM_TYPE));
}

enum ValidityFlags {
	NOT_VALID = 0,
	VALID = 1,
	NOT_CHECKED = 2
};
typedef enum ValidityFlags ValidityFlag;

struct StackValidityInformation {
	ValidityFlag front_canary_validity;
	ValidityFlag size_validity; // size <= capacity
	ValidityFlag struct_validity; // hash of struct corresponds to the hash of the struct at the momenct
	ValidityFlag data_validity; // hash of data corresponds to the hash of the data at the moment
	ValidityFlag back_canary_validity;
};
typedef struct StackValidityInformation StackValidityInformation;
static bool all_validity_flags_are_set(StackValidityInformation validity_information) {
	return validity_information.front_canary_validity == VALID && validity_information.size_validity == VALID && validity_information.struct_validity == VALID && validity_information.data_validity == VALID && validity_information.back_canary_validity == VALID;
}

static StackValidityInformation validate_stack(const StackImpl* stack_impl_ptr) {
	StackValidityInformation validity_information;
	validity_information.front_canary_validity = (ValidityFlag) (stack_impl_ptr->canary == CANARY_VALUE);
	validity_information.size_validity = (ValidityFlag) (stack_impl_ptr->size <= stack_impl_ptr->capacity);
	validity_information.struct_validity = (ValidityFlag) (stack_impl_ptr->struct_hash == calculate_struct_hash(stack_impl_ptr));
	if (validity_information.struct_validity) {
		// We skip checking of data if struct is not valid as we can segfault while doing so.
		validity_information.data_validity = (ValidityFlag) (stack_impl_ptr->data_hash == calculate_data_hash(stack_impl_ptr));
		validity_information.back_canary_validity = STACK_IMPL_CANARY_AT_THE_END(stack_impl_ptr) == CANARY_VALUE;
	} else {
		validity_information.data_validity = NOT_CHECKED;
		validity_information.back_canary_validity = NOT_CHECKED;
	}
	return validity_information;
}

#if defined(__GNUC__)
#define UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#define UNREACHABLE __assume(0)
#else
#define UNREACHABLE
#endif

static void print_validity(ValidityFlag validity_flag) {
	switch (validity_flag) {
		case VALID: fprintf(stderr, "ok"); break;
		case NOT_VALID: fprintf(stderr, "BAD!"); break;
		case NOT_CHECKED: fprintf(stderr, "NOT CHECKED"); break;
		default: assert(0); UNREACHABLE;
	}
}

#define EXPANDED_MACRO_VALUE_TO_STRING(MACRO) #MACRO
#define MACRO_VALUE_TO_STRING(MACRO) EXPANDED_MACRO_VALUE_TO_STRING(MACRO)
static void dump_stack(VariableLocation variable_location, StackValidityInformation validity_information, const StackImpl* stack_impl_ptr) {
	fprintf(stderr, "stack<%s>[%p] \"%s\" from %s(%zu), %s (issued for line %zu of \"%s\", function \"%s\"):\n", MACRO_VALUE_TO_STRING(STACK_ITEM_TYPE), (void*) stack_impl_ptr, stack_impl_ptr->definition_location.variable, stack_impl_ptr->definition_location.source_file, stack_impl_ptr->definition_location.line, stack_impl_ptr->definition_location.function, variable_location.line, variable_location.source_file, variable_location.function);
	fprintf(stderr, "canary = %llx (", stack_impl_ptr->canary);
	print_validity(validity_information.front_canary_validity);
	fputs(")\n", stderr);

	fprintf(stderr, "struct hash = %llx\n", stack_impl_ptr->struct_hash);
	fprintf(stderr, "data hash = %llx\n", stack_impl_ptr->data_hash);

	fputs("--- Struct hash validity: ", stderr);
	print_validity(validity_information.struct_validity);
	fputs(" ---\n", stderr);

	fprintf(stderr, "size = %zu (", stack_impl_ptr->size);
	print_validity(validity_information.size_validity);
	fprintf(stderr, ")\ncapacity = %zu\n", stack_impl_ptr->capacity);
	fputs("------\n", stderr);

	fputs("--- Data hash validity: ", stderr);
	print_validity(validity_information.data_validity);
	fputs(" ---\n", stderr);
	fprintf(stderr, "data[%p] {\n", (void*) stack_impl_ptr->data);

	for (size_t i = 0; i < stack_impl_ptr->capacity; ++i) {
		fprintf(stderr, "\t[%zu] = %d", i, stack_impl_ptr->data[i]);
		if (stack_impl_ptr->data[i] == STACK_POISON) {
			fputs(" (POISON)", stderr);
		}
		fputs(",\n", stderr);
	}
	fputs("}\n", stderr);
	fputs("------\n", stderr);
	fprintf(stderr, "canary = %llx (", STACK_IMPL_CANARY_AT_THE_END(stack_impl_ptr));
	print_validity(validity_information.back_canary_validity);
	fputs(").\n", stderr);
}

static void ensure_stack_is_valid(VariableLocation variable_location, const StackImpl* stack_impl_ptr) {
	StackValidityInformation stack_validity_information = validate_stack(stack_impl_ptr);
	if (!all_validity_flags_are_set(stack_validity_information)) {
		dump_stack(variable_location, stack_validity_information, stack_impl_ptr);
		fprintf(stderr, "Stack is not valid.\n");
		exit(STACK_VERIFICATION_FAILED_EXIT_CODE);
	}
}

static void update_hashes(StackImpl* stack_impl_ptr) {
	// Now we are using the fact that we are zeroing new items and the struct itself when initializing it as we are zeroing padding bytes as well.
	stack_impl_ptr->struct_hash = calculate_struct_hash(stack_impl_ptr);
	stack_impl_ptr->data_hash = calculate_data_hash(stack_impl_ptr);
}

bool STACK_INIT_FUNCTION_NAME(VariableLocation definition_location, STACK_TYPE_NAME* stack_ptr) {
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

	stack_impl_ptr->definition_location = definition_location;
	update_hashes(stack_impl_ptr);

	return true;
}

void STACK_DEINIT_FUNCTION_NAME(VariableLocation variable_location, STACK_TYPE_NAME* stack_ptr) {
	assert(stack_ptr != NULL);

	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;
	ensure_stack_is_valid(variable_location, stack_impl_ptr);

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

bool STACK_PUSH_FUNCTION_NAME(VariableLocation variable_location, STACK_TYPE_NAME* stack_ptr, STACK_ITEM_ACCEPTANCE_TYPE item) {
	assert(stack_ptr != NULL);

	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;
	ensure_stack_is_valid(variable_location, stack_impl_ptr);

	if (!ensure_stack_has_space_for_new_item(stack_ptr)) {
		return false;
	}
	stack_impl_ptr = (StackImpl*) *stack_ptr;

#if STACK_ACCEPTS_ITEMS_BY_POINTERS
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

bool STACK_POP_FUNCTION_NAME(VariableLocation variable_location, STACK_TYPE_NAME* stack_ptr, STACK_ITEM_TYPE* item_ptr) {
	assert(stack_ptr != NULL);
	assert(item_ptr != NULL);

	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;
	ensure_stack_is_valid(variable_location, stack_impl_ptr);
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
