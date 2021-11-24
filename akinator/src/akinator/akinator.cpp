#include "akinator/akinator.h"

#include "akinator/vector.h"
#include "support/tts.h"
#include "tree/macro_utils.h"
#include "tree/tree.h"

#include <assert.h>
#include <stdio.h>
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

AkinatorMode akinator_read_mode() {

	PRODUCE_OUTPUT("Что хочешь, дорогой?\n");
	PRODUCE_OUTPUT("0 --- угадать объект.\n");
	PRODUCE_OUTPUT("1 --- сравнить объекты.\n");
	PRODUCE_OUTPUT("2 --- дать определение объекту.\n");
	PRODUCE_OUTPUT("3 --- создать графический дамп базы данных.\n");
	PRODUCE_OUTPUT("4 --- выйти (с обновлением базы данных).\n");

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

struct FindNodeByValueArg {
	char* value;
	TreeNode** result;
	VectorString* characteristics;
};

// Case-insensitive? UTF-8?
int akinator_find_node_by_value_callback_on_enter(TreeNode* node, void* arg) {
	assert(node != NULL);
	assert(arg != NULL);
	
	FindNodeByValueArg* callback_arg = (FindNodeByValueArg*) arg;
	char* value = callback_arg->value;
	TreeNode** result = callback_arg->result;
	VectorString* characteristics = callback_arg->characteristics;
	
	if (strcmp(node->value, value) == 0) {
		*result = node;
		return TREE_DIRECTION_NONE;
	}
	
	if (characteristics != NULL) {
		vector_string_push(characteristics, node->value);
	}

	
	return TREE_DIRECTION_LEFT | TREE_DIRECTION_RIGHT;
}

void akinator_find_node_by_value_callback_on_leave(TreeNode* node, void* arg) {
	assert(node != NULL);
	assert(arg != NULL);
	
	FindNodeByValueArg* callback_arg = (FindNodeByValueArg*) arg;
	
	if (callback_arg->result == NULL && callback_arg->characteristics != NULL) {
		char* value = NULL;
		
		/*
		mkdir -p build/obj/release/src/akinator/
		g++ -c -pedantic -Werror -std=c++11 -Iinclude -DCLI_SUPPORTS_STDIN=0 -Wall -Wextra -Weffc++ -Wc++0x-compat -Wc++11-compat -Wc++14-compat -Waggressive-loop-optimizations -Walloc-zero -Walloca -Walloca-larger-than=8192 -Warray-bounds -Wcast-align -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wdangling-else -Wduplicated-branches -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Wformat-overflow=2 -Wformat-truncation=2 -Winline -Wlarger-than=8192 -Wvla-larger-than=8192 -Wlogical-op -Wmissing-declarations -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wrestrict -Wshadow -Wsign-promo -Wstack-usage=8192 -Wstrict-null-sentinel -Wstrict-overflow=2 -Wstringop-overflow=4 -Wsuggest-attribute=noreturn -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wnarrowing -Wno-old-style-cast -Wvarargs -Waligned-new -Walloc-size-larger-than=1073741824 -Walloc-zero -Walloca -Walloca-larger-than=8192 -Wdangling-else -Wduplicated-branches -Wformat-overflow=2 -Wformat-truncation=2 -Wrestrict -Wshadow=global -fcheck-new -Wno-unused-result -fsized-deallocation -fstrict-overflow -flto-odr-type-merging -O2 -DVM_DEBUG=0 -o build/obj/release/src/akinator/akinator.cpp.obj -MD src/akinator/akinator.cpp
		In file included from include/akinator/akinator.h:3,
				         from src/akinator/akinator.cpp:1:
		src/akinator/akinator.cpp: In function ‘void akinator_find_node_by_value_callback_on_leave(TreeNode*, void*)’:
		src/akinator/akinator.cpp:102:71: error: invalid conversion from ‘char**’ to ‘const char**’ [-fpermissive]
		  102 |                 if (!vector_string_pop(callback_arg->characteristics, &value)) {
			  |                                                                       ^~~~~~
			  |                                                                       |
			  |                                                                       char**
		include/akinator/vector.h:67:115: note: in definition of macro ‘vector_string_pop’
		   67 | p(VECTOR_PTR, ITEM_PTR) vector_string_pop(MAKE_VARIABLE_LOCATION(VECTOR_PTR), VECTOR_PTR, ITEM_PTR)
			  |                                                                                           ^~~~~~~~

		In file included from include/akinator/akinator.h:3,
				         from src/akinator/akinator.cpp:1:
		include/akinator/vector.h:54:98: note:   initializing argument 3 of ‘bool vector_string_pop(Variable, void**, const char**)’
		   54 | TOR_POP_FUNCTION_NAME(Variable variable, VECTOR_TYPE_NAME* vector_ptr, VECTOR_ITEM_TYPE* item_ptr);
			  |                                                                                          ^

		make: *** [Makefile:115: build/obj/release/src/akinator/akinator.cpp.obj] Ошибка 1
		*/
		if (!vector_string_pop(callback_arg->characteristics, (const char**) &value)) {
			LOG_ERROR_FOR_NODE(node, "failed to pop the value");
			return;
		}
		
		assert(strcmp(node->value, value) == 0);
	
	}
}

TreeNode* akinator_find_node_by_value(Tree* tree, char* value, VectorString* characteristics) {
	assert(tree != NULL);
	
	TreeNode* result = NULL;
	FindNodeByValueArg arg = {value, &result, characteristics};
	
	tree_visit_depth_first(tree, akinator_find_node_by_value_callback_on_enter, akinator_find_node_by_value_callback_on_leave, &arg);
	
	return result;
}

// Check node doesn't exist.
int akinator_guess_object_callback(TreeNode* node, void* arg) {
	assert(node != NULL);
	
	TreeNode*** parent_link = (TreeNode***) arg;

	PRODUCE_OUTPUT("Это %s? [Да/Нет]\n", node->value);

	char line[MAX_LINE_LENGTH + 1] = {};

	while (read_line(line)) {
		if (strcmp(line, "Да") == 0) {
			if (node->left != NULL) {
				*parent_link = &node->left;
				return TREE_DIRECTION_LEFT;
			} else {
				PRODUCE_OUTPUT("Видишь, я умный.\n");
				return TREE_DIRECTION_NONE;
			}
		} else if (strcmp(line, "Нет") == 0) {
			if (node->right != NULL) {
				*parent_link = &node->right;
				return TREE_DIRECTION_RIGHT;
			} else {
				PRODUCE_OUTPUT("Не угадал. Кто это был?\n");
				if (!read_line(line)) {
					return TREE_DIRECTION_NONE;
				}
				
				PRODUCE_OUTPUT("А чем %s отличается от %s? ", line, node->value);
				PRODUCE_OUTPUT("Это ");

				char difference[MAX_LINE_LENGTH + 1] = {};
				if (!read_line(difference)) {
					return false;
				}

				TreeNode* object_node = NULL;
				if (!tree_node_init(&object_node, line)) {
					return TREE_DIRECTION_NONE;
				}

				TreeNode* question_node = NULL;
				if (!tree_node_init(&question_node, difference)) {
					tree_node_deinit(&object_node);
					return TREE_DIRECTION_NONE;
				}

				question_node->left = object_node;
				question_node->right = node;

				**parent_link = question_node;
				return TREE_DIRECTION_NONE;
			}
		} else {
			PRODUCE_OUTPUT("Не понял. Повтори пожалуйста.\n");
		}
	}

	return TREE_DIRECTION_NONE;
}

void akinator_process_guess_request(Tree* tree) {
	TreeNode** parent_link = &tree->root;
	tree_visit_depth_first(tree, akinator_guess_object_callback, NULL, &parent_link);
}

void akinator_process_comparison_request(Tree* tree) {
	(void) tree;
}

void akinator_process_definition_request(Tree* tree) {
	(void) tree;
}

