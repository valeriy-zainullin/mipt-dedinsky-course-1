#include "string-functions.h"

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static bool HAVE_ERRORS = false;

#define CHECK(expr) \
	if (!(expr)) { \
		fprintf(stderr, "%s%d%s%s%s", #expr  " at line ", __LINE__, " from function ", __func__, " is false.\n"); \
		HAVE_ERRORS = true; \
	}

void test_my_puts() {
	FILE* original_stdin = stdin;

	stdin = fopen("tmp.txt", "w");
	CHECK((void*) stdin != NULL);
	my_puts("TEST");
	fclose(stdin);
	stdin = original_stdin;

	FILE* stream = fopen("tmp.txt", "w");
	char buffer[4 + 1];
	CHECK(fgets(buffer, sizeof(buffer) / sizeof(char), stream) == buffer);
	CHECK(strcmp(buffer, "TEST") == 0);
	fclose(stream);
}

void test_my_strchr() {
	const char* string = "TEST";
	CHECK(my_strchr(string, 'T') == strchr(string, 'T'));
}

void test_my_strlen() {
	const char* string_1 = "TEST";
	const char* string_2 = "";
	CHECK(my_strlen(string_1) == strlen(string_1));
	CHECK(my_strlen(string_2) == strlen(string_2));
}

void test_my_strcpy() {
	char buffer[10] = "TEST1";
	CHECK(my_strcpy(buffer, "TEST2"));
	CHECK(strcmp(buffer, "TEST2") == 0);
}

void test_my_strncpy() {
	char buffer[10] = "TEST1";
	CHECK(my_strncpy(buffer, "TEST2", 4) == buffer);
	CHECK(strcmp(buffer, "TEST") == 0);
}

void test_my_strcat() {
	char buffer[9 + 1] = "TEST1";
	CHECK(strcat(buffer, "TEST") == buffer);
	CHECK(strcmp(buffer, "TEST1TEST") == 0);
}

void test_my_strncat() {
	char buffer[9 + 1] = "TEST1";
	const char* tmp = "TEST";
	CHECK(strncat(buffer, tmp, 4) == buffer);
	CHECK(strcmp(buffer, "TEST1TES") == 0);
}

/*
char* my_fgets(char* s, int size, FILE* stream);
char* my_strdup(const char* s);
*/

int main() {
	test_my_puts();
	test_my_strchr();
	test_my_strlen();
	test_my_strcpy();
	test_my_strncpy();
	test_my_strcat();
	test_my_strncat();
	/*
	test_my_fgets();
	test_my_strdup();
	*/

	return 0;
}
