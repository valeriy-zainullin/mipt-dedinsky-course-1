#pragma once

#include <stddef.h>

struct ListIntNode {
	size_t prev;
	size_t next;

	int item;
};

#define LIST_ITEM_TYPE int
#define LIST_NODE_TYPE ListIntNode
#define LIST_TYPE ListInt

#define LIST_VECTOR_INIT_FUNCTION_NAME vector_list_int_node_init
#define LIST_VECTOR_DEINIT_FUNCTION_NAME vector_list_int_node_deinit
#define LIST_VECTOR_PUSH_FUNCTION_NAME vector_list_int_node_push
#define LIST_VECTOR_POP_FUNCTION_NAME vector_list_int_node_pop
#define LIST_VECTOR_DUMP_FUNCTION_NAME vector_list_int_node_dump
#define LIST_VECTOR_GET_ITEM_FUNCTION_NAME vector_list_int_node_get_item
#define LIST_VECTOR_GET_DATA_START_FUNCTION_NAME vector_list_int_node_get_data_start

#define LIST_INSERTS_ITEMS_BY_POINTERS 0
#define LIST_INSERT_FUNCTION_NAME list_int_insert
#define LIST_DELETE_FUNCTION_NAME list_int_delete
#define LIST_GET_NODE_FUNCTION_NAME list_int_get_node
#define LIST_DEFRAGMENTATE_FUNCTION_NAME list_int_defragmentate

typedef void* LIST_TYPE;

bool LIST_INIT_FUNCTION_NAME(LIST_TYPE* list);
void LIST_DEINIT_FUNCTION_NAME(LIST_TYPE* list);

#if LIST_INSERTS_ITEMS_BY_POINTERS
#define LIST_ITEM_TYPE_ON_INSERTION LIST_ITEM_TYPE*
#else
#define LIST_ITEM_TYPE_ON_INSERTION LIST_ITEM_TYPE
#endif

void LIST_INSERT_FUNCTION_NAME(LIST_TYPE* list, LIST_ITEM_TYPE* prev_item, LIST_ITEM_TYPE_ON_INSERTION item);
void LIST_DELETE_FUNCTION_NAME(LIST_TYPE* list, LIST_NODE_TYPE* prev);
bool LIST_GET_NODE_FUNCTION_NAME(LIST_TYPE* list, LIST_NODE_TYPE* node);
void LIST_DEGRAGMENTATE_FUNCTION_NAME(LIST_TYPE* list);

#if !defined(INCLUDED_FROM_LIST_IMPL)
#undef LIST_ITEM_TYPE
#undef LIST_NODE_TYPE
#undef LIST_TYPE
#undef LIST_INSERTS_ITEMS_BY_POINTERS
#undef LIST_INSERT_FUNCTION_NAME
#undef LIST_DELETE_FUNCTION_NAME
#undef LIST_GET_NODE_FUNCTION_NAME
#undef LIST_DEFRAGMENTATE_FUNCTION_NAME
#endif

