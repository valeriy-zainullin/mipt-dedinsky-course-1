#include "differentiation/differentiation.h"
#include "input/parsing.h"

#include <stddef.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	if (argc != 2) {
		return 1;
	}
	FILE* output_stream = fopen(argv[1]);
	if (output_stream == NULL) {
		fprintf(stderr, "Не удалось открыть выходной файл.\n");
		return 2;
	}

	Reader reader = {};
	if (!reader_init(&reader, stdin, true)) {
		fprintf(stderr, "Не удалось инициализировать читатель.\n");
		fclose(output_stream);
		return 3;
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
	
	return 0;
}
