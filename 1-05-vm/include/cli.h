#pragma once

#include <stdbool.h>

#include <stdio.h>

#include "text.h"

enum CliExitCodes {
	CLI_EXIT_CODE_SUCCESS = 0,
	CLI_EXIT_CODE_INVALID_ARGUMENTS = 1
};
typedef enum CliExitCodes CliExitCode;

/*
enum CliStatus {
	CLI_STATUS_SUCCESS = 0;
};
*/

bool cli_get_streams(int argc, const char * const * argv, FILE** input_stream, FILE** output_stream);

void cli_print_text_status(const char* input_file, TextStatus text_status);

/*
int cli_text_status_to_exit_code(TextStatus text_status);
int cli_status_to_exit_code(CliStatus status);
*/
