#pragma once

#include <string.h>
#include <stdlib.h>

static bool string_concat(char** dst, const char* lhs, const char* rhs) {
	*dst = (char*) calloc(strlen(lhs) + strlen(rhs) + 1, sizeof(char));
	
	if (*dst == NULL) {
		return false;
	}
	
	strcpy(*dst, lhs);
	strcat(*dst, rhs);
	
	return true;
}

static void string_free(char** dst) {
	free(*dst);
	*dst = NULL;
}

