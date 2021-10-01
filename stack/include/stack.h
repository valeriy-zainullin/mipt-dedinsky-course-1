#pragma once

#include <stdbool.h>

#define STACK_ITEM_TYPE int
STACK_ITEM_TYPE

#define STACK_TYPE_NAME StackInt
//#define STACK_TYPE_NAME Stack_##STACK_ITEM_TYPE
typedef void* STACK_TYPE_NAME;

#define STACK_FUNCTION_PREFIX stack_##STACK_ITEM_TYPE
/*STACK_FUNCTION_PREFIX
#define STACK_INIT_FUNCTION_NAME STACK_FUNCTION_PREFIX##_init
#define STACK_DEINIT_FUNCTION_NAME STACK_FUNCTION_PREFIX##_deinit
#define STACK_PUSH_FUNCTION_NAME STACK_FUNCTION_PREFIX##_push
#define STACK_POP_FUNCTION_NAME STACK_FUNCTION_PREFIX##_pop
*/
#define STACK_INIT_FUNCTION_NAME stack_##(STACK_ITEM_TYPE)##_init
#define STACK_DEINIT_FUNCTION_NAME stack_##STACK_ITEM_TYPE##_deinit
#define STACK_PUSH_FUNCTION_NAME stack_##STACK_ITEM_TYPE##_push
#define STACK_POP_FUNCTION_NAME stack_##STACK_ITEM_TYPE##_pop
STACK_INIT_FUNCTION_NAME

// #define STACK_ACCEPTS_ITEMS_BY_POINTERS
#if defined(STACK_ACCEPTS_ITEMS_BY_POINTERS)
#define STACK_ITEM_ACCEPTANCE_TYPE STACK_ITEM_TYPE*
#else
#define STACK_ITEM_ACCEPTANCE_TYPE STACK_ITEM_TYPE
#endif

bool STACK_INIT_FUNCTION_NAME(STACK_TYPE_NAME* stack_ptr);
bool STACK_DEINIT_FUNCTION_NAME(STACK_TYPE_NAME* stack_ptr);
bool STACK_PUSH_FUNCTION_NAME(STACK_TYPE_NAME* stack_ptr, STACK_ITEM_ACCEPTANCE_TYPE item);
bool STACK_POP_FUNCTION_NAME(STACK_TYPE_NAME* stack_ptr, STACK_ITEM_TYPE* item_ptr);

#if !defined(INCLUDED_FROM_STACK_IMLEMENTATION)
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
