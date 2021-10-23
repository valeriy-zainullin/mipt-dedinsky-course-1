#include "cli.h"
#include "debugutils.h"

#include "text.h"

#include <stdio.h>

int main(int argc, char** argv) {
	if (argc !=) {
		return (int) CLI_EXIT_CODE_INVALID_ARGUMENTS;
	}
	const char* input_file = argv[1];
	const char* output_file = argv[2];

	Text text;
	TextStatus status = text_read_from_file(&text, input_file);
	if (status != TEXT_SUCCESS) {
		cli_print_text_status(input_file, status);
		return cli_text_status_to_exit_code(status);
	}

	TextLines lines;
	status = text_select_lines(&text, &lines);
	if (status != TEXT_SUCCESS) {
		cli_print_text_status(input_file, status);
		text_free(&text);
		return cli_text_status_to_exit_code(status);
	}
	text_terminate_lines(text);
	// text_remove_empty_lines(lines);

	FILE* output_stream = fopen(output_file, "wb");
	if (output_stream == NULL) {
		cli_print_status(output_file, CLI_STATUS_FAILED_TO_OPEN);
		text_free_lines(&lines);
		text_free(&text);
		return cli_status_to_exit_code(CLI_STATUS_FAILED_TO_OPEN);
	}

	vm_assemble(lines, output_stream);

	text_free_lines(&lines);
	text_free(&text);

	return (int) CLI_EXIT_CODE_SUCCESS;
}