#pragma once

#include "location.h"

#include <stdbool.h>

#define STACK_ITEM_TYPE int
#define STACK_TYPE_NAME StackInt
#define STACK_INIT_FUNCTION_NAME stack_int_init
#define STACK_DEINIT_FUNCTION_NAME stack_int_deinit
#define STACK_PUSH_FUNCTION_NAME stack_int_push
#define STACK_POP_FUNCTION_NAME stack_int_pop
#define stack_int_init(STACK_PTR) stack_int_init(LOCATION, STACK_PTR)
#define stack_int_deinit(STACK_PTR) stack_int_deinit(LOCATION, STACK_PTR);
#define stack_int_push(STACK_PTR, ITEM) stack_int_push(LOCATION, STACK_PTR, ITEM)
#define stack_int_pop(STACK_PTR) stack_int_pop(LOCATION, STACK_PTR)
#define STACK_ACCEPTS_ITEMS_BY_POINTERS 0
#define 

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

bool STACK_INIT_FUNCTION_NAME(SourceLocation source_location, STACK_TYPE_NAME* stack_ptr);
void STACK_DEINIT_FUNCTION_NAME(SourceLocation source_location STACK_TYPE_NAME* stack_ptr);
bool STACK_PUSH_FUNCTION_NAME(SourceLocation source_location, STACK_TYPE_NAME* stack_ptr, STACK_ITEM_ACCEPTANCE_TYPE item);
bool STACK_POP_FUNCTION_NAME(SourceLocation source_location, STACK_TYPE_NAME* stack_ptr, STACK_ITEM_TYPE* item_ptr);

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
