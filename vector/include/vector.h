#pragma once

#include "variable_location.h"

#include <stdbool.h>
#include <stddef.h>

// Vector that is easy to change item type of. Just edit such a header. If two or more stacks are needed, just replicate this header and corresponding cpp file.

// --- These lines are designed to be changes according to needs. ---
#define VECTOR_CANARY_PROTECTION_ENABLED 0
#define VECTOR_STRUCT_HASH_PROTECTION_ENABLED 0
#define VECTOR_ANY_HASH_PROTECTION_ENABLED VECTOR_STRUCT_HASH_PROTECTION_ENABLED

#define VECTOR_ITEM_TYPE const char*
#define VECTOR_TYPE_NAME VectorString
#define VECTOR_ACCEPTS_ITEMS_BY_POINTERS 0
#define VECTOR_COMPARES_ITEMS_WITH_MEMCMP 0

static const int VECTOR_INT_VERIFICATION_FAILED_EXIT_CODE = 1;
#define VECTOR_VERIFICATION_FAILED_EXIT_CODE VECTOR_INT_VERIFICATION_FAILED_EXIT_CODE
MAY_BE_UNUSED static VECTOR_ITEM_TYPE const VECTOR_STRING_POISON = (VECTOR_ITEM_TYPE) 0xBD796DEDLL;
#define VECTOR_POISON VECTOR_STRING_POISON

typedef void* VECTOR_TYPE_NAME; // Implicit cast to void* is allowed in this case.
/*
struct STACK_TYPE_NAME##_STRUCT;
typedef STACK_TYPE_NAME##_STRUCT* STACK_TYPE_NAME;
*/

#if VECTOR_ACCEPTS_ITEMS_BY_POINTERS
#define VECTOR_ITEM_ACCEPTANCE_TYPE VECTOR_ITEM_TYPE*
#else
#define VECTOR_ITEM_ACCEPTANCE_TYPE VECTOR_ITEM_TYPE
#endif

// Implementation function names.
#define VECTOR_INIT_FUNCTION_NAME vector_string_init
#define VECTOR_DEINIT_FUNCTION_NAME vector_string_deinit
#define VECTOR_PUSH_FUNCTION_NAME vector_string_push
#define VECTOR_POP_FUNCTION_NAME vector_string_pop
#define VECTOR_DUMP_FUNCTION_NAME vector_string_dump
#define VECTOR_GET_ITEM_FUNCTION_NAME vector_string_get_item
#define VECTOR_GET_DATA_START_FUNCTION_NAME vector_string_get_data_start
// ------

#define VECTOR_PRINT_ITEM(STREAM, ITEM) fprintf(STREAM, "%p", (void*) ITEM)

// Function definitions.
bool VECTOR_INIT_FUNCTION_NAME(Variable variable, VECTOR_TYPE_NAME* vector_ptr);
void VECTOR_DEINIT_FUNCTION_NAME(Variable variable, VECTOR_TYPE_NAME* vector_ptr);
bool VECTOR_PUSH_FUNCTION_NAME(Variable variable, VECTOR_TYPE_NAME* vector_ptr, VECTOR_ITEM_ACCEPTANCE_TYPE item);
bool VECTOR_POP_FUNCTION_NAME(Variable variable, VECTOR_TYPE_NAME* vector_ptr, VECTOR_ITEM_TYPE* item_ptr);
void VECTOR_DUMP_FUNCTION_NAME(Variable variable, VECTOR_TYPE_NAME* vector_ptr);

VECTOR_ITEM_TYPE* VECTOR_GET_ITEM_FUNCTION_NAME(Variable variable, VECTOR_TYPE_NAME* vector_ptr, size_t index);
size_t VECTOR_GET_LENGTH_FUNCTION_NAME(Variable variable, VECTOR_TYPE_NAME* vector_ptr);
void* VECTOR_GET_DATA_START_FUNCTION_NAME(Variable variable, VECTOR_TYPE_NAME* vector_ptr);

#if !defined(INCLUDED_FROM_VECTOR_IMPLEMENTATION)
// Interfacing macros.
// Don't define them for stack implementation as stack implementation function names from definitions (for example, "bool STACK_INIT_FUNCTION_NAME(VariableLocation variable_location, STACK_TYPE_NAME* stack_ptr);") will be macro-expanded to real function names (in example, "bool stack_int_init(VariableLocation variable_location, void** stack_ptr);") and then they will be expanded to these function-like macros ("stack_int_init" in example), but then number of arguments will mismatch. And still it would be a wrong behaviour.
#define vector_string_init(...) vector_string_init(MAKE_VARIABLE_LOCATION(VECTOR_PTR), __VA_ARGS__)
#define vector_string_deinit(...) vector_string_deinit(MAKE_VARIABLE_LOCATION(VECTOR_PTR), __VA_ARGS__)
#define vector_string_push(...) vector_string_push(MAKE_VARIABLE_LOCATION(VECTOR_PTR), __VA_ARGS__)
#define vector_string_pop(...) vector_string_pop(MAKE_VARIABLE_LOCATION(VECTOR_PTR), __VA_ARGS__)
#define vector_string_dump(...) vector_string_dump(MAKE_VARIABLE_LOCATION(VECTOR_PTR), __VA_ARGS__)
#define vector_string_get_item(...) vector_string_get_item(MAKE_VARIABLE_LOCATION(VECTOR_PTR), __VA_ARGS__)
#define vector_string_get_length(...) vector_string_get_length(MAKE_VARIABLE_LOCATION(VECTOR_PTR), __VA_ARGS__)
#define vector_string_get_data_start(...) vector_string_get_data_start(MAKE_VARIABLE_LOCATION(VECTOR_PTR), __VA_ARGS__)
#endif

#if !defined(INCLUDED_FROM_VECTOR_IMPLEMENTATION)
#undef VECTOR_ITEM_TYPE
#undef VECTOR_TYPE_NAME
#undef VECTOR_INIT_FUNCTION_NAME
#undef VECTOR_DEINIT_FUNCTION_NAME
#undef VECTOR_PUSH_FUNCTION_NAME
#undef VECTOR_POP_FUNCTION_NAME
#undef VECTOR_DUMP_FUNCTION_NAME
#if defined(VECTOR_ACCEPTS_ITEMS_BY_POINTERS)
#undef VECTOR_ACCEPTS_ITEMS_BY_POINTERS
#endif
#undef VECTOR_ITEM_ACCEPTANCE_TYPE
#endif
