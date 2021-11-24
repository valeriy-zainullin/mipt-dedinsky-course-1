#include "support/tts.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

static const size_t MAX_SAID_STRING_LENGTH = TTS_MAX_SAID_STRING_LENGTH_MACRO;
static const size_t MAX_COMMAND_LENGTH = TTS_MAX_COMMAND_LENGTH_MACRO;

// Запретить одинарную и двойную ковычки.
void tts_say(const char * format, ...) {
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

	// fputs(command, stderr);

	((void) command);

	// system(command);
}

