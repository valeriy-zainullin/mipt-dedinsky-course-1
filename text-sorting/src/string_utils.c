#include "string_utils.h"

#include <string.h>
#include <stddef.h>
#include <stdlib.h>

char* string_cat(const char* left_hand_side, const char* right_hand_side) {
	size_t left_hand_side_length = strlen(left_hand_side);
	size_t right_hand_side_length = strlen(right_hand_side);
	char* buffer = malloc((left_hand_side_length + right_hand_side_length + 1) * sizeof(char));
	if (buffer == NULL) {
		return NULL;
	}
	memcpy(buffer, left_hand_side, left_hand_side_length);
	memcpy(buffer + left_hand_side_length, right_hand_side, right_hand_side_length + 1);
	return buffer;
}

void string_free(char* buffer) {
	free((void*) buffer);
}
