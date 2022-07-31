#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: move this file into a new "data-structures" subfolder, if there will be more data structures.

struct vector {
	void* memory;
	size_t size;
	size_t capacity;
	size_t item_size;
};

bool vector_reserve(struct vector* vector_ptr, size_t new_capacity) {
	void* new_memory = realloc(vector_ptr->memory, vector_ptr->item_size * new_capacity);
	
	if (new_memory == NULL) {
		return false;
	}
	
	vector_ptr->memory = new_memory;
	vector_ptr->capacity = new_capacity;
	return true;
}

static size_t get_capacity_for_size(size_t size) {
	if (size == 0) {
		return 1;
	} else if ((size & (size - 1)) == 0) {
		// Size is a power of 2.
		return size;
	}
	
	size_t capacity = size * 2;
	
	while ((capacity & (capacity - 1)) != 0) {
		capacity = (capacity & (capacity - 1));
	}
	
	return capacity;
}

struct vector* vector_new(size_t initial_size, size_t item_size) {
	struct vector* vector_ptr = calloc(1, sizeof(struct vector));
	if (vector_ptr == NULL) {
		return NULL;
	}
	vector_ptr->item_size = item_size;
	vector_ptr->capacity = get_capacity_for_size(initial_size);
	vector_ptr->size = initial_size;

	vector_ptr->memory = calloc(vector_ptr->capacity, vector_ptr->item_size);
	if (vector_ptr->memory == NULL) {
		free(vector_ptr);
		return NULL;
	}
	
	return vector_ptr;
}

struct vector* vector_delete(struct vector* vector_ptr) {
	if (vector_ptr == NULL) {
		// Why not protect against double free?
		return NULL;
	}
	free(vector_ptr->memory);
	free(vector_ptr);
	return NULL;
}

static bool vector_check_appendable(struct vector const* vector_ptr) {
	assert(vector_ptr->size <= vector_ptr->capacity);
	return vector_ptr->size < vector_ptr->capacity;
}

static bool vector_expand(struct vector* vector_ptr) {
	return vector_reserve(vector_ptr, vector_ptr->capacity * 2);
}

static bool vector_check_shrinkable(struct vector const* vector_ptr) {
	// Reallocating for 0 capacity may return a NULL pointer meaning that reallocation failed.
	// Or may succeed as well.
	// return vector_ptr->capacity / 2 >= 1 && vector_ptr->size * 4 <= vector_ptr->capacity;
	assert(vector_ptr->capacity != 0);
	return vector_ptr->capacity >= 2 && vector_ptr->size * 4 <= vector_ptr->capacity;
}

static bool vector_shrink(struct vector* vector_ptr) {
	return vector_reserve(vector_ptr, vector_ptr->capacity / 2);
}

void* vector_get_storage(struct vector* vector_ptr) {
	return vector_ptr->memory;
}

size_t vector_get_item_size(struct vector* vector_ptr) {
	return vector_ptr->item_size;
}

void* vector_get_item_ptr(struct vector* vector_ptr, size_t index) {
	assert(index < vector_ptr->size);
	return (char*) vector_ptr->memory + vector_ptr->item_size * index;
}

void const* vector_get_const_item_ptr(struct vector const* vector_ptr, size_t index) {
	return (const char*) vector_ptr->memory + vector_ptr->item_size * index;
}

void vector_set(struct vector* vector_ptr, size_t index, void const* item_ptr) {
	assert(index < vector_ptr->size);
	memcpy(vector_get_item_ptr(vector_ptr, index), item_ptr, vector_ptr->item_size);
}

void vector_get(struct vector const* vector_ptr, size_t index, void* item_ptr) {
	memcpy(item_ptr, vector_get_const_item_ptr(vector_ptr, index), vector_ptr->item_size);	
}

bool vector_check_empty(struct vector const* vector_ptr) {
	return vector_ptr->size == 0;
}

size_t vector_get_size(struct vector const* vector_ptr) {
	return vector_ptr->size;
}

bool vector_pop(struct vector* vector_ptr) {
	if (vector_check_empty(vector_ptr)) {
		return false;
	}
	
	vector_ptr->size -= 1;
	
	if (vector_check_shrinkable(vector_ptr) && !vector_shrink(vector_ptr)) {
		vector_ptr->size += 1;
		return false;
	}
	
	return true;
}

bool vector_push(struct vector* vector_ptr, const void* item_ptr) {
	if (!vector_check_appendable(vector_ptr) && !vector_expand(vector_ptr)) {
		return false;
	}
	assert(vector_check_appendable(vector_ptr));
	
	vector_ptr->size += 1;
	vector_set(vector_ptr, vector_ptr->size - 1, item_ptr);
	
	return true;
}

bool vector_resize(struct vector* vector_ptr, size_t new_size) {
	vector_ptr->size = new_size;

	size_t new_capacity = get_capacity_for_size(new_size);
	if (new_capacity > vector_ptr->capacity) {
		if (!vector_reserve(vector_ptr, new_capacity)) {
			return false;
		}
	}
	
	return true;
}

void vector_print(struct vector const* vector_ptr, void (*print_function)(void const* item_ptr)) {
	printf("[");
	bool is_first = true;
	for (size_t i = 0; i < vector_ptr->size; ++i) {
		if (!is_first) {
			printf(", ");
		}
		print_function(vector_get_const_item_ptr(vector_ptr, i));
		is_first = false;
	}
	printf("]\n");
}
