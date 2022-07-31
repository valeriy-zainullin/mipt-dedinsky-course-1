#include "tokenizer.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

// TODO: move to a separate header if needed later in other files.
#define UNREACHABLE assert(false); __builtin_unreachable();

static void print_escaped_string(char const* string) {
	for (; *string != '\0'; ++string) {
		if (*string == '"') {
			printf("\\");
		}
		printf("%c", *string);
	}
}

static void print_token(char const* label, char const* value, struct tokenizer_location const* location) {
	printf("<%s:\"", label);
	print_escaped_string(value);
	printf("\"@??.c:%zu,%zu> ", location->line, location->col);
}

int tokenizer_handle_keyword(char const* text) {
	print_token("keyword", text, (struct tokenizer_location const*) &tokenizer_current_location);
	tokenizer_notify_token_handled();
	return 1;
}

int tokenizer_handle_punctuator(char const* text) {
	print_token("punctuator", text, (struct tokenizer_location const*) &tokenizer_current_location);
	tokenizer_notify_token_handled();
	return 1;
}

int tokenizer_handle_identifier(char const* text) {
	print_token("identifier", text, (struct tokenizer_location const*) &tokenizer_current_location);
	tokenizer_notify_token_handled();
	return 1;
}

int tokenizer_handle_integer_constant(char const* text) {
	print_token("integer-constant", text, (struct tokenizer_location const*) &tokenizer_current_location);
	tokenizer_notify_token_handled();
	return 1;
}

int tokenizer_handle_floating_constant(char const* text) {
	print_token("floating-constant", text, (struct tokenizer_location const*) &tokenizer_current_location);
	tokenizer_notify_token_handled();
	return 1;
}

int tokenizer_handle_character_constant(char const* text) {
	print_token("character-constant", text, (struct tokenizer_location const*) &tokenizer_current_location);
	tokenizer_notify_token_handled();
	return 1;
}

int tokenizer_handle_string_literal(char const* text) {
	print_token("string-literal", text, (struct tokenizer_location const*) &tokenizer_current_location); 
	tokenizer_notify_token_handled();
	return 1;
}

int tokenizer_handle_invalid_token(char const* text) {
	printf("*yytext = %c\n", *text);
	/* Trigger an error. Unknown token. But returning -1 here is a temporary solution. */
	/* Better to do some recovery. */
	printf("Unknown token.\n");
	// TODO: check if it is needed to be done.
	// tokenizer_notify_token_handled();
	return -1;
}

int main() {
	// As far as I remember, zero is EOF, negative is error and positive should be our token index.
	while (yylex() > 0);
	printf("\n");
	return 0;
}
