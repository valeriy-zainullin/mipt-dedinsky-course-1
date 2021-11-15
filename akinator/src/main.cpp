#include "tree/tree.h"

#include "database.h"
#include "macro_utils.h"
#include "tts.h"

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define PRODUCE_OUTPUT(...) printf(__VA_ARGS__); tts_say(__VA_ARGS__)

#define MAX_LINE_LENGTH_MACRO 1024

static const size_t MAX_LINE_LENGTH = MAX_LINE_LENGTH_MACRO;

enum AkinatorMode {
	AKINATOR_MODE_GUESS,
	AKINATOR_MODE_COMPARE,
	AKINATOR_MODE_GET_DEFINITION,
	AKINATOR_MODE_EXIT,
	AKINATOR_MODE_COUNT
};

static bool read_line(char* line) {
	assert(line != NULL);

	line[0] = '\0';

	int num_chars_read = 0;

	// 0 if '\n' immediately follows.
	int num_read = scanf(" %" EXPANDED_TO_STRING(MAX_LINE_LENGTH_MACRO) "[^\n]%n", line, &num_chars_read);

	if (num_read < 0) {
		return false;
	}

	return true;

}

static AkinatorMode read_mode() {

	PRODUCE_OUTPUT("Что хочешь, дорогой?\n");
	PRODUCE_OUTPUT("0 --- угадать объект.\n");
	PRODUCE_OUTPUT("1 --- сравнить объекты.\n");
	PRODUCE_OUTPUT("2 --- дать определение объекту.\n");
	PRODUCE_OUTPUT("3 --- выйти (с обновлением базы данных).\n");

	fflush(stdout);

	int choice = 0;

	char line[MAX_LINE_LENGTH + 1] = {};

	while (read_line(line)) {
		int num_read = sscanf(line, "%d", &choice);

		if (num_read < 0) {
			LOG_ERROR("Не удалось прочитать выбор.\n");
			continue;
		}

		if (0 <= choice && choice < (int) AKINATOR_MODE_COUNT) {
			return (AkinatorMode) choice;
		}
	}

	return AKINATOR_MODE_EXIT;

}

static int guess_object(Node* node) {
	printf("Это %s? [Да/Нет]\n", node->value);
	fflush(stdout);


}

static void process_guess_request(Tree* tree) {
	tree_visit_depth_first(tree, guess_object, NULL);
}

static void process_comparison_request(Tree* tree) {
	(void) tree;
}

static void process_definition_request(Tree* tree) {
	(void) tree;
}

static bool read_database(Tree* tree, const char* filename) {
	FILE* stream = fopen(filename, "r");
	if (stream == NULL) {
		return false;
	}

	if (!database_read(tree, stream)) {
		fclose(stream);
		return false;
	}

	fclose(stream);

	return true;
}

static bool save_database(Tree* tree, const char* filename) {
	FILE* stream = fopen(filename, "w");
	if (stream == NULL) {
		return false;
	}

	if (!database_save(tree, stream)) {
		fclose(stream);
		return false;
	}

	fclose(stream);

	return true;
}

static const int AKINATOR_EXIT_FAILED_TO_SAVE_DATABASE = 4;
static const int AKINATOR_EXIT_FAILED_TO_READ_DATABASE = 3;
static const int AKINATOR_EXIT_INVALID_ARGUMENTS = 2;
static const int AKINATOR_EXIT_ERROR = 1;
static const int AKINATOR_EXIT_SUCCESS = 0;

int main(int argc, const char** argv) {
	if (argc != 2) {
		return AKINATOR_EXIT_INVALID_ARGUMENTS;
	}

	Tree tree = {};

	tree_init(&tree);

	if (!read_database(&tree, argv[1])) {
		tree_deinit(&tree);
		return AKINATOR_EXIT_FAILED_TO_READ_DATABASE;
	}

	while (true) {
		AkinatorMode mode = read_mode();

		if (mode == AKINATOR_MODE_EXIT) {
			break;
		}

		switch (mode) {
			case AKINATOR_MODE_GUESS:          process_guess_request(&tree);      break;
			case AKINATOR_MODE_COMPARE:        process_comparison_request(&tree); break;
			case AKINATOR_MODE_GET_DEFINITION: process_definition_request(&tree); break;

			case AKINATOR_MODE_EXIT:
			case AKINATOR_MODE_COUNT:
			default: UNREACHABLE;
		}
	}

	if (!save_database(&tree, argv[1])) {
		tree_deinit(&tree);
		return AKINATOR_EXIT_FAILED_TO_SAVE_DATABASE;
	}

	if (!tree_deinit(&tree)) {
		return AKINATOR_EXIT_ERROR;
	}

	return AKINATOR_EXIT_SUCCESS;
}
