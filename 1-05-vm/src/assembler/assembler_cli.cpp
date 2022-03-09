#include "assembler/assembler.h"
#include "assembly/program.h"
#include "assembly/status.h"
#include "cli.h"
#include "support/debug_utils.h"

#include "text.h"

#include <stdio.h>
#include <stdlib.h>

static const int VM_ASSEMBLER_NUMBER_OF_PASSES = 2;

int main(int argc, char** argv) {
	if (argc != 3) {
		return 1;
	}
	const char* input_file = argv[1];
	const char* output_file = argv[2];

	Text text;
	TextStatus status = text_read_from_file(&text, input_file);
	if (status != TEXT_SUCCESS) {
		return 2;
	}

	TextLines lines;
	if (!text_select_lines(text, &lines)) {
		text_free(&text);
		return 3;
	}
	text_terminate_lines(text);

	FILE* output_stream = fopen(output_file, "wb");
	if (output_stream == NULL) {
		text_free_lines(&lines);
		text_free(&text);
		return 4;
	}

	VMAssembler* assembler = (VMAssembler*) calloc(sizeof(VMAssembler), 1);

	// VmStream
	if (!vm_assembler_init(assembler, output_stream)) {
		return 5;
	}

	VMAssemblyStatus assembly_status = {};

	for (int i = 1; i <= VM_ASSEMBLER_NUMBER_OF_PASSES; ++i) {
		// rewind(output_stream);
		fclose(output_stream);
		output_stream = fopen(output_file, "wb");
		if (output_stream == NULL) {
			break;
		}
		assembler->output_file = output_stream; // TODO: name it output stream.

		assembler->pass = i;
		assembly_status = vm_text_process_program(&lines, (void*) assembler);

		if (assembly_status.error != VM_SUCCESS) {
			break;
		}
	}

	vm_assembler_deinit(assembler);
	free(assembler);
	fclose(output_stream);

	if (assembly_status.error != VM_SUCCESS) {
		const char* description = vm_status_to_string(assembly_status.error);
		printf("Строка %zu: %s.\n", assembly_status.line + 1, description);
	
		if (remove(output_file) != 0) {
			printf("Не удалось удалить выходной файл.\n");
		}
	}

	text_free_lines(&lines);
	text_free(&text);

	return 0;
}
