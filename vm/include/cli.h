#pragma once

#include <stdbool.h>

#include <stdio.h>

enum CLI_EXIT_CODES {
	CLI_EXIT_CODE_SUCCESS = 0,
	CLI_EXIT_CODE_INVALID_ARGUMENTS = 1
};
typedef enum CLI_EXIT_CODES CLI_EXIT_CODE;


bool cli_get_streams(int argc, const char * const * argv, FILE** input_stream, FILE** output_stream);