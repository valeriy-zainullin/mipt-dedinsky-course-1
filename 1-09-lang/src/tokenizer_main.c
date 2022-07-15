#include <assert.h>
#include <stdio.h>

#include "token.h"

int yylex();

// TODO: move to a separate header if needed later in other files.
#define UNREACHABLE assert(false); __builtin_unreachable();

int token_process_keyword(char const* text) {
	printf("keyword: %s\n", text);
	return 1;
}

int token_process_identifier(char const* text) {
	printf("identifier: %s\n", text);
	return 1;
}

int token_process_integer_constant(enum token_integer_constant_base base, char const* text) {
	switch (base) {
		case TOKEN_INTEGER_CONSTANT_DECIMAL_BASE: {
			printf("integer_constant: decimal_constant, %s\n", text);
			break;
		}
		
		case TOKEN_INTEGER_CONSTANT_OCTAL_BASE: {
			printf("integer_constant: octal_constant, %s\n", text);
			break;
		}
		
		case TOKEN_INTEGER_CONSTANT_HEXADECIMAL_BASE: {
			printf("integer_constant: hexadecimal_constant, %s\n", text);
			break;
		}
		
		default: UNREACHABLE;
	}
	
	return 1;
}


int token_process_floating_constant(enum token_floating_constant_base base, char const* text) {
	switch (base) {
		case TOKEN_FLOATING_CONSTANT_DECIMAL_BASE: {
			printf("floating_constant: decimal_floating_constant, %s\n", text);
			break;
		}
		
		case TOKEN_FLOATING_CONSTANT_HEXADECIMAL_BASE: {
			printf("floating_constant: hexadecimal_floating_constant, %s\n", text);
			break;
		}
		
		default: UNREACHABLE;
	}
	
	return 1;
}

int token_process_character_constant(char const* text) {
	printf("character_constant: %s\n", text);
	
	return 1;
}

int token_process_string_literal(char const* text) {
	printf("string_literal: %s\n", text);
	return 1;
}

int token_process_punctuator(char const* text) {
	printf("punctuator: %s\n", text);
	return 1;
}

int token_process_invalid_token(char const* text) {
	printf("*yytext = %c\n", *text);
	/* Trigger an error. Unknown token. But returning -1 here is a temporary solution. */
	/* Better to do some recovery. */
	printf("Unknown token.\n");
	return -1;
}

int main() {
	// As far as I remember, zero is EOF, negative is error and positive should be our token index.
	while (yylex() > 0);
	return 0;
}
