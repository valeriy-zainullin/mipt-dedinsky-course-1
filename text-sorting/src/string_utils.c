#include "string_utils.h"

#include <string.h>
#include <stddef.h>
#include <stdlib.h>

char* string_cat(const char* left_hand_side, const char* right_hand_side) {
	char* buffer = malloc((strlen(left_hand_side) + strlen(right_hand_side)) * sizeof(char));
	if (buffer == NULL) {
		return NULL;
	}
	strcpy(buffer, left_hand_side);
	strcat(buffer, right_hand_side);
	return buffer;
}

void string_free(char* buffer) {
	free((void*) buffer);
}
