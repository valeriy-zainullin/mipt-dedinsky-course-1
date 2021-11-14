#include "tree/tree.h"

#include "macro_utils.h"

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define PRODUCE_OUTPUT(...) printf(__VA_ARGS__); say(__VA_ARGS__)

#define LOG_ERROR(...) (void) (__VA_ARGS__)

#define MAX_SAID_STRING_LENGTH_MACRO 1024
#define MAX_COMMAND_LENGTH_MACRO 2048

static const size_t MAX_SAID_STRING_LENGTH = MAX_SAID_STRING_LENGTH_MACRO;
static const size_t MAX_COMMAND_LENGTH = MAX_COMMAND_LENGTH_MACRO;

enum AkinatorMode {
	AKINATOR_MODE_GUESS,
	AKINATOR_MODE_COMPARE,
	AKINATOR_MODE_GET_DEFINITION,
	AKINATOR_MODE_EXIT,
	AKINATOR_MODE_COUNT
};

// Запретить одинарную и двойную ковычки.
static void say(const char * format, ...) {
	va_list list;

	char said_string[MAX_SAID_STRING_LENGTH + 1] = {};

	va_start(list, format);
	vsnprintf(said_string, MAX_SAID_STRING_LENGTH, format, list);
	va_end(list);

	char command[MAX_COMMAND_LENGTH + 1] = {};

	snprintf(
		command,
		MAX_COMMAND_LENGTH,

		"festival \""
		"(set! male1 voice_msu_ru_nsh_clunits)"
		"(male1)"
		"(Parameter.set 'Language 'russian)"
		"(SayText \\\"%s\\\")"
		"\"",

		said_string
	);

	fputs(command, stderr);

	system(command);
}

static AkinatorMode read_mode() {
	PRODUCE_OUTPUT("Что хочешь, дорогой?");
	PRODUCE_OUTPUT("0 --- угадать объект.\n");
	PRODUCE_OUTPUT("1 --- сравнить объекты.\n");
	PRODUCE_OUTPUT("2 --- дать определение объекту.\n");
	PRODUCE_OUTPUT("3 --- выйти (с обновлением базы данных).\n");

	fflush(stdout);

	int choice = 0;

	while (true) {
		int num_read = scanf("%d", &choice);

		if (num_read < 0) {
			LOG_ERROR("Не удалось прочитать выбор.\n");

			if (feof(stdin)) {
				break;
			}
		}
	
		if (0 <= choice && choice < (int) AKINATOR_MODE_COUNT) {
			return (AkinatorMode) choice;
		}
	}

	return AKINATOR_MODE_EXIT;
}

static void process_guess() {}

static void process_compare() {}

static void process_get_definition() {}

int main() {

	while (true) {
		AkinatorMode mode = read_mode();

		switch (mode) {
			case AKINATOR_MODE_GUESS:          process_guess();          break;
			case AKINATOR_MODE_COMPARE:        process_compare();        break;
			case AKINATOR_MODE_GET_DEFINITION: process_get_definition(); break;
			case AKINATOR_MODE_EXIT:                                     break;

			case AKINATOR_MODE_COUNT:
			default: UNREACHABLE;
		}
	}

	return 0;
}
