#pragma once

#include <stdbool.h>
#include <stddef.h>

// TODO: move this file into a new "data-structures" subfolder, if there will be more data structures.

struct vector {
	void* memory;
	size_t size;
	size_t capacity;
	size_t item_size;
};

struct vector* vector_new(size_t initial_size, size_t item_size);
struct vector* vector_delete(struct vector* vector_ptr);

bool vector_push(struct vector* vector_ptr, const void* item_ptr);
bool vector_pop(struct vector* vector_ptr);

void vector_set(struct vector* vector_ptr, size_t index, void const* item_ptr);
void vector_get(struct vector const* vector_ptr, size_t index, void* item_ptr);

bool vector_check_empty(struct vector const* vector_ptr);
size_t vector_get_size(struct vector const* vector_ptr);

void* vector_get_item_ptr(struct vector* vector_ptr, size_t index);
void const* vector_get_const_item_ptr(struct vector const* vector_ptr, size_t index);

bool vector_reserve(struct vector* vector_ptr, size_t new_capacity);
void* vector_get_storage(struct vector* vector_ptr);

size_t vector_get_item_size(struct vector* vector_ptr);

bool vector_resize(struct vector* vector_ptr, size_t new_size);

void vector_print(struct vector const* vector_ptr, void (*print_function)(void const* item_ptr));
