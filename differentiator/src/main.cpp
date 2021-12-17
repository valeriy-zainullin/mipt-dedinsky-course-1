#include "differentiation/differentiation.h"
#include "input/parsing.h"
#include "macroutils.h"
#include "output/graphviz.h"
#include "output/latex.h"
#include "output/text.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

static void print_help(const char* program_name) {
	// Наоборот, сначала флаги?
	fprintf(stderr, "Использование: %s <путь к выходному файлу> [-t | -g | -p] [-d]\n", program_name);
	fprintf(stderr, "Дифференцирует выражение из стандартного ввода и выдаёт шаги дифференцирования и ответ в выходной файл.\n");
	fprintf(stderr, "Типы вывода: вывод в текстовом виде (-t), вывод деревьев выражений в виде картинки с помощью graphviz (-g), вывод pdf-файла (-p, по умолчанию).");
	fprintf(stderr, "Флаг -d при выводе деревьев выражений с помощью graphviz (-g) выдаёт исходный текст для graphviz в stderr и добавляет адреса узлов в картинку, при выводе pdf-файла выдаёт код для pdflatex в stderr, для остальных случаев он игнорируется.\n");
	fprintf(stderr, "Синтаксис и поддерживаемые функции: TODO.\n");
}

enum OutputType {
	OUTPUT_TYPE_TEXT,
	OUTPUT_TYPE_GRAPHVIZ, // TREE_PICTURE ?
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
		case 'g': args->output_type = OUTPUT_TYPE_GRAPHVIZ; break;
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
	
	reader_deinit(&reader);
	
	return true;
}

static bool init_differentiation_callbacks(Args* args, DifferentiationCallbacks* callbacks) {
	switch (EXPECT(args->output_type, OUTPUT_TYPE_PDF)) {
		case OUTPUT_TYPE_TEXT: {
			TextWriter* text_writer = (TextWriter*) calloc(1, sizeof(TextWriter));
			if (text_writer == NULL) {
				return false;
			}
			text_writer_init(text_writer, args->output_stream);
			
			callbacks->before_differentiation = &text_writer_before_differentiation;
			callbacks->on_differentiation_started = &text_writer_on_differentiation_started;
			callbacks->on_differentiation_ended = &text_writer_on_differentiation_ended;
			callbacks->after_differentiation = &text_writer_after_differentiation;
			callbacks->arg = text_writer;
			
			break;
		}
		
		case OUTPUT_TYPE_GRAPHVIZ: {
			// Picture writer?
			GraphvizWriter* graphviz_writer = (GraphvizWriter*) calloc(1, sizeof(GraphvizWriter));
			if (graphviz_writer == NULL) {
				return false;
			}
			graphviz_writer_init(graphviz_writer, args->output_stream, args->is_debug_mode);
			
			callbacks->before_differentiation = &graphviz_writer_before_differentiation;
			callbacks->on_differentiation_started = &graphviz_writer_on_differentiation_started;
			callbacks->on_differentiation_ended = &graphviz_writer_on_differentiation_ended;
			callbacks->after_differentiation = &graphviz_writer_after_differentiation;
			callbacks->arg = graphviz_writer;
			
			break;
		}
		
		/*
		
		case OUTPUT_TYPE_PDF: {
			PdfWriter pdf_writer = {};
			pdf_writer_init(&pdf_writer, output_file, is_debug_mode);
			
			callbacks.on_differentiation_started = &pdf_writer_on_differentiation_started;
			callbacks.on_differentiation_ended = &pdf_writer_on_differentiation_ended;
			callbacks.arg = &pdf_writer;
			
			pdf_writer_before_differentiation(&pdf_writer);
			
			break;
		}
		
		*/
		
		default: assert(false); UNREACHABLE;
	}
	
	return true;
}

static bool do_differentiation(Tree* tree, DifferentiationCallbacks* callbacks) {
	Tree output_tree = {};
	if (!differentiate(tree, &output_tree, callbacks)) {
		fprintf(stderr, "Не удалось продифференцировать выражение.\n");
		return false;
	}
	// TODO: tree destructor, tree constructor?
	tree_node_deinit_deallocate_subtree(&output_tree.root);
	
	return true;
}

static void deinit_differentiation_callbacks(Args* args, DifferentiationCallbacks* callbacks) {
	switch (EXPECT(args->output_type, OUTPUT_TYPE_PDF)) {
		case OUTPUT_TYPE_TEXT: {
			TextWriter* text_writer = (TextWriter*) callbacks->arg;
			
			// text_writer_after_differentiation(text_writer, tree);
			text_writer_deinit(text_writer);
			
			free(text_writer);
			
			break;
		}
		
		case OUTPUT_TYPE_GRAPHVIZ: {
			GraphvizWriter* graphviz_writer = (GraphvizWriter*) callbacks->arg;
			
			// graphviz_writer_after_differentiation(graphviz_writer, tree);
			graphviz_writer_deinit(graphviz_writer);
			
			free(graphviz_writer);
			
			break;
		}
		
		default: assert(false); UNREACHABLE;
	}
}

static const int SUCCESS = 0;
static const int WRONG_USAGE = 1;
static const int ERROR_WHILE_READING = 2;

int main(int argc, char** argv) {
	Args args = {};
	if (!process_args(argc, argv, &args)) {
		return WRONG_USAGE;
	}
	
	Tree tree = {};
	if (!read_expression(&tree)) {
		fclose(args.output_stream);
		return ERROR_WHILE_READING;
	}
	
	DifferentiationCallbacks callbacks = {};
	
	if (!init_differentiation_callbacks(&args, &callbacks)) {
		return ERROR_WHILE_READING;// ERROR_WHILE_INITIALIZING;
	}
	
	do_differentiation(&tree, &callbacks);
	
	deinit_differentiation_callbacks(&args, &callbacks);
	
	(void) &init_differentiation_callbacks;
	(void) &do_differentiation;
	(void) &deinit_differentiation_callbacks;
	
	// TODO: tree destructor, tree constructor?
	tree_node_deinit_deallocate_subtree(&tree.root);
	
	fclose(args.output_stream);
	
	return 0;
}
