#define INCLUDED_FROM_VECTOR_IMPLEMENTATION
#include "vector_vm.h"

#include "variable_location.h"

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if VECTOR_CANARY_PROTECTION_ENABLED

#define CANARY_TYPE unsigned long long
#define CANARY_SIZE sizeof(unsigned long long)
static unsigned long long CANARY_VALUE = 0xCD07B10913AE98FALL;

#endif

typedef enum ValidityFlags {

	NOT_VALID   = 0,
	VALID 	    = 1,
	NOT_CHECKED = 2
} ValidityFlag;

typedef struct ValidityInfo {

	#if VECTOR_CANARY_PROTECTION_ENABLED
	ValidityFlag front_canary_validity;
	#endif

	// size <= capacity
	ValidityFlag size_validity;

	#if VECTOR_STRUCT_HASH_PROTECTION_ENABLED
	// hash of struct corresponds to the hash of the struct at the moment
	ValidityFlag struct_validity;
	#endif

	#if VECTOR_CANARY_PROTECTION_ENABLED
	ValidityFlag back_canary_validity;
	#endif

} ValidityInfo;

struct VectorImpl {

	#if VECTOR_CANARY_PROTECTION_ENABLED
		const CANARY_TYPE canary;
	#endif

	#if VECTOR_STRUCT_HASH_PROTECTION_ENABLED
		unsigned long long struct_hash;
	#endif

	size_t size;
	size_t capacity;

	Variable definition;

	VECTOR_ITEM_TYPE data[1];// []

	// data...
	#if VECTOR_CANARY_PROTECTION_ENABLED
	// const CANARY_TYPE canary;
	#endif
};
typedef struct VectorImpl VectorImpl;

static size_t get_block_size(size_t capacity) {
	assert(capacity >= 1); // Zero capacity is forbidden. There is always memory for at least one element.

	#if VECTOR_CANARY_PROTECTION_ENABLED
		return sizeof(VectorImpl) + (capacity - 1) * sizeof(VECTOR_ITEM_TYPE) + sizeof(CANARY_TYPE);
	#else
		return sizeof(VectorImpl) + (capacity - 1) * sizeof(VECTOR_ITEM_TYPE);
	#endif
}

#define CANARY_AT_THE_END(PTR) (* (const CANARY_TYPE*) (PTR->data + PTR->capacity))

// www.cse.yorku.ca/~oz/hash.html
#if VECTOR_STRUCT_HASH_PROTECTION_ENABLED
static unsigned long long get_gnu_hash(const unsigned char* bytes, size_t length) {

	unsigned long long hash = 5381;
	for (size_t i = 0; i < length; ++i) {
		hash = ((hash << 5) + hash) + bytes[i];
	}

	return hash;
}
#endif

#if VECTOR_STRUCT_HASH_PROTECTION_ENABLED
static unsigned long long calculate_struct_hash(const VectorImpl* vector_impl_ptr) {
	return get_gnu_hash((const unsigned char*) &vector_impl_ptr->size, (size_t) ((const char*) vector_impl_ptr->data -
																				(const char*) &vector_impl_ptr->size));
}
#endif

#if VECTOR_ANY_HASH_PROTECTION_ENABLED
static void update_hashes(VectorImpl* vector_impl_ptr) {

	// Now we are using the fact that we are zeroing new items and the struct itself when initializing it as we are zeroing padding bytes as well.
#if VECTOR_STRUCT_HASH_PROTECTION_ENABLED
	vector_impl_ptr->struct_hash = calculate_struct_hash(vector_impl_ptr);
#endif

}
#endif

// про флаги isAllFlagsValid (?)
static bool is_fully_valid(ValidityInfo validity) {

	return
	(
		#if VECTOR_CANARY_PROTECTION_ENABLED
				validity.front_canary_validity == VALID &&
		#endif
				validity.size_validity == VALID &&

		#if VECTOR_STRUCT_HASH_PROTECTION_ENABLED
				validity.struct_validity == VALID &&
		#endif

		#if VECTOR_CANARY_PROTECTION_ENABLED
				validity.back_canary_validity == VALID &&
		#endif

		true
	);
}

static ValidityInfo validate_vector(const VectorImpl* vector_impl_ptr) {
	ValidityInfo validity;

	#if VECTOR_CANARY_PROTECTION_ENABLED
		validity.front_canary_validity = (ValidityFlag) (vector_impl_ptr->canary == CANARY_VALUE);
		validity.back_canary_validity = VALID;
	#endif

	validity.size_validity = (ValidityFlag) (vector_impl_ptr->size <= vector_impl_ptr->capacity);

	#if VECTOR_STRUCT_HASH_PROTECTION_ENABLED
		validity.struct_validity = (ValidityFlag) (vector_impl_ptr->struct_hash == calculate_struct_hash(vector_impl_ptr));
	#endif

	#if VECTOR_STRUCT_HASH_PROTECTION_ENABLED && (VECTOR_CANARY_PROTECTION_ENABLED || VECTOR_DATA_HASH_PROTECTION_ENABLED)

		// We skip checking of data and back canary if struct is not valid as we can segfault while doing so.
		if (validity.struct_validity) {

			#if VECTOR_CANARY_PROTECTION_ENABLED
					validity.back_canary_validity = (ValidityFlag) (CANARY_AT_THE_END(vector_impl_ptr) == CANARY_VALUE);
			#endif

		} else {

			#if VECTOR_CANARY_PROTECTION_ENABLED
					validity.back_canary_validity = NOT_CHECKED;
			#endif
		}
	#endif

	// TODO: struct hash protection is not enabled.
	return validity;
}

static void print_validity(FILE* output_file, ValidityFlag validity_flag) {
	switch (validity_flag) {

		case VALID: 	  fprintf(output_file, "ok"); break;
		case NOT_VALID:   fprintf(output_file, "BAD!"); break;
		case NOT_CHECKED: fprintf(output_file, "NOT CHECKED"); break;

		default: assert(0); UNREACHABLE;
	}
}

#define MACRO_TO_STR2(MACRO) #MACRO
#define MACRO_TO_STR(MACRO) MACRO_TO_STR2(MACRO)

static void dump_vector(Variable variable,
                        ValidityInfo validity,
                        const VectorImpl* vector_impl_ptr,
                        FILE* output_file = stderr) {
	fprintf(
		output_file,
		"stack<%s>[%p] \"%s\" from %s(%zu), %s ",
		
		MACRO_TO_STR(VECTOR_ITEM_TYPE),
		(void*) vector_impl_ptr,
		vector_impl_ptr->definition.name,
		vector_impl_ptr->definition.file,
		vector_impl_ptr->definition.line,
		vector_impl_ptr->definition.function
	);

	fprintf(
		output_file,
		"(issued for line %zu of \"%s\", function \"%s\"):\n",

		variable.line,
		variable.file,
		variable.function
	);

	#if VECTOR_CANARY_PROTECTION_ENABLED
		fprintf(output_file, "canary = %llx (", vector_impl_ptr->canary);
		print_validity(output_file, validity.front_canary_validity);
		fputs(")\n", output_file);
	#endif

	#if VECTOR_STRUCT_HASH_PROTECTION_ENABLED
		fprintf(output_file, "struct hash = %llx\n", vector_impl_ptr->struct_hash);
	#endif

	#if VECTOR_STRUCT_HASH_PROTECTION_ENABLED
		fputs("--- Struct hash validity: ", output_file);
		print_validity(output_file, validity.struct_validity);
		fputs(" ---\n", output_file);
	#endif

	fprintf(output_file, "size = %zu (", vector_impl_ptr->size);
	print_validity(output_file, validity.size_validity);
	fprintf(output_file, ")\ncapacity = %zu\n", vector_impl_ptr->capacity);
	// Variable location.

	#if VECTOR_STRUCT_HASH_PROTECTION_ENABLED
		fputs("------\n", output_file);
	#endif

	fprintf(output_file, "data[%p] {\n", (void*) vector_impl_ptr->data);

	for (size_t i = 0; i < vector_impl_ptr->capacity; ++i) {

		fprintf(output_file, "\t[%zu] = ", i);
		VECTOR_PRINT_ITEM(output_file, vector_impl_ptr->data[i]);

		#if VECTOR_COMPARES_ITEMS_WITH_MEMCMP
			if (memcmp((const void*) &vector_impl_ptr->data[i], (const void*) &VECTOR_POISON, sizeof(VECTOR_ITEM_TYPE)) == 0) {
				fputs(" (POISON)", output_file);
			}
		#else
			if (vector_impl_ptr->data[i] == VECTOR_POISON) {
				fputs(" (POISON)", output_file);
			}
		#endif

		fputs(",\n", output_file);
	}

	fputs("}\n", output_file);

	#if VECTOR_CANARY_PROTECTION_ENABLED
		fprintf(output_file, "canary = %llx (", CANARY_AT_THE_END(vector_impl_ptr));

		if (validity.back_canary_validity == NOT_CHECKED) {
			validity.back_canary_validity = (ValidityFlag) (CANARY_AT_THE_END(vector_impl_ptr) == CANARY_VALUE);
		}
		print_validity(output_file, validity.back_canary_validity);

		fputs(")\n", output_file);
	#endif

	fputs("}\n", output_file);
	// fflush here?
}

#define VALIDATE_VECTOR(RETURN_VALUE_ON_ERROR) {                 \
                                                                 \
	ValidityInfo validity = validate_vector(vector_impl_ptr);    \
                                                                 \
	if (!is_fully_valid(validity)) {                             \
                                                                 \
		dump_vector(variable, validity, vector_impl_ptr);        \
		fprintf(stderr, "Vector is not valid.\n");               \
                                                                 \
		return RETURN_VALUE_ON_ERROR;                            \
	}                                                            \
}

bool VECTOR_INIT_FUNCTION_NAME(Variable definition, VECTOR_TYPE_NAME* vector_ptr) {
	assert(vector_ptr != NULL);

	static const size_t INITIAL_CAPACITY = 1; // Can't be zero, because we allocate space for 1 element in the structure itself.
	const size_t INITIAL_SIZE = get_block_size(INITIAL_CAPACITY);

	*vector_ptr = calloc(1, INITIAL_SIZE);
	if (*vector_ptr == NULL) {
		return false;
	}

	VectorImpl* vector_impl_ptr = (VectorImpl*) *vector_ptr;
	vector_impl_ptr->size = 0;
	vector_impl_ptr->capacity = INITIAL_CAPACITY;

	#if VECTOR_CANARY_PROTECTION_ENABLED
		// Dropping constnesses and initializing canaries.
		* (CANARY_TYPE*) &vector_impl_ptr->canary = CANARY_VALUE;
		* (CANARY_TYPE*) &CANARY_AT_THE_END(vector_impl_ptr) = CANARY_VALUE;
	#endif

	vector_impl_ptr->definition = definition;

	for (size_t i = 0; i < vector_impl_ptr->capacity; ++i) {
		vector_impl_ptr->data[i] = VECTOR_POISON;
	}
	#if VECTOR_ANY_HASH_PROTECTION_ENABLED
		update_hashes(vector_impl_ptr);
	#endif

	return true;
}

void VECTOR_DEINIT_FUNCTION_NAME(Variable variable, VECTOR_TYPE_NAME* vector_ptr) {
	assert(vector_ptr != NULL);

	VectorImpl* vector_impl_ptr = (VectorImpl*) *vector_ptr;
	VALIDATE_VECTOR();

	free(*vector_ptr);
	*vector_ptr = NULL;
}

// Две функции.
static bool ensure_space(VECTOR_TYPE_NAME* vector_ptr) {
	assert(vector_ptr != NULL);

	VectorImpl* vector_impl_ptr = (VectorImpl*) *vector_ptr;

	if (vector_impl_ptr->size + 1 <= vector_impl_ptr->capacity) {
		return true;
	}

	size_t new_capacity = vector_impl_ptr->capacity * 2;

	void* new_vector_ptr = realloc(*vector_ptr, get_block_size(new_capacity));
	if (new_vector_ptr == NULL) {
		return false;
	}

	*vector_ptr = new_vector_ptr;
	vector_impl_ptr = (VectorImpl*) *vector_ptr;

	// Filling new items with poison as we will be referencing them for hashing and dumping.
	// Otherwise we don't know the value stored there. And C standard says it is undefined behaviour to read uninitialized data.
	for (size_t i = vector_impl_ptr->capacity; i < new_capacity; ++i) {
		vector_impl_ptr->data[i] = VECTOR_POISON;
	}

	vector_impl_ptr->capacity = new_capacity;
	#if VECTOR_CANARY_PROTECTION_ENABLED
		* (CANARY_TYPE*) &CANARY_AT_THE_END(vector_impl_ptr) = CANARY_VALUE;
	#endif

	return true;
}

bool VECTOR_PUSH_FUNCTION_NAME(Variable variable, 
                               VECTOR_TYPE_NAME* vector_ptr, VECTOR_ITEM_ACCEPTANCE_TYPE item) {
	assert(vector_ptr != NULL);

	VectorImpl* vector_impl_ptr = (VectorImpl*) *vector_ptr;
	VALIDATE_VECTOR(false);

	if (!ensure_space(vector_ptr)) {
		return false;
	}
	vector_impl_ptr = (VectorImpl*) *vector_ptr;

	if (vector_impl_ptr->data[vector_impl_ptr->size] != VECTOR_POISON) {

		ValidityInfo vector_validity = validate_vector(vector_impl_ptr);

		dump_vector(variable, vector_validity, vector_impl_ptr);
	
		fprintf(stderr, "Tried to insert to position %zu, but that position is not poisoned.\n", vector_impl_ptr->size);
		
		return false;
	}

	#if VECTOR_ACCEPTS_ITEMS_BY_POINTERS
		vector_impl_ptr->data[vector_impl_ptr->size] = *item;
	#else
		vector_impl_ptr->data[vector_impl_ptr->size] = item;
	#endif
	
	vector_impl_ptr->size += 1;

	#if VECTOR_ANY_HASH_PROTECTION_ENABLED
		update_hashes(vector_impl_ptr);
	#endif

	return true;
}

static void free_unused_space(VECTOR_TYPE_NAME* vector_ptr) {
	assert(vector_ptr != NULL);

	VectorImpl* vector_impl_ptr = (VectorImpl*) *vector_ptr;

	if (vector_impl_ptr->size <= vector_impl_ptr->capacity / 4) {

		size_t new_capacity = vector_impl_ptr->capacity / 4;
		if (new_capacity < 1) {
			// We can't have capacity be less than 1, as one element is allocated in the structure itself.
			new_capacity = 1;
		}

		void* new_vector_ptr = realloc(*vector_ptr, get_block_size(new_capacity));
		if (new_vector_ptr == NULL) {
			// Failed to shrink.
			return;
		}

		*vector_ptr = new_vector_ptr;
		vector_impl_ptr = (VectorImpl*) *vector_ptr;

		vector_impl_ptr->capacity = new_capacity;
		#if VECTOR_CANARY_PROTECTION_ENABLED
			* (CANARY_TYPE*) &CANARY_AT_THE_END(vector_impl_ptr) = CANARY_VALUE;
		#endif
	}
}

bool VECTOR_POP_FUNCTION_NAME(Variable variable, VECTOR_TYPE_NAME* vector_ptr, VECTOR_ITEM_TYPE* item_ptr) {
	assert(vector_ptr != NULL);
	assert(item_ptr != NULL);

	VectorImpl* vector_impl_ptr = (VectorImpl*) *vector_ptr;
	VALIDATE_VECTOR(false);

	if (vector_impl_ptr->size == 0) {
		return false;
	}

	*item_ptr = vector_impl_ptr->data[vector_impl_ptr->size - 1];
	vector_impl_ptr->size -= 1;
	vector_impl_ptr->data[vector_impl_ptr->size] = VECTOR_POISON;
		
	free_unused_space(vector_ptr);
	vector_impl_ptr = (VectorImpl*) *vector_ptr;

	#if VECTOR_ANY_HASH_PROTECTION_ENABLED
		update_hashes(vector_impl_ptr);
	#endif

	return true;
}

void VECTOR_DUMP_FUNCTION_NAME(Variable variable, VECTOR_TYPE_NAME* vector_ptr) {
	assert(vector_ptr != NULL);

	VectorImpl* vector_impl_ptr = (VectorImpl*) *vector_ptr;
	ValidityInfo validity = validate_vector(vector_impl_ptr);
	dump_vector(variable, validity, vector_impl_ptr, stdout);
}

VECTOR_ITEM_TYPE* VECTOR_GET_ITEM_FUNCTION_NAME(Variable variable, VECTOR_TYPE_NAME* vector_ptr, size_t index) {
	assert(vector_ptr != NULL);

	VectorImpl* vector_impl_ptr = (VectorImpl*) *vector_ptr;
	VALIDATE_VECTOR(NULL); // Segmentation fault if vector is invalid and the function is used in idiomatic way.
	
	assert(index < vector_impl_ptr->size);
	
	return &vector_impl_ptr->data[index];
}

size_t VECTOR_GET_LENGTH_FUNCTION_NAME(Variable variable, VECTOR_TYPE_NAME* vector_ptr) {
	assert(vector_ptr != NULL);
	
	VectorImpl* vector_impl_ptr = (VectorImpl*) *vector_ptr;
	VALIDATE_VECTOR(0); // 0 if vector is invalid

	return vector_impl_ptr->size;
}

void* VECTOR_GET_DATA_START_FUNCTION_NAME(Variable variable, VECTOR_TYPE_NAME* vector_ptr) {
	assert(vector_ptr != NULL);
	
	VectorImpl* vector_impl_ptr = (VectorImpl*) *vector_ptr;
	VALIDATE_VECTOR(NULL); // NULL is vector is invalid.
	
	return (void*) vector_impl_ptr->data;
}
