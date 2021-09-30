#include "my_qsort.h"

#include <assert.h>
#include <stdio.h>

#define SWAP_OBJECTS(TYPE, LEFT_HAND_SIDE, RIGHT_HAND_SIDE) { \
	TYPE tmp = * (TYPE*) (LEFT_HAND_SIDE);                      \
	*(LEFT_HAND_SIDE) = * (TYPE*) (RIGHT_HAND_SIDE);            \
	*(RIGHT_HAND_SIDE) = tmp;                                   \
}

// size_t is often the fastest built-in type computer can operate with.
static void my_qsort_swap_bytes(char* left_hand_side, char* right_hand_side, size_t number_of_bytes) {
	for (size_t i = 0; i < number_of_bytes / sizeof(size_t); ++i) {
		SWAP_OBJECTS(size_t, left_hand_side + i * sizeof(size_t), right_hand_side + i * sizeof(size_t));
	}
	for (size_t i = 0; i < number_of_bytes % sizeof(size_t); ++i) {
		SWAP_OBJECTS(char, left_hand_side + number_of_bytes - 1 - i, right_hand_side + number_of_bytes - 1 - i);
	}
}

static size_t my_qsort_partition(char* items, size_t length, size_t item_size, int(*compare)(const void*, const void*), size_t pivot_item_index) {
	assert(length >= 1);

	size_t left = 0; // <=
	size_t right = length - 1; // >=
	while (left < right) {
		while (left < right && compare(items + left * item_size, items + pivot_item_index * item_size) <= 0) {
			left += 1;
			fprintf(stderr, "left = %zu.\n", left);
		}
		while (left < right && compare(items + right * item_size, items + pivot_item_index * item_size) >= 0) {
			right -= 1;
			fprintf(stderr, "right = %zu.\n", right);
		}
		if (left < right) {
			fprintf(stderr, "Swapping %zu and %zu.\n", left, right);
			my_qsort_swap_bytes(items + left * item_size, items + right * item_size, item_size);
			left += 1;
			right -= 1;
		}
	}

	return left;
}

// To not cast items to char* every time we need to do pointer arithmetics with it.
static void my_qsort_impl(char* items, size_t length, size_t item_size, int(*compare)(const void*, const void*)) {
	printf("items = %p, length = %zu, item_size = %zu.\n", items, length, item_size);
	fflush(stdout);
	if (length <= 1) {
		return;
	} else if (length == 2) {
		if (compare(items, items + item_size) > 0) {
			my_qsort_swap_bytes(items, items + item_size, item_size);
		}
		return;
	}
	size_t pivot_item_index = length / 2;
	pivot_item_index = my_qsort_partition(items, length, item_size, compare, pivot_item_index);
	printf("pivot_item_index = %zu.\n", pivot_item_index);
	fflush(stdout);
	my_qsort_impl(items, pivot_item_index, item_size, compare);
	my_qsort_impl(items + (pivot_item_index + 1) * item_size, length - pivot_item_index - 1, item_size, compare);
}

void my_qsort(void* items, size_t length, size_t item_size, int(*compare)(const void*, const void*)) {
	my_qsort_impl((char*) items, length, item_size, compare);
}
