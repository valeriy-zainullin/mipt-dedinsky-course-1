#include "tree/tree.h"

#include "database.h"
#include "macro_utils.h"
#include "tts.h"

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRODUCE_OUTPUT(...) printf(__VA_ARGS__); tts_say(__VA_ARGS__)

#define MAX_LINE_LENGTH_MACRO 1024

static const size_t MAX_LINE_LENGTH = MAX_LINE_LENGTH_MACRO;

enum AkinatorMode {
	AKINATOR_MODE_GUESS,
	AKINATOR_MODE_COMPARE,
	AKINATOR_MODE_GET_DEFINITION,
	AKINATOR_MODE_DUMP,
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
	PRODUCE_OUTPUT("3 --- создать графический дамп базы данных.\n");
	PRODUCE_OUTPUT("4 --- выйти (с обновлением базы данных).\n");

	fflush(stdout);

	int choice = 0;

	char line[MAX_LINE_LENGTH + 1] = {};

	while (read_line(line)) {
		int num_read = sscanf(line, "%d", &choice);

		if (num_read < 1) {
			LOG_ERROR("Не удалось прочитать выбор ");
			continue;
		}

		if (0 <= choice && choice < (int) AKINATOR_MODE_COUNT) {
			return (AkinatorMode) choice;
		}
	}

	return AKINATOR_MODE_EXIT;

}

static int guess_object(TreeNode* node, void* arg) {
	assert(node != NULL);
	
	TreeNode*** parent_link = (TreeNode***) arg;

	PRODUCE_OUTPUT("Это %s? [Да/Нет]\n", node->value);
	fflush(stdout);

	char line[MAX_LINE_LENGTH + 1] = {};

	while (read_line(line)) {
		if (strcmp(line, "Да") == 0) {
			if (node->left != NULL) {
				*parent_link = &node->left;
				return TREE_DIRECTION_LEFT;
			} else {
				PRODUCE_OUTPUT("Видишь, я умный.\n");
				fflush(stdout);
				return TREE_DIRECTION_NONE;
			}
		} else if (strcmp(line, "Нет") == 0) {
			if (node->right != NULL) {
				*parent_link = &node->right;
				return TREE_DIRECTION_RIGHT;
			} else {
				PRODUCE_OUTPUT("Не угадал. Кто это был?\n");
				fflush(stdout);
				if (!read_line(line)) {
					return TREE_DIRECTION_NONE;
				}
				
				PRODUCE_OUTPUT("А чем %s отличается от %s? ", line, node->value);
				PRODUCE_OUTPUT("Это ");
				fflush(stdout);

				char difference[MAX_LINE_LENGTH + 1] = {};
				if (!read_line(difference)) {
					return false;
				}

				TreeNode* object_node = (TreeNode*) calloc(1, sizeof(TreeNode));
				if (object_node == NULL) {
					return TREE_DIRECTION_NONE;
				}
				object_node->left = NULL;
				object_node->right = NULL;
				strcpy(object_node->value, line);

				TreeNode* question_node = (TreeNode*) calloc(1, sizeof(TreeNode));
				if (node == NULL) {
					return TREE_DIRECTION_NONE;
				}
				strcpy(question_node->value, difference);

				question_node->left = object_node;
				question_node->right = node;

				**parent_link = question_node;
				return TREE_DIRECTION_NONE;
			}
		} else {
			PRODUCE_OUTPUT("Не понял. Повтори пожалуйста.\n");
			fflush(stdout);
		}
	}

	return TREE_DIRECTION_NONE;
}

static void process_guess_request(Tree* tree) {
	TreeNode** parent_link = &tree->root;
	tree_visit_depth_first(tree, guess_object, &parent_link);
}

static void process_comparison_request(Tree* tree) {
	(void) tree;
}

static void process_definition_request(Tree* tree) {
	(void) tree;
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

static const size_t MAX_COMMAND_LENGTH = 1024;

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

	tree_visit_depth_first(tree, dump_node, stream);
	
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
