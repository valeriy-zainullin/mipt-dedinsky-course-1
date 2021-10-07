#pragma once

#include "variable_location.h"

#include <stdbool.h>
#include <stddef.h>

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

// Interfacing macros.
#define stack_int_init(STACK_PTR) stack_int_do_init(MAKE_VARIABLE_LOCATION(STACK_PTR), STACK_PTR)
#define stack_int_deinit(STACK_PTR) stack_int_do_deinit(MAKE_VARIABLE_LOCATION(STACK_PTR), STACK_PTR);
#define stack_int_push(STACK_PTR, ITEM) stack_int_do_push(MAKE_VARIABLE_LOCATION(STACK_PTR), STACK_PTR, ITEM)
#define stack_int_pop(STACK_PTR, ITEM_PTR) stack_int_do_pop(MAKE_VARIABLE_LOCATION(STACK_PTR), STACK_PTR, ITEM_PTR)

// Implementation function names.
#define STACK_INIT_FUNCTION_NAME stack_int_do_init
#define STACK_DEINIT_FUNCTION_NAME stack_int_do_deinit
#define STACK_PUSH_FUNCTION_NAME stack_int_do_push
#define STACK_POP_FUNCTION_NAME stack_int_do_pop
// ------

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

bool STACK_INIT_FUNCTION_NAME(VariableLocation variable_location, STACK_TYPE_NAME* stack_ptr);
void STACK_DEINIT_FUNCTION_NAME(VariableLocation variable_location, STACK_TYPE_NAME* stack_ptr);
bool STACK_PUSH_FUNCTION_NAME(VariableLocation variable_location, STACK_TYPE_NAME* stack_ptr, STACK_ITEM_ACCEPTANCE_TYPE item);
bool STACK_POP_FUNCTION_NAME(VariableLocation variable_location, STACK_TYPE_NAME* stack_ptr, STACK_ITEM_TYPE* item_ptr);

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
