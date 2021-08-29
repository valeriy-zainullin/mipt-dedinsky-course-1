#include "string-functions.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

int my_puts(const char* s) {
	while (*s != '\0') {
		if (putchar(*s) == EOF) {
			return EOF;
		}
		++s;
	}
	return 1;
}

char* my_strchr(const char* s, int c) {
	while (*s != '\0' && *s != c) {
		++s;
	}
	if (*s == c) {
		return (char*) s;
	}
	return NULL;
}

size_t my_strlen(const char* s) {
	size_t length = 0;
	while (*s != '\0') {
		length += 1;
		++s;
	}
	return length;
}

// Strings may not overlap!
char* my_strcpy(char* dest, const char* src) {
	char* original_dest = dest;
	while (*src != '\0') {
		*dest = *src;
		++dest;
		++src;
	}
	*dest = '\0';
	return original_dest;
}

char* my_strncpy(char* dest, const char* src, size_t n) {
	char* original_dest = dest;
	while (n > 0 && *src != '\0') {
		*dest = *src;
		++dest;
		++src;
		--n;
	}
	while (n > 0) {
		*dest = '\0';
		++dest;
		--n;
	}
	return original_dest;
}

// Strings may not overlap!
char* my_strcat(char* dest, const char* src) {
	while (*dest != '\0') {
		++dest;
	}
	while (*src != '\0') {
		*dest = *src;
		++dest;
		++src;
	}
	return dest;
}

char* my_strncat(char* dest, const char* src, size_t n) {
	char* original_dest = dest;
	while (*dest != '\0') {
		++dest;
	}
	while (n > 0 && *src != '\0') {
		*dest = *src;
		++dest;
		++src;
		--n;
	}
	return original_dest;
}

char* my_fgets(char* s, int size, FILE* stream) {
	while (size > 1) {
		int character = fgetc(stream);
		if (character == EOF) {
			break;
		}
		*s = character;
		++s;
		--size;
		if (character == '\n') {
			break;
		}
	}
	*s = '\0';
	return s;
}

char* my_strdup(const char* s) {
	size_t length = my_strlen(s);
	char* dup = malloc(length * sizeof(char));
	if (dup == NULL) {
		return NULL;
	}
	my_strcpy(dup, s);
	return dup;
}
