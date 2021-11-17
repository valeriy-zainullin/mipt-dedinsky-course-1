#include "database.h"

#include "tree/tree.h"

#include "macro_utils.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#define DATABASE_MAX_STRING_LENGTH_MACRO 256
static const size_t DATABASE_MAX_STRING_LENGTH = DATABASE_MAX_STRING_LENGTH_MACRO;

static bool database_read_node(TreeNode** node, FILE* stream) {
	char string[DATABASE_MAX_STRING_LENGTH + 1] = {};
	int num_chars_read = 0;

	fscanf(stream, " { \"%" EXPANDED_TO_STRING(DATABASE_MAX_STRING_LENGTH_MACRO) "[^\"]\"%n", string, &num_chars_read);

	// Могли пропустить пробелы, а по формату всё равно не совпать.
	if (num_chars_read == 0) {
		
		if (feof(stream)) {

			LOG_ERROR("Поток закончился слишком рано");
			return false;

		} else if (ferror(stream)) {

			LOG_ERROR("Ошибка при чтении потока");
			return false;
		}

		LOG_ERROR("Неверный формат базы данных");
		return false;
	}

	*node = (TreeNode*) calloc(1, sizeof(TreeNode));
	if (*node == NULL) {
		LOG_ERROR("Не удалось выделить память");
		return false;
	}

	(*node)->left = NULL;
	(*node)->right = NULL;

	strcpy((*node)->value, string);

	char bracket[2] = {};
	fscanf(stream, " %1[{}]", bracket);

	if (bracket[0] == '{') {
		if (ungetc(bracket[0], stream) == EOF) {
			LOG_ERROR("Не удалось вернуть символ в поток");
			return false;
		}

		if (!database_read_node(&(*node)->left, stream)) {
			return false;
		}

		if (!database_read_node(&(*node)->right, stream)) {
			return false;
		}

		bracket[0] = '\0';
		fscanf(stream, " %1[}]", bracket);

		if (bracket[0] != '}') {
			LOG_ERROR("Неверный формат базы данных");
			return false;
		}
	
	}

	return true;

}

// Запретить "
bool database_read(Tree* tree, FILE* stream) {
	assert(tree != NULL);
	assert(stream != NULL);

	return database_read_node(&tree->root, stream);
}

static void database_save_node(TreeNode* node, FILE* stream) {
	fprintf(stream, "{\"%s\"", node->value);

	assert((node->left == NULL) == (node->right == NULL));

	if (node->left != NULL) {
		fprintf(stream, " ");

		database_save_node(node->left, stream);
	}

	if (node->right != NULL) {
		fprintf(stream, " ");

		database_save_node(node->right, stream);
	}

	fprintf(stream, "}");
}

bool database_save(Tree* tree, FILE* stream) {
	assert(tree != NULL);
	assert(stream != NULL);

	database_save_node(tree->root, stream);

	return !ferror(stream);
}
