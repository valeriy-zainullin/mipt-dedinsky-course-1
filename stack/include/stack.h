#pragma once

#include "variable_location.h"

#include <stdbool.h>
#include <stddef.h>

// Stack that is easy to change item type of. Just edit such a header. If two or more stacks are needed, just replicate this header and corresponding cpp file.

// --- These lines are designed to be changes according to needs. ---
#define STACK_CANARY_PROTECTION_ENABLED 1
#define STACK_STRUCT_HASH_PROTECTION_ENABLED 1
#define STACK_DATA_HASH_PROTECTION_ENABLED 1
#define STACK_ANY_HASH_PROTECTION_ENABLED STACK_STRUCT_HASH_PROTECTION_ENABLED || STACK_DATA_HASH_PROTECTION_ENABLED

#define STACK_ITEM_TYPE int
#define STACK_TYPE_NAME StackInt
#define STACK_ACCEPTS_ITEMS_BY_POINTERS 0

static const int STACK_INT_VERIFICATION_FAILED_EXIT_CODE = 1;
#define STACK_VERIFICATION_FAILED_EXIT_CODE STACK_INT_VERIFICATION_FAILED_EXIT_CODE
static const STACK_ITEM_TYPE STACK_INT_POISON = -120304010;
#define STACK_POISON STACK_INT_POISON

typedef void* STACK_TYPE_NAME; // Implicit cast to void* is allowed in this case.
/*
struct STACK_TYPE_NAME##_STRUCT;
typedef STACK_TYPE_NAME##_STRUCT* STACK_TYPE_NAME;
*/

#if STACK_ACCEPTS_ITEMS_BY_POINTERS
#define STACK_ITEM_ACCEPTANCE_TYPE STACK_ITEM_TYPE*
#else
#define STACK_ITEM_ACCEPTANCE_TYPE STACK_ITEM_TYPE
#endif

// Implementation function names.
#define STACK_INIT_FUNCTION_NAME stack_int_init
#define STACK_DEINIT_FUNCTION_NAME stack_int_deinit
#define STACK_PUSH_FUNCTION_NAME stack_int_push
#define STACK_POP_FUNCTION_NAME stack_int_pop
// ------

// Function definitions.
bool STACK_INIT_FUNCTION_NAME(VariableLocation variable_location, STACK_TYPE_NAME* stack_ptr);
void STACK_DEINIT_FUNCTION_NAME(VariableLocation variable_location, STACK_TYPE_NAME* stack_ptr);
bool STACK_PUSH_FUNCTION_NAME(VariableLocation variable_location, STACK_TYPE_NAME* stack_ptr, STACK_ITEM_ACCEPTANCE_TYPE item);
bool STACK_POP_FUNCTION_NAME(VariableLocation variable_location, STACK_TYPE_NAME* stack_ptr, STACK_ITEM_TYPE* item_ptr);

#if !defined(INCLUDED_FROM_STACK_IMPLEMENTATION)
// Interfacing macros.
// Don't define them for stack implementation as stack implementation function names from definitions (for example, "bool STACK_INIT_FUNCTION_NAME(VariableLocation variable_location, STACK_TYPE_NAME* stack_ptr);") will be macro-expanded to real function names (in example, "bool stack_int_init(VariableLocation variable_location, void** stack_ptr);") and then they will be expanded to these function-like macros ("stack_int_init" in example), but then number of arguments will mismatch. And still it would be a wrong behaviour.
#define stack_int_init(STACK_PTR) stack_int_init(MAKE_VARIABLE_LOCATION(STACK_PTR), STACK_PTR)
#define stack_int_deinit(STACK_PTR) stack_int_deinit(MAKE_VARIABLE_LOCATION(STACK_PTR), STACK_PTR)
#define stack_int_push(STACK_PTR, ITEM) stack_int_push(MAKE_VARIABLE_LOCATION(STACK_PTR), STACK_PTR, ITEM)
#define stack_int_pop(STACK_PTR, ITEM_PTR) stack_int_pop(MAKE_VARIABLE_LOCATION(STACK_PTR), STACK_PTR, ITEM_PTR)
#endif

#if !defined(INCLUDED_FROM_STACK_IMPLEMENTATION)
#undef STACK_ITEM_TYPE
#undef STACK_ITEM_TYPE_SIZE
#undef STACK_TYPE_NAME
#undef STACK_FUNCTION_PREFIX
#undef STACK_INIT_FUNCTION_NAME
#undef STACK_DEINIT_FUNCTION_NAME
#undef STACK_PUSH_FUNCTION_NAME
#undef STACK_POP_FUNCTION_NAME
#if defined(STACK_ACCEPTS_ITEMS_BY_POINTERS)
#undef STACK_ACCEPTS_ITEMS_BY_POINTERS
#endif
#undef STACK_ITEM_ACCEPTANCE_TYPE
#endif
