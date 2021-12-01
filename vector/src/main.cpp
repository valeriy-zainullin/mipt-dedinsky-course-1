#include "vector.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

int main() {
	VectorString strings = NULL;
	if (!vector_string_init(&strings)) {
		printf("Failed to initialize vector.\n");
		return 1;
	}

	if (
		!vector_string_push(&strings, "123") ||
		!vector_string_push(&strings, "234") ||
		!vector_string_push(&strings, "345")
	) {
		printf("Failed to insert an element.\n");
		return 2;
	}

	if (
		vector_string_get_item(&strings, 0) == NULL ||
		strcmp(*vector_string_get_item(&strings, 0), "123") != 0
	) {
		printf("Wrong item at position 0.\n");
		vector_string_deinit(&strings);
		return 3;
	}

	if (
		vector_string_get_item(&strings, 1) == NULL ||
		strcmp(*vector_string_get_item(&strings, 1), "234") != 0
	) {
		printf("Wrong item at position 1.\n");
		vector_string_deinit(&strings);
		return 3;
	}

	if (
		vector_string_get_item(&strings, 2) == NULL ||
		strcmp(*vector_string_get_item(&strings, 2), "345") != 0
	) {
		printf("Wrong item at position 2.\n");
		vector_string_deinit(&strings);
		return 3;
	}


	vector_string_deinit(&strings);

	return 0;
}
