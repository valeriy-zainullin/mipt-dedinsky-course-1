#include "text.h"
#include "string_utils.h"

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
#define PRINT_MESSAGE_FOR_FILE(FILE_PATH, MESSAGE) printf("\"%s\": %s.\n", FILE_PATH, FAILED_TO_OPEN_THE_FILE_MESSAGE)

static bool get_file_length(FILE* stream, size_t* length) {
	if (fseek(stream, 0L, SEEK_END) != 0) {
		return false;
	}
	long length_as_long = ftell(stream);
	if (length_as_long == -1L) {
		return false;
	}
	rewind(stream);

	*length = length_as_long;
	return true;
}

static void dump_lines(const char* output_file_path, TextLines lines) {
	FILE* stream = fopen(output_file_path, "w");
	if (stream == NULL) {
		PRINT_MESSAGE_FOR_FILE(output_file_path, FAILED_TO_OPEN_THE_FILE_MESSAGE);
		return;
	}
	for (size_t line_index = 0; line_index < lines.number_of_lines; ++line_index) {
		TextLine* line = &lines.lines[line_index];
		for (
			TextIterator iterator = line->first_character;
			iterator != line->after_the_last_character;
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
}

static int qsort_comparator(const void* left_hand_side, const void* right_hand_side) {
	return text_compare_substrings(* (TextSubstring*) left_hand_side, * (TextSubstring*) right_hand_side);
}

static int myqsort_comparator(const void* left_hand_side, const void* right_hand_side) {
	return text_compare_reversed_substrings(* (TextSubstring*) left_hand_side, * (TextSubstring*) right_hand_side);
}

static void process_file(const char* path) {
	Text text;
	if (!text_read_from_file(&text, path)) {
		return;
	}

	TextLines lines;
	if (!text_select_lines(text, &lines)) {
		PRINT_MESSAGE_FOR_FILE(path, FAILED_TO_ALLOCATE_MEMORY_MESSAGE);
		free(text.characters);
		return;
	}

	qsort(lines.lines, lines.number_of_lines, sizeof(TextLine), qsort_comparator);
	const char* output_file_path = string_cat(path, ".qsorted");
	if (output_file_path == NULL) {
		text_free_lines(lines);
		text_free(text);
		return;
	}
	dump_lines(output_file_path, lines);
	string_free((char*) output_file_path);

	// TODO: my_qsort.
	qsort(lines.lines, lines.number_of_lines, sizeof(TextLine), myqsort_comparator);
	output_file_path = string_cat(path, ".my_qsorted");
	if (output_file_path == NULL) {
		text_free_lines(lines);
		text_free(text);
		return;
	}
	dump_lines(output_file_path, lines);
	string_free((char*) output_file_path);

	text_free_lines(lines);
	text_free(text);
}

static void print_usage(const char* invocation) {
	printf("Usage: %s [text-file] ...\n", invocation);
	printf("Sorts text file, then writes output to a new file with \".sorted\" prefix.");
	printf("After that sorts text file in reversed order, then writes output to a new file with \".mysorted\" prefix.\n");
}

int main(int argc, const char * const * argv) {
	if (argc <= 1) {
		print_usage(argv[0]);
		return (int) RETURN_CODE_WRONG_USAGE;
	}

	size_t number_of_files = argc - 1;
	for (size_t i = 0; i < number_of_files; ++i) {
		process_file(argv[1 + i]);
	}

	return (int) RETURN_CODE_SUCCESS;
}
