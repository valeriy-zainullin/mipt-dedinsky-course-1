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

typedef enum ValidityFlags {

	NOT_VALID   = 0,
	VALID 	    = 1,
	NOT_CHECKED = 2
} ValidityFlag;

typedef struct ValidityInfo {

	#if STACK_CANARY_PROTECTION_ENABLED
	ValidityFlag front_canary_validity;
	#endif

	// size <= capacity
	ValidityFlag size_validity;

	#if STACK_STRUCT_HASH_PROTECTION_ENABLED
	// hash of struct corresponds to the hash of the struct at the moment
	ValidityFlag struct_validity;
	#endif

	#if STACK_DATA_HASH_PROTECTION_ENABLED
	// hash of data corresponds to the hash of the data at the moment
	ValidityFlag data_validity;
	#endif

	#if STACK_CANARY_PROTECTION_ENABLED
	ValidityFlag back_canary_validity;
	#endif

} ValidityInfo;

struct StackImpl {

	#if STACK_CANARY_PROTECTION_ENABLED
		const CANARY_TYPE canary;
	#endif

	#if STACK_STRUCT_HASH_PROTECTION_ENABLED
		unsigned long long struct_hash;
	#endif

	#if STACK_DATA_HASH_PROTECTION_ENABLED
		unsigned long long data_hash;
	#endif

	size_t size;
	size_t capacity;

	VariableLocation definition_location;

	STACK_ITEM_TYPE data[1];// []

	// data...
	#if STACK_CANARY_PROTECTION_ENABLED
	// const CANARY_TYPE canary;
	#endif
};
typedef struct StackImpl StackImpl;

static size_t get_block_size(size_t capacity) {
	assert(capacity >= 1); // Zero capacity is forbidden. There is always memory for at least one element.

	#if STACK_CANARY_PROTECTION_ENABLED
		return sizeof(StackImpl) + (capacity - 1) * sizeof(STACK_ITEM_TYPE) + sizeof(CANARY_TYPE);
	#else
		return sizeof(StackImpl) + (capacity - 1) * sizeof(STACK_ITEM_TYPE);
	#endif
}

#define CANARY_AT_THE_END(PTR) (* (const CANARY_TYPE*) (PTR->data + PTR->capacity))

// www.cse.yorku.ca/~oz/hash.html
#if STACK_STRUCT_HASH_PROTECTION_ENABLED || STACK_DATA_HASH_PROTECTION_ENABLED
static unsigned long long get_gnu_hash(const unsigned char* bytes, size_t length) {

	unsigned long long hash = 5381;
	for (size_t i = 0; i < length; ++i) {
		hash = ((hash << 5) + hash) + bytes[i];
	}

	return hash;
}
#endif

#if STACK_STRUCT_HASH_PROTECTION_ENABLED
static unsigned long long calculate_struct_hash(const StackImpl* stack_impl_ptr) {
	return get_gnu_hash((const unsigned char*) &stack_impl_ptr->size, (size_t) ((char*) stack_impl_ptr->data -
																				(char*) &stack_impl_ptr->size));
}
#endif

#if STACK_DATA_HASH_PROTECTION_ENABLED
static unsigned long long calculate_data_hash(const StackImpl* stack_impl_ptr) {
	return get_gnu_hash((const unsigned char*) stack_impl_ptr->data, stack_impl_ptr->capacity * sizeof(STACK_ITEM_TYPE));
}
#endif

#if STACK_ANY_HASH_PROTECTION_ENABLED
static void update_hashes(StackImpl* stack_impl_ptr) {

	// Now we are using the fact that we are zeroing new items and the struct itself when initializing it as we are zeroing padding bytes as well.
#if STACK_STRUCT_HASH_PROTECTION_ENABLED
	stack_impl_ptr->struct_hash = calculate_struct_hash(stack_impl_ptr);
#endif

#if STACK_DATA_HASH_PROTECTION_ENABLED
	stack_impl_ptr->data_hash = calculate_data_hash(stack_impl_ptr);
#endif
}
#endif

// про флаги isAllFlagsValid (?)
static bool is_fully_valid(ValidityInfo validity) {

	return
	(
		#if STACK_CANARY_PROTECTION_ENABLED
				validity.front_canary_validity == VALID &&
		#endif
				validity.size_validity == VALID &&

		#if STACK_STRUCT_HASH_PROTECTION_ENABLED
				validity.struct_validity == VALID &&
		#endif

		#if STACK_DATA_HASH_PROTECTION_ENABLED
				validity.data_validity == VALID &&
		#endif

		#if STACK_CANARY_PROTECTION_ENABLED
				validity.back_canary_validity == VALID &&
		#endif

		true
	);
}

static ValidityInfo validate_stack(const StackImpl* stack_impl_ptr) {
	ValidityInfo validity;

#if STACK_CANARY_PROTECTION_ENABLED
	validity.front_canary_validity = (ValidityFlag) (stack_impl_ptr->canary == CANARY_VALUE);
	validity.data_validity = VALID;
	validity.back_canary_validity = VALID;
#endif

	validity.size_validity = (ValidityFlag) (stack_impl_ptr->size <= stack_impl_ptr->capacity);

#if STACK_STRUCT_HASH_PROTECTION_ENABLED
	validity.struct_validity = (ValidityFlag) (stack_impl_ptr->struct_hash == calculate_struct_hash(stack_impl_ptr));
#endif

#if STACK_STRUCT_HASH_PROTECTION_ENABLED && (STACK_CANARY_PROTECTION_ENABLED || STACK_DATA_HASH_PROTECTION_ENABLED)

	// We skip checking of data and back canary if struct is not valid as we can segfault while doing so.
	if (validity.struct_validity) {

		#if STACK_DATA_HASH_PROTECTION_ENABLED
				validity.data_validity = (ValidityFlag) (stack_impl_ptr->data_hash == calculate_data_hash(stack_impl_ptr));
		#endif

		#if STACK_CANARY_PROTECTION_ENABLED
				validity.back_canary_validity = (ValidityFlag) (CANARY_AT_THE_END(stack_impl_ptr) == CANARY_VALUE);
		#endif

	} else {

		#if STACK_DATA_HASH_PROTECTION_ENABLED
				validity.data_validity = NOT_CHECKED;
		#endif
		#if STACK_CANARY_PROTECTION_ENABLED
				validity.back_canary_validity = NOT_CHECKED;
		#endif
	}
#endif

// TODO: struct hash protection is not enabled.
	return validity;
}
ValidityInfo (*VALIDATE_STACK_PTR)(const StackImpl* stack_impl_ptr) = &validate_stack;

#define validate_stack (*VALIDATE_STACK_PTR) // To make the program more reliable.

#if defined(__GNUC__)
#define UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#define UNREACHABLE __assume(0)
#else
#define UNREACHABLE
#endif

static void print_validity(ValidityFlag validity_flag) {
	switch (validity_flag) {

		case VALID: 	  fprintf(stderr, "ok"); break;
		case NOT_VALID:   fprintf(stderr, "BAD!"); break;
		case NOT_CHECKED: fprintf(stderr, "NOT CHECKED"); break;

		default: assert(0); UNREACHABLE;
	}
}

#define EXPANDED_MACRO_VALUE_TO_STRING(MACRO) #MACRO
#define MACRO_VALUE_TO_STRING(MACRO) EXPANDED_MACRO_VALUE_TO_STRING(MACRO)
static void dump_stack(VariableLocation variable_location,
					   ValidityInfo validity,
					   const StackImpl* stack_impl_ptr) {

	fprintf(stderr, "stack<%s>[%p] \"%s\" from %s(%zu), %s (issued for line %zu of \"%s\", function \"%s\"):\n", 
			MACRO_VALUE_TO_STRING(STACK_ITEM_TYPE), (void*) stack_impl_ptr, 
			stack_impl_ptr->definition_location.variable, stack_impl_ptr->definition_location.source_file, 
			stack_impl_ptr->definition_location.line, stack_impl_ptr->definition_location.function, 
			variable_location.line, variable_location.source_file, variable_location.function);

	#if STACK_CANARY_PROTECTION_ENABLED
		fprintf(stderr, "canary = %llx (", stack_impl_ptr->canary);
		print_validity(validity.front_canary_validity);
		fputs(")\n", stderr);
	#endif

	#if STACK_STRUCT_HASH_PROTECTION_ENABLED
		fprintf(stderr, "struct hash = %llx\n", stack_impl_ptr->struct_hash);
	#endif

	#if STACK_DATA_HASH_PROTECTION_ENABLED
		fprintf(stderr, "data hash = %llx\n", stack_impl_ptr->data_hash);
	#endif

	#if STACK_STRUCT_HASH_PROTECTION_ENABLED
		fputs("--- Struct hash validity: ", stderr);
		print_validity(validity.struct_validity);
		fputs(" ---\n", stderr);
	#endif

		fprintf(stderr, "size = %zu (", stack_impl_ptr->size);
		print_validity(validity.size_validity);
		fprintf(stderr, ")\ncapacity = %zu\n", stack_impl_ptr->capacity);
		// Variable location.

	#if STACK_STRUCT_HASH_PROTECTION_ENABLED

		fputs("------\n", stderr);
	#endif

	#if STACK_DATA_HASH_PROTECTION_ENABLED
		fputs("--- Data hash validity: ", stderr);
		print_validity(validity.data_validity);
		fputs(" ---\n", stderr);
	#endif

	fprintf(stderr, "data[%p] {\n", (void*) stack_impl_ptr->data);

	for (size_t i = 0; i < stack_impl_ptr->capacity; ++i) {

		fprintf(stderr, "\t[%zu] = %d", i, stack_impl_ptr->data[i]);

		if (stack_impl_ptr->data[i] == STACK_POISON) {
			fputs(" (POISON)", stderr);
		}

		fputs(",\n", stderr);
	}

	fputs("}\n", stderr);

	#if STACK_DATA_HASH_PROTECTION_ENABLED
		fputs("------\n", stderr);
	#endif

	#if STACK_CANARY_PROTECTION_ENABLED
		fprintf(stderr, "canary = %llx (", CANARY_AT_THE_END(stack_impl_ptr));

		if (validity.back_canary_validity == NOT_CHECKED) {
			validity.back_canary_validity = (ValidityFlag) (CANARY_AT_THE_END(stack_impl_ptr) == CANARY_VALUE);
		}
		print_validity(validity.back_canary_validity);

		fputs(")\n", stderr);
	#endif

	fputs("}\n", stderr);
}

#define VALIDATE_STACK() {                                       \
                                                                 \
	ValidityInfo validity = validate_stack(stack_impl_ptr);      \
                                                                 \
	if (!is_fully_valid(validity)) {                             \
                                                                 \
		dump_stack(variable_location, validity, stack_impl_ptr); \
		fprintf(stderr, "Stack is not valid.\n");                \
                                                                 \
		return false;                                            \
	}                                                            \
}

bool STACK_INIT_FUNCTION_NAME(VariableLocation definition_location, STACK_TYPE_NAME* stack_ptr) {
	assert(stack_ptr != NULL);

	static const size_t INITIAL_CAPACITY = 1; // Can't be zero, because we allocate space for 1 element in the structure itself.
	const size_t INITIAL_SIZE = get_block_size(INITIAL_CAPACITY);

	*stack_ptr = calloc(INITIAL_SIZE, 1);
	if (*stack_ptr == NULL) {
		return false;
	}

	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;
	stack_impl_ptr->size = 0;
	stack_impl_ptr->capacity = INITIAL_CAPACITY;

	// Dropping constnesses and initializing canaries.
	* (CANARY_TYPE*) &stack_impl_ptr->canary = CANARY_VALUE;
	* (CANARY_TYPE*) &CANARY_AT_THE_END(stack_impl_ptr) = CANARY_VALUE;

	stack_impl_ptr->definition_location = definition_location;

	for (size_t i = 0; i < stack_impl_ptr->capacity; ++i) {
		stack_impl_ptr->data[i] = STACK_POISON;
	}
	#if STACK_ANY_HASH_PROTECTION_ENABLED
		update_hashes(stack_impl_ptr);
	#endif

	return true;
}

void STACK_DEINIT_FUNCTION_NAME(VariableLocation variable_location, STACK_TYPE_NAME* stack_ptr) {
	assert(stack_ptr != NULL);

	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;
	VALIDATE_STACK();

	free(*stack_ptr);
	*stack_ptr = NULL;
}

// Две функции.
static bool ensure_space(STACK_TYPE_NAME* stack_ptr) {
	assert(stack_ptr != NULL);

	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;

	if (stack_impl_ptr->size + 1 <= stack_impl_ptr->capacity) {
		return true;
	}

	size_t new_capacity = stack_impl_ptr->capacity * 2;

	void* new_stack_ptr = realloc(*stack_ptr, get_block_size(new_capacity));
	if (new_stack_ptr == NULL) {
		return false;
	}

	*stack_ptr = new_stack_ptr;
	stack_impl_ptr = (StackImpl*) *stack_ptr;

	// Filling new items with poison as we will be referencing them for hashing and dumping.
	// Otherwise we don't know the value stored there. And C standard says it is undefined behaviour to read uninitialized data.
	for (size_t i = stack_impl_ptr->capacity; i < new_capacity; ++i) {
		stack_impl_ptr->data[i] = STACK_POISON;
	}

	stack_impl_ptr->capacity = new_capacity;
	* (CANARY_TYPE*) &CANARY_AT_THE_END(stack_impl_ptr) = CANARY_VALUE;

	return true;
}

bool STACK_PUSH_FUNCTION_NAME(VariableLocation variable_location, 
							  STACK_TYPE_NAME* stack_ptr, STACK_ITEM_ACCEPTANCE_TYPE item) {
	assert(stack_ptr != NULL);

	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;
	VALIDATE_STACK();

	if (!ensure_space(stack_ptr)) {
		return false;
	}
	stack_impl_ptr = (StackImpl*) *stack_ptr;

	if (stack_impl_ptr->data[stack_impl_ptr->size] != STACK_POISON) {

		ValidityInfo stack_validity = validate_stack(stack_impl_ptr);

		dump_stack(variable_location, stack_validity, stack_impl_ptr);
		fprintf(stderr, "Tried to insert to position %zu, but that position is not poisoned.\n", stack_impl_ptr->size);
		
		return false;
	}

#if STACK_ACCEPTS_ITEMS_BY_POINTERS
	stack_impl_ptr->data[stack_impl_ptr->size] = *item;
#else
	stack_impl_ptr->data[stack_impl_ptr->size] = item;
#endif
	stack_impl_ptr->size += 1;
#if STACK_ANY_HASH_PROTECTION_ENABLED
	update_hashes(stack_impl_ptr);
#endif

	return true;
}

static void free_unused_space(STACK_TYPE_NAME* stack_ptr) {
	assert(stack_ptr != NULL);

	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;
	if (stack_impl_ptr->size <= stack_impl_ptr->capacity / 4) {
		size_t new_capacity = stack_impl_ptr->capacity / 4;
		if (new_capacity < 1) {
			// We can't have capacity be less than 1, as one element is allocated in the structure itself.
			new_capacity = 1;
		}
		void* new_stack_ptr = realloc(*stack_ptr, get_block_size(new_capacity));
		if (new_stack_ptr == NULL) {
			// Failed to shrink.
			return;
		}
		*stack_ptr = new_stack_ptr;
		stack_impl_ptr = (StackImpl*) *stack_ptr;

		stack_impl_ptr->capacity = new_capacity;
		* (CANARY_TYPE*) &CANARY_AT_THE_END(stack_impl_ptr) = CANARY_VALUE;
	}
}

bool STACK_POP_FUNCTION_NAME(VariableLocation variable_location, STACK_TYPE_NAME* stack_ptr, STACK_ITEM_TYPE* item_ptr) {
	assert(stack_ptr != NULL);
	assert(item_ptr != NULL);

	StackImpl* stack_impl_ptr = (StackImpl*) *stack_ptr;
	VALIDATE_STACK();
	if (stack_impl_ptr->size == 0) {
		return false;
	}
	*item_ptr = stack_impl_ptr->data[stack_impl_ptr->size - 1];
	stack_impl_ptr->size -= 1;
	stack_impl_ptr->data[stack_impl_ptr->size] = STACK_POISON;
		
	free_unused_space(stack_ptr);
	stack_impl_ptr = (StackImpl*) *stack_ptr;

#if STACK_ANY_HASH_PROTECTION_ENABLED
	update_hashes(stack_impl_ptr);
#endif

	return true;
}
