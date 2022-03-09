#include "list.h"

#include "../../vector/external/vector_list_int_node.h"

struct ListImpl {
	VectorListIntNode nodes;
	
	ListAddress first;
}

bool LIST_INIT_FUNCTION_NAME(LIST_TYPE* list) {

	*list = calloc(1, sizeof(ListImpl));

	if (*list == NULL) {
		return false;
	}
	
	ListImpl* list_impl = (ListImpl*) *list;
	
	if (!LIST_VECTOR_INIT_FUNCTION_NAME(&list_impl->nodes)) {
		free(list_impl);
		return false;
	}
	list_impl->first = 0;
}

void LIST_DEINIT_FUNCTION_NAME(LIST_TYPE* list) {
	assert(list != NULL);

	ListImpl* list_impl = (ListImpl*) *list;

	LIST_VECTOR_DEINIT_FUNCTION_NAME(&list_impl->nodes);
	free(list_impl);
}

void LIST_INSERT_FUNCTION_NAME(LIST_TYPE* list, LIST_ITEM_TYPE* prev_item, LIST_ITEM_TYPE_ON_INSERTION item) {
	assert(list != NULL);
	assert(prev_item != NULL);

	ListImpl* list_impl = (ListImpl*) *list;
	
	ListIntNode node = {};
	node.prev = (size_t) (prev_item - LIST_VECTOR_GET_DATA_START(&list_impl->nodes));
	node.next = prev_item->next;
	#if defined(LIST_INSERTS_ITEMS_BY_POINTERS)
		node.item = *item;
	#else
		node.item = item;
	#endif
	
	if (!LIST_VECTOR_PUSH_FUNCTION_NAME(&list_impl->nodes, p
}

void LIST_DELETE_FUNCTION_NAME(LIST_TYPE* list, LIST_NODE_TYPE* prev);
bool LIST_GET_NODE_FUNCTION_NAME(LIST_TYPE* list, LIST_NODE_TYPE* node);
void LIST_DEGRAGMENTATE_FUNCTION_NAME(LIST_TYPE* list);

