#pragma once

#include <stdbool.h>

struct ast_location {
	// TODO: maybe add file id.
	// unsigned int file_id;
	// TODO: Check if there are file ids before opening the file.
	size_t start_line;
	size_t start_col;
	size_t end_line;
	size_t end_col;
	char const* text;
};
