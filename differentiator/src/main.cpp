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

static bool process_args(int argc, char** argv, Args* args) {
	if (argc < 2 || argc > 4) {
		fprintf(stderr, "Неправильное количество аргументов.\n");
		print_help(argv[0]);
		return false;
	}
	args->output_stream = fopen(argv[1], "w");
	if (args->output_stream == NULL) {
		fprintf(stderr, "Не удалось открыть выходной файл.\n");
		print_help(argv[0]);
		return false;
	}
	
	if (argc == 2) {
		args->output_type = OUTPUT_TYPE_PDF;
		return true;
	}
	
	if (argv[2][0] != '-') {
		fprintf(stderr, "Неверный параметр \"%s\".\n", argv[2]);
		print_help(argv[0]);
		return false;
	}
	switch (argv[2][1]) {
		case 't': args->output_type = OUTPUT_TYPE_TEXT;     break;
		case 'g': args->output_type = OUTPUT_TYPE_PICTURE;  break;
		case 'p': args->output_type = OUTPUT_TYPE_PDF;      break;
		default: {
			print_help(argv[0]);
			return false;
		}
	}
	
	return true;
}

static bool read_expression(Tree* tree) {
	printf("Введите выражение: ");
	fflush(stdout);
	
	Reader reader = {};
	reader_init(&reader, stdin, true);
	
	if (!parsing_read_expression(&reader, tree)) {
		fprintf(stderr, "Не удалось прочитать и разобрать выражение.\n");
		return false;
	}
	
	TextWriter text_writer = {};
	text_writer_init(&text_writer, stdout);
	text_writer_before_differentiation(&text_writer, tree);
	text_writer_deinit(&text_writer);
	
	reader_deinit(&reader);
	
	return true;
}
int main(int argc, char** argv) {
	
	return 0;
}
