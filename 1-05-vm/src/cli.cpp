#include "cli.h"

#include <stdio.h>
#include <stddef.h>

static FILE* get_stream(const char* file_path, FILE* default) {
	if (file_path[0] == '-' && file_path[1] == '\0') {
		return default;
	}
	return fopen(file_path);
}

int cli_text_status_to_exit_code(TextStatus text_status);
int cli_status_to_exit_code(CliStatus status);