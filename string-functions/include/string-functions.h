#pragma once

#include <stddef.h>
#include <stdio.h>

int my_puts(const char* s);
char* my_strchr(const char* s, int c);
size_t my_strlen(const char* s);
char* my_strcpy(char* dest, const char* src);
char* my_strncpy(char* dest, const char* src, size_t n);
char* my_strcat(char* dest, const char* src);
char* my_strncat(char* dest, const char* src, size_t n);
char* my_fgets(char* s, int size, FILE* stream);
char* my_strdup(const char* s);
