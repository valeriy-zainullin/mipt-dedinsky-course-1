#include "cli.h"
#include "debugutils.h"
#include "assembler.h"

#include "text.h"

#include <stdio.h>

int main(int argc, char** argv) {
	if (argc != 2) {
		return 1;
	}
	const char* input_file = argv[1];

	FILE* input_stream = fopen(input_file, "wb");
	if (input_stream == NULL) {
		return 4;
	}

	vm_execute(input_stream, stdout);

	// return (int) CLI_EXIT_CODE_SUCCESS;
	return 0;
}