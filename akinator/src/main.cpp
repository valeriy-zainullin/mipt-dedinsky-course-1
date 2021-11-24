#include "tree/tree.h"

#include "akinator/akinator.h"
#include "akinator/database.h"
#include "support/macro_utils.h"
#include "support/tts.h"

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRODUCE_OUTPUT(...) printf(__VA_ARGS__); fflush(stdout); tts_say(__VA_ARGS__)

#define MAX_LINE_LENGTH_MACRO 1024
static const size_t MAX_LINE_LENGTH = MAX_LINE_LENGTH_MACRO;
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

static int dump_node(TreeNode* node, void* arg) {
	FILE* stream = (FILE*) arg;

	assert((node->left == NULL) == (node->right == NULL));

	if (node->left != NULL) {
		// Escape characters.
		fprintf(
			stream,

			"node_%p [shape=record,label=\"{ %s | <left> %p | <right> %p}\" ];\n",

			(void*) node,
			node->value,
			(void*) node->left,
			(void*) node->right
		);

		fprintf(stream, "node_%p:<left> -> node_%p [label=\"Да (left)\"];\n", (void*) node, (void*) node->left);
		fprintf(stream, "node_%p:<right> -> node_%p [label=\"Нет (right)\"];\n", (void*) node, (void*) node->right);

		return TREE_DIRECTION_LEFT | TREE_DIRECTION_RIGHT;
	} else {
		// Escape characters.
		fprintf(
			stream,

			"node_%p [shape=record,label=\"{ %s }\" ];\n",

			(void*) node,
			node->value
		);

		return TREE_DIRECTION_NONE;
	}
}

static const size_t MAX_COMMAND_LENGTH = 2 * MAX_LINE_LENGTH + 25;

static void process_dump_request(Tree* tree) {
	PRODUCE_OUTPUT("Введите имя выходного файла: ");
	fflush(stdout);

	char filename[MAX_LINE_LENGTH + 1] = {};
	if (!read_line(filename)) {
		return;
	}

	char txt_file_name[MAX_LINE_LENGTH + 4 + 1] = {};
	strcpy(txt_file_name, filename);
	strcat(txt_file_name, ".txt");

	FILE* stream = fopen(txt_file_name, "w");
	if (stream == NULL) {
		return;
	}

	fprintf(stream, "digraph G {\n");

	tree_visit_depth_first(tree, dump_node, NULL, stream);
	
	fprintf(stream, "}\n");

	if (ferror(stream)) {
		LOG_ERROR("Ошибка при записи дампа");
		fclose(stream);
		return;
	}

	fclose(stream);

	char command[MAX_COMMAND_LENGTH + 1] = {};

	snprintf(command, MAX_COMMAND_LENGTH, "dot -q -Tpng %s > %s", txt_file_name, filename);
	system(command);
	
	snprintf(command, MAX_COMMAND_LENGTH, "xdg-open %s", filename);
	system(command);
	
	printf("Нажмите Enter, чтобы продолжить.\n");
	fgetc(stdin);
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
		AkinatorMode mode = akinator_read_mode();

		if (mode == AKINATOR_MODE_EXIT) {
			break;
		}

		switch (mode) {
			case AKINATOR_MODE_GUESS:          akinator_process_guess_request(&tree);      break;
			case AKINATOR_MODE_COMPARE:        akinator_process_comparison_request(&tree); break;
			case AKINATOR_MODE_GET_DEFINITION: akinator_process_definition_request(&tree); break;
			case AKINATOR_MODE_DUMP:           process_dump_request(&tree);       break;

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
