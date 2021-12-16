#include "differentiation/differentiation.h"
#include "input/parsing.h"

#include <stddef.h>
#include <stdlib.h>

enum OutputType {
	OUTPUT_TYPE_TEXT,
	OUTPUT_TYPE_PICTURE,
	OUTPUT_TYPE_PDF
};

// const char* path? Args.
struct Args {
	FILE* output_stream;
	OutputType output_type;
	bool is_debug_mode;
};

	}
	FILE* output_stream = fopen(argv[1]);
	if (output_stream == NULL) {
		fprintf(stderr, "Не удалось открыть выходной файл.\n");
		return 2;
	}
	}
	
	printf("Введите выражение: ");
	fflush(stdout);
	
	Tree tree = {};
	
	if (!parsing_read_expression(reader, tree)) {
		fprintf(stderr, "Не удалось разобрать выражение.\n");
		fclose(output_stream);
		return 4;
	}
	
	latex_write(output_stream, tree);
	
	reader_deinit(&reader);
	fclose(output_stream);
int main(int argc, char** argv) {
	
	return 0;
}
