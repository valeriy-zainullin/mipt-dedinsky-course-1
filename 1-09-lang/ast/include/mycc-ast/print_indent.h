#pragma once

#include <stddef.h>
#include <stdio.h>

inline void print_indent(FILE* file, size_t indent_level) {
	while (indent_level > 0) {
		fprintf(file, "  ");
		indent_level -= 1;
	}
}
