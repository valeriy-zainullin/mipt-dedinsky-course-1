#include "differentiation/differentiation.h"
#include "input/parsing.h"

#include <stddef.h>
#include <stdlib.h>

static void print_help(const char* program_name) {
	// Наоборот, сначала флаги?
	fprintf(stderr, "Использование: %s <путь к выходному файлу> [-t | -g | -p] [-d]\n", program_name);
	fprintf(stderr, "Дифференцирует выражение из стандартного ввода и выдаёт шаги дифференцирования и ответ в выходной файл.\n");
	fprintf(stderr, "Типы вывода: вывод в текстовом виде (-t), вывод деревьев выражений в виде картинки с помощью graphviz (-g), вывод pdf-файла (-p, по умолчанию).");
	fprintf(stderr, "Флаг -d при выводе деревьев выражений с помощью graphviz (-g) выдаёт исходный текст для graphviz в stdout, при выводе pdf-файла выдаёт код для pdflatex в stdout, для остальных случаев он игнорируется.\n");
	fprintf(stderr, "Синтаксис и поддерживаемые функции: TODO.\n");
}

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
