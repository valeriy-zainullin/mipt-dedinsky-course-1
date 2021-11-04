#include "assembler/assembler.h"
#include "assembly/program.h"
#include "cli.h"
#include "support/debug_utils.h"

#include "text.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	if (argc != 3) {
		return 1;
	}
	const char* input_file = argv[1];
	const char* output_file = argv[2];

	Text text;
	TextStatus status = text_read_from_file(&text, input_file);
	if (status != TEXT_SUCCESS) {
		/*
		cli_print_text_status(input_file, status);
		return cli_text_status_to_exit_code(status);
		*/
		return 2;
	}

	TextLines lines;
	/*status = */ if (!text_select_lines(text, &lines))//;
	//if (status != TEXT_SUCCESS) {
	{
		//cli_print_text_status(input_file, status);
		text_free(&text);
		// return cli_text_status_to_exit_code(status);
		return 3;
	}
	text_terminate_lines(text);
	// text_remove_empty_lines(lines);

	FILE* output_stream = fopen(output_file, "wb");
	if (output_stream == NULL) {
		//cli_print_status(output_file, CLI_STATUS_FAILED_TO_OPEN);
		text_free_lines(&lines);
		text_free(&text);
		// return cli_status_to_exit_code(CLI_STATUS_FAILED_TO_OPEN);
		return 4;
	}

	VmAssembler* assembler = (VmAssembler*) calloc(sizeof(VmAssembler), 1);

	assembler->output_file = output_stream;
	assembler->ip = 0;
	assembler->labels.nlabels = 0;

	vm_text_process_program(&lines, (void*) assembler);

	free(assembler);
	text_free_lines(&lines);
	text_free(&text);

	// return (int) CLI_EXIT_CODE_SUCCESS;
	return 0;
}