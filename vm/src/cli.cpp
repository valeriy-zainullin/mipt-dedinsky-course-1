#include "cli.h"

#include <stdio.h>
#include <stddef.h>

static FILE* get_stream(const char* file_path, FILE* default) {
	if (file_path[0] == '-' && file_path[1] == '\0') {
		return default;
	}
	return fopen(file_path);
}

bool cli_parse_args(int argc, const char * const * argv, FILE** input_stream, FILE** output_stream) {
	*input_stream = stdin;
	*output_stream = stdout;

#if CLI_SUPPORTS_STDIN
	if (argc >= 1) {
		*input_stream = get_stream(argv[1]);
	}
	if (argc >= 2) {
		*output_stream = get_stream(argv[2]);
	}

	if (*input_stream == NULL || *output_stream == NULL) {
		return false;
	}
#else
	if (argc <= 1) {
		return false;
	}

	*input_stream = argv[1];
	if (argc > 2) {
		*output_stream = argv[2];
	}
#endif

	return true;
}