#include "Text.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

enum RETURN_CODE {
	RETURN_CODE_WRONG_USAGE = 1,
	RETURN_CODE_SUCCESS = 0
};

// Messages.
#if defined(_WIN32)
static const char * const FAILED_TO_OPEN_THE_FILE_MESSAGE = "не удалось открыть файл";
static const char * const FAILED_TO_GET_LENGTH_OF_THE_FILE_MESSAGE = "не удалось выяснить длину файла";
#else
static const char * const FAILED_TO_OPEN_THE_FILE_MESSAGE = "";
#endif
// ----
#define PRINT_MESSAGE_FOR_FILE(FILE, MESSAGE) printf("\"%s\": %s.\n", path, FAILED_TO_OPEN_THE_FILE_MESSAGE)

static bool get_file_length(FILE* stream, size_t* length) {
	if (fseek(stream, 0L, SEEK_END) != 0) {
		PRINT_MESSAGE_FOR_FILE(path, FAILED_TO_GET_LENGTH_OF_THE_FILE_MESSAGE);
		return false;
	}
	long length_as_long = ftell(stream);
	if (length_as_long == -1L) {
		PRINT_MESSAGE_FOR_FILE(path, FAILED_TO_GET_LENGTH_OF_THE_FILE_MESSAGE);
		fclose(stream);
		return false;
	}
	rewind(stream);

	length = length_as_long;
	return true;
}

static bool read_text(const char* path, Text* text_ptr) {
	FILE* stream = fopen(path, "r");
	if (stream == NULL) {
		PRINT_MESSAGE_FOR_FILE(path, FAILED_TO_OPEN_THE_FILE_MESSAGE);
		return false;
	}
	size_t length;
	if (!get_file_length(stream, &length)) {
		fclose(stream);
		return false;
	}

	text_ptr->number_of_characters = length;
	text_ptr->characters = malloc(length * sizeof(unsigned char));
	if (text_ptr->characters == NULL) {
		PRINT_MESSAGE_FOR_FILE(path, FAILED_TO_ALLOCATE_MEMORY_MESSAGE);
		fclose(stream);
		return false;
	}
	if (
		fread(stream, sizeof(unsigned char), text_ptr->number_of_characters, text_ptr->characters) <
		text_ptr->number_of_characters
	) {
		PRINT_MESSAGE_FOR_FILE(path, READ_LESS_THAN_EXPECTED_MESSAGE);
		free(text_ptr->characters);
		fclose(stream);
		return false;
	}
	fclose(stream);
	return true;
}
static void free_text(Text text) {
	free(text.characters);
}

static void dump_lines(const char* path, TextLines lines) {
	stream = fopen(output_file_path, "w");
	if (stream == NULL) {
		PRINT_MESSAGE_FOR_FILE(path, FAILED_TO_OPEN_THE_FILE_MESSAGE);
		return;
	}
	for (size_t line_index = 0; line_index < lines.number_of_lines; ++line_index) {
		TextLine* line = lines.lines[line_index];
		for (
			TextIterator iterator = line.first_character;
			iterator != line.after_the_last_character;
			++iterator
		) {
			if (fputc(*iterator, stream) == EOF) {
				break;
			}
		}
		if (fputc('\n', stream) == EOF) {
			break;
		}
	}
	StringFree(output_file_path);
}

static void process_file(const char* path) {
	Text text = {NULL};
	if (!read_text(path, text)) {
		return;
	}

	TextLines lines = {NULL};
	if (!text_select_lines(text, &lines)) {
		PRINT_MESSAGE_FOR_FILE(path, FAILED_TO_ALLOCATE_MEMORY_MESSAGE);
		free(text.characters);
		return;
	}

	qsort(lines.lines, lines.number_of_lines, sizeof(TextLine), text_compare_substrings);
	const char* output_file_path = StringCat(path, ".qsorted");
	if (output_file_path == NULL) {
		text_free_lines(lines);
		free_text(text);
		return;
	}
	print_lines(output_file_path, lines.lines);
	StringFree(output_file_path);

	my_qsort(lines.lines, lines.number_of_lines, sizeof(TextLine), text_compare_reversed_substrings);
	output_file_path = StringCat(path, ".my_qsorted");
	if (output_file_path == NULL) {
		text_free_lines(lines);
		free_text(text);
		return;
	}
	print_lines(output_file_path, lines.lines);
	StringFree(output_file_path);

	text_free_lines(lines);
	free_text(text);
}

int main(int argc, const char * const * argv) {
	if (argc <= 1) {
		print_usage();
		return (int) RETURN_CODE_WRONG_USAGE;
	}

	size_t number_of_files = argc - 1;
	for (size_t i = 0; i < number_of_files; ++i) {
		process_file(argv[1 + i]);
	}

	return (int) RETURN_CODE_SUCCESS;
}