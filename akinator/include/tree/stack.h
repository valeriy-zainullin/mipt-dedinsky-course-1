#pragma once

#include "tree/visiting_iteration.h"

#include "support/variable_location.h"

#include <stdbool.h>
#include <stddef.h>

// Stack that is easy to change item type of. Just edit such a header. If two or more stacks are needed, just replicate this header and corresponding cpp file.

// --- These lines are designed to be changes according to needs. ---
#define STACK_CANARY_PROTECTION_ENABLED 0
#define STACK_STRUCT_HASH_PROTECTION_ENABLED 0
#define STACK_DATA_HASH_PROTECTION_ENABLED 0
#define STACK_ANY_HASH_PROTECTION_ENABLED STACK_STRUCT_HASH_PROTECTION_ENABLED || STACK_DATA_HASH_PROTECTION_ENABLED

#define STACK_ITEM_TYPE VisitingIteration
#define STACK_TYPE_NAME StackVisitingIteration
#define STACK_ACCEPTS_ITEMS_BY_POINTERS 0
#define STACK_COMPARES_WITH_MEMCMP 1 // Used for structs.

static const int STACK_INT_VERIFICATION_FAILED_EXIT_CODE = 1;
#define STACK_VERIFICATION_FAILED_EXIT_CODE STACK_INT_VERIFICATION_FAILED_EXIT_CODE

MAY_BE_UNUSED static const STACK_ITEM_TYPE STACK_VISITING_ITERATION_POISON = {(TreeNode*) 0xBD796DEDLL, ITERATION_TYPE_LEAVING};
#define STACK_POISON STACK_VISITING_ITERATION_POISON

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

#if STACK_COMPARES_WITH_MEMCMP
#define STACK_ITEMS_EQUAL(LHS, RHS) (memcmp((void*) &LHS, (void*) &RHS, sizeof(STACK_ITEM_TYPE)) == 0)
#else
#define STACK_ITEMS_EQUAL(LHS, RHS) (LHS == RHS)
#endif

// Implementation function names.
#define STACK_INIT_FUNCTION_NAME stack_visiting_iteration_init
#define STACK_DEINIT_FUNCTION_NAME stack_visiting_iteration_deinit
#define STACK_PUSH_FUNCTION_NAME stack_visiting_iteration_push
#define STACK_POP_FUNCTION_NAME stack_visiting_iteration_pop
#define STACK_DUMP_FUNCTION_NAME stack_visiting_iteration_dump
// ------

#define STACK_PRINT_ITEM(STREAM, ITEM) fprintf(STREAM, "VisitingIteration{node = (TreeNode*) %p, iteration = (IterationType) %d}", (void*) ITEM.node, (int) ITEM.type)

// Function definitions.
bool STACK_INIT_FUNCTION_NAME(Variable variable, STACK_TYPE_NAME* stack_ptr);
void STACK_DEINIT_FUNCTION_NAME(Variable variable, STACK_TYPE_NAME* stack_ptr);
bool STACK_PUSH_FUNCTION_NAME(Variable variable, STACK_TYPE_NAME* stack_ptr, STACK_ITEM_ACCEPTANCE_TYPE item);
bool STACK_POP_FUNCTION_NAME(Variable variable, STACK_TYPE_NAME* stack_ptr, STACK_ITEM_TYPE* item_ptr);
void STACK_DUMP_FUNCTION_NAME(Variable variable, STACK_TYPE_NAME* stack_ptr);

#if !defined(INCLUDED_FROM_STACK_IMPLEMENTATION)
// Interfacing macros.
// Don't define them for stack implementation as stack implementation function names from definitions (for example, "bool STACK_INIT_FUNCTION_NAME(VariableLocation variable_location, STACK_TYPE_NAME* stack_ptr);") will be macro-expanded to real function names (in example, "bool stack_int_init(VariableLocation variable_location, void** stack_ptr);") and then they will be expanded to these function-like macros ("stack_int_init" in example), but then number of arguments will mismatch. And still it would be a wrong behaviour.
#define stack_visiting_iteration_init(STACK_PTR) stack_visiting_iteration_init(MAKE_VARIABLE_LOCATION(STACK_PTR), STACK_PTR)
#define stack_visiting_iteration_deinit(STACK_PTR) stack_visiting_iteration_deinit(MAKE_VARIABLE_LOCATION(STACK_PTR), STACK_PTR)
#define stack_visiting_iteration_push(STACK_PTR, ITEM) stack_visiting_iteration_push(MAKE_VARIABLE_LOCATION(STACK_PTR), STACK_PTR, ITEM)
#define stack_visiting_iteration_pop(STACK_PTR, ITEM_PTR) stack_visiting_iteration_pop(MAKE_VARIABLE_LOCATION(STACK_PTR), STACK_PTR, ITEM_PTR)
#define stack_visiting_iteration_dump(STACK_PTR) stack_visiting_iteration_dump(MAKE_VARIABLE_LOCATION(STACK_PTR), STACK_PTR)
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
#undef STACK_DUMP_FUNCTION_NAME
#if defined(STACK_ACCEPTS_ITEMS_BY_POINTERS)
#undef STACK_ACCEPTS_ITEMS_BY_POINTERS
#endif
#undef STACK_ITEM_ACCEPTANCE_TYPE
#endif
